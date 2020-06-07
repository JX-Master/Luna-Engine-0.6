// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IReadWriteLock.hpp
* @author JXMaster
* @date 2019/12/9
*/
#pragma once
#include <Base/IObject.hpp>

namespace luna
{
	//! @interface IReadWriteLock
	//! @threadsafe
	//! The read write lock enables multiple threads to access one resource in
	//! two modes:
	//! * Read mode, in which multiple threads can access the same resource at the
	//! same time, but no thread can write to the resource or change the data other
	//! threads are accessing.
	//! * Write mode, in which only one thread can access the same resource at the
	//! same time, so the thread is allowed to write to the resource.
	//! 
	//! The lock can be locked either in read mode or write mode, if the lock is locked
	//! in read mode, additional calls to `read_lock` is allowed, but calls to `write_lock`
	//! blocks until all read locks have been released; if the lock is locked in write
	//! mode, all other `read_lock` and `write_lock` will be blocked until the write lock has
	//! been released.
	//! 
	//! You cannot promote a read lock to a write lock since some other threads may also holds the 
	//! read lock, you need to release the read lock first and acquire the write lock after, else 
	//! the deadlock will happen. The read write lock supports recursive locking from one thread in
	//! read or write mode, but you need to call `xxx_unlock` the same times as you call `xxx_lock`.
	//! The read write lock cannot be write locked from one thread and be released from another thread.
	struct IReadWriteLock : public IObject
	{
		luiid("{46df812a-9ab6-48d4-8daf-b6457484cd64}");

		virtual void read_lock() = 0;
		virtual RV try_read_lock() = 0;
		virtual void read_unlock() = 0;
		virtual void write_lock() = 0;
		virtual RV try_write_lock() = 0;
		virtual void write_unlock() = 0;
	};

	//! RAII wrapper for read lock.
	class ReadGuard
	{
		IReadWriteLock* m_lock;
	public:
		ReadGuard() = delete;
		ReadGuard(const P<IReadWriteLock>& lock)
		{
			m_lock = lock.get();
			m_lock->read_lock();
		}
		ReadGuard(IReadWriteLock* lock)
		{
			m_lock = lock;
			m_lock->read_lock();
		}
		void unlock()
		{
			if (m_lock)
			{
				m_lock->read_unlock();
			}
			m_lock = nullptr;
		}
		~ReadGuard()
		{
			unlock();
		}
	};

	//! RAII wrapper for write lock.
	class WriteGuard
	{
		IReadWriteLock* m_lock;
	public:
		WriteGuard(const P<IReadWriteLock>& lock)
		{
			m_lock = lock.get();
			m_lock->write_lock();
		}
		WriteGuard(IReadWriteLock* lock)
		{
			m_lock = lock;
			m_lock->write_lock();
		}
		void unlock()
		{
			if (m_lock)
			{
				m_lock->write_unlock();
			}
			m_lock = nullptr;
		}
		~WriteGuard()
		{
			unlock();
		}
	};
}