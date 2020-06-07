// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Thread.hpp
* @author JXMaster
* @date 2018/12/21
*/
#pragma once
#include "../../CoreHeader.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
#include "Fiber.hpp"
#include "../../Error/Error.hpp"
namespace luna
{
	namespace win
	{
		struct IWindowsThread : public IThread
		{
			luiid("{f1154226-a632-46bd-b7dd-5882eb9294ee}");

			virtual HANDLE get_handle() = 0;
			virtual void set_handle(HANDLE handle) = 0;
			virtual Fiber* get_native_fiber() = 0;
			virtual void set_native_fiber(Fiber* handle) = 0;
			virtual Fiber* get_fiber() = 0;
			virtual void set_fiber(Fiber* handle) = 0;
			virtual ThreadErrorContext* get_err() = 0;
		};

		class UserThread : public IWindowsThread
		{
		public:
			lucid("{a29d30a1-e572-4e61-9e3e-5083b3e0ff64}");
			luiimpl(UserThread, IWindowsThread, IThread, IWaitable, IObject);
			

			HANDLE m_handle;		// Thread handle.

			P<IRunnable> m_callback;
			P<Fiber> m_fiber;			// The fiber running on this thread.
			P<Fiber> m_native_fiber;	// The fiber converted from thread.

			ThreadErrorContext m_err;

			UserThread(IAllocator* alloc) :
				luibind(alloc),
				m_handle(NULL) {}

			~UserThread()
			{
				if (m_handle)
				{
					wait();
					::CloseHandle(m_handle);
					m_handle = nullptr;
				}
			}

			virtual void set_priority(EThreadPriority priority) override
			{
				switch (priority)
				{
				case EThreadPriority::low:
					::SetThreadPriority(m_handle, THREAD_PRIORITY_LOWEST);
					break;
				case EThreadPriority::normal:
					::SetThreadPriority(m_handle, THREAD_PRIORITY_NORMAL);
					break;
				case EThreadPriority::high:
					::SetThreadPriority(m_handle, THREAD_PRIORITY_HIGHEST);
					break;
				case EThreadPriority::critical:
					::SetThreadPriority(m_handle, THREAD_PRIORITY_TIME_CRITICAL);
					break;
				}
			}
			virtual void wait() override
			{
				if (::WaitForSingleObject(m_handle, INFINITE) != WAIT_OBJECT_0)
				{
					lupanic_msg_always("WaitForSingleObject failed.");
				}
			}
			virtual RV try_wait() override
			{
				if (::WaitForSingleObject(m_handle, 0) == WAIT_OBJECT_0)
				{
					return s_ok;
				}
				return e_failure;
			}

			virtual HANDLE get_handle() override
			{
				return m_handle;
			}
			virtual void set_handle(HANDLE handle) override
			{
				m_handle = handle;
			}
			virtual Fiber* get_native_fiber() override
			{
				return m_native_fiber.get();
			}
			virtual void set_native_fiber(Fiber* handle) override
			{
				m_native_fiber = handle;
			}
			virtual Fiber* get_fiber() override
			{
				return m_fiber.get();
			}
			virtual void set_fiber(Fiber* handle) override
			{
				m_fiber = handle;
			}
			virtual ThreadErrorContext* get_err() override
			{
				return &m_err;
			}
		};

		class MainThread : public IWindowsThread
		{
		public:
			lucid("{384494c9-298b-47b8-af1f-83e26ecd429a}");
			luiimpl_static(MainThread, IWindowsThread, IThread, IWaitable, IObject);
			

			HANDLE m_handle;		// Thread handle.
			P<Fiber> m_fiber;			// The fiber running on this thread.
			P<Fiber> m_native_fiber;	// The fiber converted from thread.

			ThreadErrorContext m_err;

			MainThread() :
				m_handle(NULL) {}

			virtual void set_priority(EThreadPriority priority) override
			{
				switch (priority)
				{
				case EThreadPriority::low:
					::SetThreadPriority(m_handle, THREAD_PRIORITY_LOWEST);
					break;
				case EThreadPriority::normal:
					::SetThreadPriority(m_handle, THREAD_PRIORITY_NORMAL);
					break;
				case EThreadPriority::high:
					::SetThreadPriority(m_handle, THREAD_PRIORITY_HIGHEST);
					break;
				case EThreadPriority::critical:
					::SetThreadPriority(m_handle, THREAD_PRIORITY_TIME_CRITICAL);
					break;
				}
			}
			virtual void wait() override
			{
				lupanic_msg_always("The main thread cannot be waited, since it never returns.");
			}
			virtual RV try_wait() override
			{
				// The main thread cannot be waited.
				return e_not_available;
			}

			virtual HANDLE get_handle() override
			{
				return m_handle;
			}
			virtual void set_handle(HANDLE handle) override
			{
				m_handle = handle;
			}
			virtual Fiber* get_native_fiber() override
			{
				return m_native_fiber.get();
			}
			virtual void set_native_fiber(Fiber* handle) override
			{
				m_native_fiber = handle;
			}
			virtual Fiber* get_fiber() override
			{
				return m_fiber.get();
			}
			virtual void set_fiber(Fiber* handle) override
			{
				m_fiber = handle;
			}
			virtual ThreadErrorContext* get_err() override
			{
				return &m_err;
			}
		};
	}
}
#endif