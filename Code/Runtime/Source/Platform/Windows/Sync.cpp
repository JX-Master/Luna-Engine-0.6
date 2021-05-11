// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Sync.cpp
* @author JXMaster
* @date 2019/12/26
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_WINDOWS

#include <Runtime/Assert.hpp>
#include "../../OS.hpp"
#include "MiniWin.hpp"

namespace Luna
{
	namespace OS
	{
		struct Signal
		{
			CRITICAL_SECTION m_cs;
			CONDITION_VARIABLE m_cv;
			volatile bool m_signaled;
			volatile bool m_manual_reset;

			Signal()
			{
				::InitializeCriticalSection(&m_cs);
				::InitializeConditionVariable(&m_cv);
				m_signaled = false;
			}
			~Signal()
			{
				::DeleteCriticalSection(&m_cs);
			}
		};
		handle_t new_signal(bool manual_reset)
		{
			Signal* sig = memnew<Signal>();
			sig->m_manual_reset = manual_reset;
			return sig;
		}
		void delete_signal(handle_t sig)
		{
			Signal* o = (Signal*)sig;
			memdelete(o);
		}
		void wait_signal(handle_t sig)
		{
			Signal* o = (Signal*)sig;
			::EnterCriticalSection(&o->m_cs);
			// If the signal is not signaled, waits until it gets signaled.
			while (!o->m_signaled)
			{
				if (!::SleepConditionVariableCS(&o->m_cv, &o->m_cs, INFINITE))
				{
					//! ERROR_TIMEOUT is impossible for INFINITE wait time.
					lupanic_msg_always("Failed to wait for signal");
				}
			}
			// If not manual reset, consumes the signal so that other waiting threads
			// can not go here.
			if (!o->m_manual_reset)
			{
				o->m_signaled = false;
			}
			::LeaveCriticalSection(&o->m_cs);
		}
		RV try_wait_signal(handle_t sig)
		{
			Signal* o = (Signal*)sig;
			::EnterCriticalSection(&o->m_cs);
			if (!o->m_signaled)
			{
				if (!::SleepConditionVariableCS(&o->m_cv, &o->m_cs, 0))
				{
					// Failed to wait.
					::LeaveCriticalSection(&o->m_cs);
					DWORD err = ::GetLastError();
					if (err == ERROR_TIMEOUT)
					{
						return BasicError::timeout();
					}
					return BasicError::bad_system_call();
				}
			}
			// If not manual reset, consumes the signal so that other waiting threads
			// can not go here.
			if (!o->m_manual_reset)
			{
				o->m_signaled = false;
			}
			::LeaveCriticalSection(&o->m_cs);
			return RV();
		}
		void trigger_signal(handle_t sig)
		{
			Signal* o = (Signal*)sig;
			::EnterCriticalSection(&o->m_cs);
			o->m_signaled = true;
			if (o->m_manual_reset)
			{
				// Wake all threads.
				::WakeAllConditionVariable(&o->m_cv);
			}
			else
			{
				// Wake exactly one thread.
				::WakeConditionVariable(&o->m_cv);
			}
			::LeaveCriticalSection(&o->m_cs);
		}
		void reset_signal(handle_t sig)
		{
			Signal* o = (Signal*)sig;
			::EnterCriticalSection(&o->m_cs);
			o->m_signaled = false;
			::LeaveCriticalSection(&o->m_cs);
		}
		handle_t new_mutex()
		{
			CRITICAL_SECTION* cs = memnew<CRITICAL_SECTION>();
			::InitializeCriticalSection(cs);
			return cs;
		}
		void delete_mutex(handle_t mtx)
		{
			CRITICAL_SECTION* cs = (CRITICAL_SECTION*)mtx;
			::DeleteCriticalSection(cs);
			memdelete(cs);
		}
		void lock_mutex(handle_t mtx)
		{
			CRITICAL_SECTION* cs = (CRITICAL_SECTION*)mtx;
			::EnterCriticalSection(cs);
		}
		RV try_lock_mutex(handle_t mtx)
		{
			CRITICAL_SECTION* cs = (CRITICAL_SECTION*)mtx;
			if (::TryEnterCriticalSection(cs))
			{
				return RV();
			}
			return BasicError::timeout();
		}
		void unlock_mutex(handle_t mtx)
		{
			CRITICAL_SECTION* cs = (CRITICAL_SECTION*)mtx;
			::LeaveCriticalSection(cs);
		}
		handle_t new_semaphore(i32 initial_count, i32 max_count)
		{
			HANDLE h = ::CreateSemaphoreW(NULL, initial_count, max_count, NULL);
			if (!h)
			{
				lupanic_msg_always("CreateSemaphoreW failed.");
			}
			return h;
		}
		void delete_semaphore(handle_t sema)
		{
			HANDLE h = (HANDLE)sema;
			::CloseHandle(h);
		}
		void acquire_semaphore(handle_t sema)
		{
			HANDLE h = (HANDLE)sema;
			if (::WaitForSingleObject(h, INFINITE) != WAIT_OBJECT_0)
			{
				lupanic_msg_always("WaitForSingleObject failed.");
			}
		}
		RV try_acquire_semaphore(handle_t sema)
		{
			HANDLE h = (HANDLE)sema;
			DWORD wait_result = ::WaitForSingleObject(h, 0);
			if (wait_result == WAIT_OBJECT_0)
			{
				return RV();
			}
			if (wait_result == WAIT_TIMEOUT)
			{
				return BasicError::timeout();
			}
			return BasicError::bad_system_call();
		}
		void release_semaphore(handle_t sema)
		{
			HANDLE h = (HANDLE)sema;
			::ReleaseSemaphore(h, 1, NULL);
		}
	}
}

#endif