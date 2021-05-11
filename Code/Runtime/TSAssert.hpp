// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file TSAssert.hpp
* @author JXMaster
* @date 2019/1/15
*/
#pragma once
#include "Assert.hpp"
#include "Atomic.hpp"
#include "Thread.hpp"
namespace Luna
{
	struct TSLock
	{
		u32 m_owning_thread;

		TSLock() :
			m_owning_thread(u32_max) {}
	};

	struct TSGuard
	{
		u32 m_last_thread;
		TSLock& m_lock;

		TSGuard(TSLock& lock) :
			m_lock(lock)
		{
			u32 cur = get_current_thread_id();
			m_last_thread = atom_exchange_u32((u32 volatile*)(&(lock.m_owning_thread)), cur);
			luassert_msg_always((m_last_thread == u32_max) || (m_last_thread == cur), "Data race detected!");
		}

		~TSGuard()
		{
			atom_exchange_u32((u32 volatile*)(&(m_lock.m_owning_thread)), m_last_thread);
		}
	};
}

#ifdef LUNA_ENABLE_THREAD_SAFE_ASSERTION

#define lutsassert_lock() mutable Luna::TSLock m_tsassert_lock;

#define lutsassert() Luna::TSGuard _tsguard(m_tsassert_lock);

#else

#define lutsassert_lock() 

#define lutsassert() 

#endif