// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Thread.hpp
* @author JXMaster
* @date 2018/12/21
*/
#pragma once
#include "../IThread.hpp"
#include "Fiber.hpp"
namespace Luna
{
	struct Thread : public IThread
	{
		luiid("{f1154226-a632-46bd-b7dd-5882eb9294ee}");

		virtual handle_t get_handle() = 0;
		virtual void set_handle(handle_t handle) = 0;
		virtual Fiber* get_native_fiber() = 0;
		virtual void set_native_fiber(Fiber* handle) = 0;
		virtual Fiber* get_fiber() = 0;
		virtual void set_fiber(Fiber* handle) = 0;
	};

	class UserThread : public Thread
	{
	public:
		lucid("{a29d30a1-e572-4e61-9e3e-5083b3e0ff64}");
		luiimpl(UserThread, Thread, IThread, IWaitable, IObject);
			
		handle_t m_handle;		// Thread handle.

		P<IRunnable> m_callback;
		P<Fiber> m_fiber;			// The fiber running on this thread.
		P<Fiber> m_native_fiber;	// The fiber converted from thread.

		UserThread() :
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

		virtual RV set_priority(EThreadPriority priority) override
		{
			return Platform::set_thread_priority(m_handle, (Platform::ThreadPriority)priority);
		}
		virtual void wait() override
		{
			Platform::wait_thread(m_handle);
		}
		virtual RV try_wait() override
		{
			return Platform::try_wait_thread(m_handle);
		}

		virtual handle_t get_handle() override
		{
			return m_handle;
		}
		virtual void set_handle(handle_t handle) override
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
	};

	class MainThread : public Thread
	{
	public:
		lucid("{384494c9-298b-47b8-af1f-83e26ecd429a}");
		luiimpl_static(MainThread, Thread, IThread, IWaitable, IObject);
			

		handle_t m_handle;		// Thread handle.
		P<Fiber> m_fiber;			// The fiber running on this thread.
		P<Fiber> m_native_fiber;	// The fiber converted from thread.

		MainThread() :
			m_handle(NULL) {}

		virtual RV set_priority(EThreadPriority priority) override
		{
			return Platform::set_thread_priority(m_handle, (Platform::ThreadPriority)priority);
		}
		virtual void wait() override
		{
			lupanic_msg_always("The main thread cannot be waited, since it never returns.");
		}
		virtual RV try_wait() override
		{
			// The main thread cannot be waited.
			return BasicError::not_supported();
		}

		virtual handle_t get_handle() override
		{
			return m_handle;
		}
		virtual void set_handle(handle_t handle) override
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
	};
}