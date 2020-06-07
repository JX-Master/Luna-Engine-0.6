// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Signal.hpp
* @author JXMaster
* @date 2018/12/22
* @brief Windows implementation of Sync System.
*/
#pragma once
#include "../../CoreHeader.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#define LUNA_USE_LIGHT_EVENT
namespace luna
{
	namespace win
	{
		class Signal final : public ISignal
		{
		public:
			lucid("{95a2e5b2-d48a-4f19-bfb8-22c273c0ad4b}");
			luiimpl(Signal, ISignal, IWaitable, IObject);
			

#ifdef LUNA_USE_LIGHT_EVENT
			CRITICAL_SECTION m_cs;
			CONDITION_VARIABLE m_cv;
			volatile bool m_signaled;
			volatile bool m_manual_reset;
#else
			HANDLE m_handle;
#endif

#ifdef LUNA_USE_LIGHT_EVENT
			Signal(IAllocator* alloc) :
				luibind(alloc)
			{
				::InitializeCriticalSection(&m_cs);
				::InitializeConditionVariable(&m_cv);
				m_signaled = false;
			}
			~Signal()
			{
				::DeleteCriticalSection(&m_cs);
			}
#else
			Signal(IAllocator* alloc) :
				luibind(alloc),
				m_handle(NULL) {}
			~Signal()
			{
				if (m_handle)
				{
					::CloseHandle(m_handle);
					m_handle = NULL;
				}
			}
#endif

			virtual void wait() override
			{
#ifdef LUNA_USE_LIGHT_EVENT
				::EnterCriticalSection(&m_cs);
				// If the signal is not signaled, waits until it gets signaled.
				while (!m_signaled)
				{
					if (!::SleepConditionVariableCS(&m_cv, &m_cs, INFINITE))
					{
						// Failed to wait. This should never happen if the wait time is INFINITE.
						::LeaveCriticalSection(&m_cs);
						lupanic_always();
					}
				}
				// If not manual reset, consumes the signal so that other waiting threads
				// can not go here.
				if (!m_manual_reset)
				{
					m_signaled = false;
				}
				::LeaveCriticalSection(&m_cs);
#else
				if (!::WaitForSingleObject(m_handle, INFINITE) == WAIT_OBJECT_0)
				{
					// Failed to wait. This should never happen if the wait time is INFINITE.
					lupanic_always();
				}
#endif
			}
			virtual RV try_wait() override
			{
#ifdef LUNA_USE_LIGHT_EVENT
				if (!::TryEnterCriticalSection(&m_cs))
				{
					return e_failure;
				}
				if (!m_signaled)
				{
					if (!::SleepConditionVariableCS(&m_cv, &m_cs, 0))
					{
						// Failed to wait.
						::LeaveCriticalSection(&m_cs);
						return e_failure;
					}
				}
				// If not manual reset, consumes the signal so that other waiting threads
				// can not go here.
				if (!m_manual_reset)
				{
					m_signaled = false;
				}
				::LeaveCriticalSection(&m_cs);
				return s_ok;
#else
				if (::WaitForSingleObject(m_handle, 0) == WAIT_OBJECT_0)
				{
					return s_ok;
				}
				return e_failure;
#endif
			}
			virtual void trigger() override
			{
#ifdef LUNA_USE_LIGHT_EVENT
				::EnterCriticalSection(&m_cs);
				m_signaled = true;
				if (m_manual_reset)
				{
					// Wake all threads.
					::WakeAllConditionVariable(&m_cv);
				}
				else
				{
					// Wake exactly one thread.
					::WakeConditionVariable(&m_cv);
				}
				::LeaveCriticalSection(&m_cs);
#else
				::SetEvent(m_handle);
#endif
			}
			virtual void reset() override
			{
#ifdef LUNA_USE_LIGHT_EVENT
				::EnterCriticalSection(&m_cs);
				m_signaled = false;
				::LeaveCriticalSection(&m_cs);
#else
				::ResetEvent(m_handle);
#endif
			}
		};
	}
}
#endif
