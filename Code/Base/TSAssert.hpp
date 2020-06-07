// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TSAssert.hpp
* @author JXMaster
* @date 2019/1/15
*/
#pragma once
#include "Assert.hpp"
#include "Atomic.hpp"

namespace luna
{
	struct IThread;
	inline IThread* get_current_thread();	// This is implemented by ICore.

	struct TSLock
	{
		IThread* m_owning_thread;

		TSLock() :
			m_owning_thread(nullptr) {}
	};

	struct TSGuard
	{
		IThread* m_last_thread;
		TSLock& m_lock;

		TSGuard(TSLock& lock) :
			m_lock(lock)
		{
			IThread* cur = get_current_thread();
			m_last_thread = (IThread*)atom_exchange_pointer((void* volatile*)(&(lock.m_owning_thread)), cur);
			luassert_msg_always(!m_last_thread || (m_last_thread == cur), "Data race detected!");
		}

		~TSGuard()
		{
			atom_exchange_pointer((void* volatile*)(&(m_lock.m_owning_thread)), m_last_thread);
		}
	};
}

#if defined(LUNA_PROFILE) && !defined(LUNA_DISABLE_THREAD_SAFE_ASSERT)

#define lutsassert_lock() luna::TSLock m_tsassert_lock;

#define lutsassert() luna::TSGuard _tsguard(m_tsassert_lock);

#else

#define lutsassert_lock() 

#define lutsassert() 

#endif