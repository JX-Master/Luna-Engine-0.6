// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformThread.cpp
* @author JXMaster
* @date 2019/12/28
*/
#include "../../Platform.hpp"
#ifdef LUNA_PLATFORM_WINDOWS
#include "Thread.hpp"
#include "Heap.hpp"
#include "Fiber.hpp"

DWORD WINAPI WinThreadEntry(LPVOID cookie);
VOID __stdcall WinFiberEntry(LPVOID cookie);

namespace luna
{
	namespace platform
	{
		using namespace win;

		Unconstructed<MainThread> g_main_thread;
		DWORD g_tls_thread;

		RP<IThread> new_thread(IRunnable* callback, uint32 stack_size)
		{
			P<UserThread> t;
			t.attach(get_global_heap()->new_obj<UserThread>(get_global_heap()));
			t->m_callback = callback;
			DWORD tid;
			t->m_handle = ::CreateThread(NULL, stack_size, &WinThreadEntry, t.get(), CREATE_SUSPENDED, &tid);
			if (!t->m_handle)
			{
				// Throw Exception.
				DWORD dw = ::GetLastError();
				t = nullptr;
				return { nullptr, e_bad_system_call };
			}
			::ResumeThread(t->m_handle);
			return { t, s_ok };
		}
		IWindowsThread* get_current_thread_internal()
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			return t;
		}
		void platform_thread_init()
		{
			g_tls_thread = ::TlsAlloc();
			// Init main thread object.
			g_main_thread.construct();
			g_main_thread.get().m_handle = ::GetCurrentThread();
			::TlsSetValue(g_tls_thread, (IWindowsThread*)&(g_main_thread.get()));
		}
		void platform_thread_close()
		{
			g_main_thread.destruct();
			::TlsFree(g_tls_thread);
		}
		IThread* get_current_thread()
		{
			IThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			return t;
		}
		IThread* get_main_thread()
		{
			return &(g_main_thread.get());
		}
		void sleep(uint32 time_milliseconds)
		{
			::Sleep(time_milliseconds);
		}
		void fast_sleep(uint32 time_microseconds)
		{
			LARGE_INTEGER currentTime;
			::QueryPerformanceCounter(&currentTime);
			LARGE_INTEGER ticksPerSnd;
			::QueryPerformanceFrequency(&ticksPerSnd);
			uint64 endTime = currentTime.QuadPart + ticksPerSnd.QuadPart * time_microseconds / 1000000;
			if (time_microseconds > 4000) //4ms
			{
				::Sleep(time_microseconds / 1000);
			}
			::QueryPerformanceCounter(&currentTime);
			while ((uint64)(currentTime.QuadPart) < endTime)
			{
				::SwitchToThread();
				::SwitchToThread();
				::SwitchToThread();
				::SwitchToThread();
			}
		}
		void yield_current_thread()
		{
			::SwitchToThread();
		}
		RP<IFiber> convert_thread_to_fiber()
		{
			IWindowsThread* t = get_current_thread_internal();
			if (t->get_native_fiber())
			{
				return { P<IFiber>(t->get_native_fiber()), s_already_done };
			}
			LPVOID handle = ::ConvertThreadToFiber(NULL);
			P<Fiber> f;
			f.attach(get_global_heap()->new_obj<Fiber>(get_global_heap()));
			f->m_state = EFiberState::running;	// The default fiber running by default.
			f->m_fiber = handle;
			f->m_is_native_fiber = true;
			t->set_native_fiber(f);
			t->set_fiber(f);
			return { f, s_ok };
		}
		RV convert_fiber_to_thread()
		{
			IWindowsThread* t = get_current_thread_internal();
			if (!t->get_native_fiber())
			{
				return e_not_available;
			}
			luassert_msg_usr(t->get_native_fiber() == t->get_fiber(), "Trying to convert a fiber to thread when a non-default fiber is running on this thread. This is not allowed.");
			if (!::ConvertFiberToThread())
			{
				return e_bad_system_call;
			}
			t->get_native_fiber()->m_fiber = NULL;
			t->set_native_fiber(nullptr);
			return s_ok;
		}
		RP<IFiber> new_fiber(IRunnable* callback, uint32 stack_size)
		{
			luassert_usr(callback);
			if (!stack_size)
			{
				stack_size = (uint32)1_mb;
			}
			P<Fiber> f;
			f.attach(get_global_heap()->new_obj<Fiber>(get_global_heap()));
			f->m_callback = callback;
			f->m_fiber = ::CreateFiber(stack_size, WinFiberEntry, f.get());
			if (!f->m_fiber)
			{
				f = nullptr;
				return { nullptr, e_bad_system_call };
			}
			return { f, s_ok };
		}
		IFiber* get_current_fiber()
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			return t->get_fiber();
		}

		RV resume_fiber(IFiber* fiber)
		{
			luassert_usr(fiber);
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			Fiber* old_f = t->get_fiber();
			if (!old_f)
			{
				return e_not_available;
			}
			Fiber* new_f = static_cast<Fiber*>(fiber);
			if ((atom_compare_exchange_u32((uint32*)&(new_f->m_state), (uint32)EFiberState::running, (uint32)EFiberState::ready) == (uint32)EFiberState::ready) ||
				(atom_compare_exchange_u32((uint32*)&(new_f->m_state), (uint32)EFiberState::running, (uint32)EFiberState::suspended) == (uint32)EFiberState::suspended))
			{
				old_f->m_state = EFiberState::waiting;
				new_f->m_waiting_fiber = old_f;
				t->set_fiber(new_f);
				::SwitchToFiber(new_f->m_fiber);
				t->set_fiber(old_f);
				new_f->m_waiting_fiber = nullptr;
				if (new_f->m_state != EFiberState::finished)
				{
					new_f->m_state = EFiberState::suspended;
				}
				return s_ok;
			}
			return e_failure;
		}

		RV yield_current_fiber()
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			Fiber* old_f = t->get_fiber();
			if (!old_f)
			{
				return e_not_available;
			}
			Fiber* new_f = old_f->m_waiting_fiber;
			if (!new_f)
			{
				// The user calls this function when no fiber is resumed.
				return e_bad_calling_time;
			}
			::SwitchToFiber(new_f->m_fiber);	// Return to middle of `resume_fiber`.
			return s_ok;
		}

		RV tls_alloc(handle_t* handle)
		{
			DWORD index = TlsAlloc();
			if (index == TLS_OUT_OF_INDEXES)
			{
				*handle = nullptr;
				return e_failure;
			}
			*handle = (handle_t)index;
			return s_ok;
		}
		RV tls_free(handle_t handle)
		{
			if (TlsFree((DWORD)handle))
			{
				return s_ok;
			}
			return e_failure;
		}
		RV tls_set(handle_t handle, void* ptr)
		{
			if (TlsSetValue((DWORD)handle, ptr))
			{
				return s_ok;
			}
			return e_failure;
		}
		RV tls_get(handle_t handle, void** ptr)
		{
			void* d = ::TlsGetValue((DWORD)handle);
			DWORD err = ::GetLastError();
			if (err == ERROR_SUCCESS)
			{
				*ptr = d;
				return s_ok;
			}
			return e_failure;
		}
		void clear_err()
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			t->get_err()->clear_err();
		}
		void set_verr(result_t result_code, const char* msgfmt, va_list args)
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			t->get_err()->set_verr(result_code, msgfmt, args);
		}
		void set_verr_var(result_t result_code, IVariant* variant, const char* msgfmt, va_list args)
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			t->get_err()->set_verr_var(result_code, variant, msgfmt, args);
		}
		void set_verr_user(const Guid& result_domain, result_t result_code, const char * msgfmt, va_list args)
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			t->get_err()->set_verr_user(result_domain, result_code, msgfmt, args);
		}
		void set_verr_var_user(const Guid& result_domain, result_t result_code, IVariant * variant, const char * msgfmt, va_list args)
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			t->get_err()->set_verr_var_user(result_domain, result_code, variant, msgfmt, args);
		}
		const char* err_msg()
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			return t->get_err()->err_msg();
		}
		const Guid& err_domain()
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			return t->get_err()->err_domain();
		}
		result_t err_result()
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			return t->get_err()->err_result();
		}
		IVariant* err_var()
		{
			IWindowsThread* t = (IWindowsThread*)::TlsGetValue(g_tls_thread);
			return t->get_err()->err_var();
		}
	}
}

using namespace luna;
using namespace luna::win;
using namespace luna::platform;

DWORD WINAPI WinThreadEntry(LPVOID cookie)
{
	UserThread* t = (UserThread*)cookie;
	TlsSetValue(g_tls_thread, (IWindowsThread*)t);
	return t->m_callback->run();
}

VOID __stdcall WinFiberEntry(LPVOID cookie)
{
	Fiber* f = (Fiber*)cookie;
	f->m_callback->run();
	f->m_state = EFiberState::finished;
	while (true)
	{
		auto _ = luna::platform::yield_current_fiber();
	}
}

#endif