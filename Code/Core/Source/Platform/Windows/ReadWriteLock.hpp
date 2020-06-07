// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ReadWriteLock.hpp
* @author JXMaster
* @date 2019/12/9
*/
#pragma once
#include "../../CoreHeader.hpp"
#ifdef LUNA_PLATFORM_WINDOWS
namespace luna
{
	namespace win
	{
		class ReadWriteLock : public IReadWriteLock
		{
		public:
			luiimpl(ReadWriteLock, IReadWriteLock, IObject);
			
			SRWLOCK m_lock;
			volatile HANDLE m_write_thread;
			volatile size_t m_write_count;

			ReadWriteLock(IAllocator* alloc) :
				luibind(alloc)
			{
				InitializeSRWLock(&m_lock);
				m_write_thread = NULL;
				m_write_count = 0;
			}

			virtual void read_lock() override
			{
				AcquireSRWLockShared(&m_lock);
			}
			virtual RV try_read_lock() override
			{
				if (TryAcquireSRWLockShared(&m_lock) == 0) {
					return e_failure;
				}
				else 
				{
					return s_ok;
				}
			}
			virtual void read_unlock() override
			{
				ReleaseSRWLockShared(&m_lock);
			}
			virtual void write_lock() override
			{
				if (::GetCurrentThread() == m_write_thread)
				{
					++m_write_count;
					return;
				}
				AcquireSRWLockExclusive(&m_lock);
				m_write_count = 1;
				m_write_thread = ::GetCurrentThread();
			}
			virtual RV try_write_lock() override
			{
				if (::GetCurrentThread() == m_write_thread)
				{
					++m_write_count;
					return s_ok;
				}
				if (TryAcquireSRWLockExclusive(&m_lock) == 0) {
					return e_failure;
				}
				else 
				{
					m_write_count = 1;
					m_write_thread = ::GetCurrentThread();
					return s_ok;
				}
			}
			virtual void write_unlock() override
			{
				luassert_msg_usr(m_write_count, "write_unlock() called when no thread calls write_lock() before.");
				luassert_msg_usr(m_write_thread == ::GetCurrentThread(), "write_unlock() of Read Write Lock must be called by the same thread that calls write_lock().");
				--m_write_count;
				if (!m_write_count)
				{
					m_write_thread = NULL;
					ReleaseSRWLockExclusive(&m_lock);
				}
			}
		};
	}
}

#endif