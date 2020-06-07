// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Semaphore.hpp
* @author JXMaster
* @date 2018/12/22
* @brief Windows implementation of Sync System.
*/
#pragma once
#include "../../CoreHeader.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
namespace luna
{
	namespace win
	{
		class Semaphore final : public ISemaphore
		{
		public:
			lucid("{4d155da3-acdb-4ac6-aecb-70e43a5faedf}");
			luiimpl(Semaphore, ISemaphore, IWaitable, IObject);
			

			HANDLE m_handle;

			Semaphore(IAllocator* alloc) :
				luibind(alloc),
				m_handle(NULL) {}

			~Semaphore()
			{
				if (m_handle)
				{
					::CloseHandle(m_handle);
					m_handle = NULL;
				}
			}

			virtual void wait() override
			{
				if (::WaitForSingleObject(m_handle, INFINITE) != WAIT_OBJECT_0)
				{
					lupanic_always();
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
			virtual void unacquire(int32 releaseCount) override
			{
				::ReleaseSemaphore(m_handle, releaseCount, NULL);
			}
		};
	}
}
#endif
