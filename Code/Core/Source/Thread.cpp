// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformThread.cpp
* @author JXMaster
* @date 2019/12/28
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT
#include <Core/Core.hpp>
#include "Thread.hpp"
#include <Runtime/Thread.hpp>

namespace Luna
{
	Unconstructed<MainThread> g_main_thread;
	handle_t g_tls_thread;

	static void thread_callback(void* cookie)
	{
		UserThread* t = (UserThread*)cookie;
		luassert_always(succeeded(tls_set(g_tls_thread, (IThread*)t)));
		t->m_callback->run();
	}

	static void fiber_callback(void* cookie)
	{
		Fiber* f = (Fiber*)cookie;
		f->m_callback->run();
		f->m_state = EFiberState::finished;
		while (true)
		{
			auto _ = yield_current_fiber();
		}
	}

	LUNA_CORE_API RP<IThread> new_thread(IRunnable* callback, u32 stack_size)
	{
		P<UserThread> t = newobj<UserThread>();
		t->m_callback = callback;
		auto r = Platform::new_thread(thread_callback, t.get(), stack_size);
		if (failed(r))
		{
			return r.errcode();
		}
		t->m_handle = r.get();
		return t;
	}
	Thread* get_current_thread_internal()
	{
		Thread* t = (Thread*)tls_get(g_tls_thread);
		return t;
	}
	void thread_init()
	{
		g_tls_thread = tls_alloc().get();
		// Init main thread object.
		g_main_thread.construct();
		g_main_thread.get().m_handle = Platform::get_current_thread();
		luassert_always(succeeded(tls_set(g_tls_thread, (Thread*)&(g_main_thread.get()))));
	}
	void thread_close()
	{
		g_main_thread.destruct();
		tls_free(g_tls_thread);
	}
	LUNA_CORE_API IThread* get_current_thread()
	{
		IThread* t = (Thread*)tls_get(g_tls_thread);
		return t;
	}
	LUNA_CORE_API IThread* get_main_thread()
	{
		return &(g_main_thread.get());
	}
	LUNA_CORE_API RP<IFiber> convert_thread_to_fiber()
	{
		Thread* t = get_current_thread_internal();
		if (t->get_native_fiber())
		{
			return P<IFiber>(t->get_native_fiber());
		}
		auto rhandle = Platform::convert_thread_to_fiber();
		if (failed(rhandle))
		{
			return rhandle.errcode();
		}
		handle_t handle = rhandle.get();
		P<Fiber> f = newobj<Fiber>();
		f->m_state = EFiberState::running;	// The default fiber running by default.
		f->m_fiber = handle;
		f->m_is_native_fiber = true;
		t->set_native_fiber(f);
		t->set_fiber(f);
		return f;
	}
	LUNA_CORE_API RV convert_fiber_to_thread()
	{
		Thread* t = get_current_thread_internal();
		if (!t->get_native_fiber())
		{
			return BasicError::bad_calling_time();
		}
		lucheck_msg(t->get_native_fiber() == t->get_fiber(), "Trying to convert a fiber to thread when a non-default fiber is running on this thread. This is not allowed.");
		if (!::ConvertFiberToThread())
		{
			return BasicError::bad_system_call();
		}
		t->get_native_fiber()->m_fiber = NULL;
		t->set_native_fiber(nullptr);
		return RV();
	}
	LUNA_CORE_API RP<IFiber> new_fiber(IRunnable* callback, u32 stack_size)
	{
		lucheck(callback);
		if (!stack_size)
		{
			stack_size = (u32)1_mb;
		}
		P<Fiber> f = newobj<Fiber>();
		f->m_callback = callback;
		auto rf = Platform::new_fiber(fiber_callback, f.get(), stack_size);
		if (failed(rf))
		{
			return BasicError::bad_system_call();
		}
		f->m_fiber = rf.get();
		return f;
	}
	LUNA_CORE_API IFiber* get_current_fiber()
	{
		Thread* t = (Thread*)tls_get(g_tls_thread);
		return t->get_fiber();
	}

	LUNA_CORE_API RV resume_fiber(IFiber* fiber)
	{
		lucheck(fiber);
		Thread* t = (Thread*)tls_get(g_tls_thread);
		Fiber* old_f = t->get_fiber();
		if (!old_f)
		{
			return BasicError::bad_calling_time();
		}
		Fiber* new_f = static_cast<Fiber*>(fiber);
		if ((atom_compare_exchange_u32((u32*)&(new_f->m_state), (u32)EFiberState::running, (u32)EFiberState::ready) == (u32)EFiberState::ready) ||
			(atom_compare_exchange_u32((u32*)&(new_f->m_state), (u32)EFiberState::running, (u32)EFiberState::suspended) == (u32)EFiberState::suspended))
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
			return RV();
		}
		return BasicError::failure();
	}

	LUNA_CORE_API RV yield_current_fiber()
	{
		Thread* t = (Thread*)tls_get(g_tls_thread);
		Fiber* old_f = t->get_fiber();
		if (!old_f)
		{
			return BasicError::bad_calling_time();
		}
		Fiber* new_f = old_f->m_waiting_fiber;
		if (!new_f)
		{
			// The user calls this function when no fiber is resumed.
			return BasicError::bad_calling_time();
		}
		::SwitchToFiber(new_f->m_fiber);	// Return to middle of `resume_fiber`.
		return RV();
	}
}