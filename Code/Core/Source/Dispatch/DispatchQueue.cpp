// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file DispatchQueue.cpp
* @author JXMaster
* @date 2020/3/8
*/
#include "DispatchQueue.hpp"
#include "ThreadPool.hpp"

namespace luna
{
	void DispatchQueue::dispatch(IRunnable* task)
	{
		luassert_usr(task);
		MutexGuard g(m_mtx);
		m_tasks.push_back(task);
		atom_inc_u64(&m_enqueue_count);
		//! Try to awake a new thread to process this task if concurrency limit is not reached.
		if (!m_concurrency_limit || (m_num_attached_threads < m_concurrency_limit))
		{
			MutexGuard pg;
			pg.lock(dispatch::m_mtx);
			auto r_worker_thread = dispatch::consume_worker_thread();
			if (failed(r_worker_thread))
			{
				// For very rare case, if all worker threads are working now, 
				// we put this queue into the waiting list.
				dispatch::m_waiting_queue.get().push_back(WP<DispatchQueue>((DispatchQueue*)this));

				// This is a promise, because every queue in the waiting list will get one thread
				// sooner or later. One queue can be added to the waiting list multiple times.
				atom_inc_u32(&m_num_attached_threads);
			}
			else
			{
				// Attaches the thread and wakes up the thread.
				atom_inc_u32(&m_num_attached_threads);
				r_worker_thread.get()->m_working_queue = (DispatchQueue*)this;
				r_worker_thread.get()->m_wake->trigger();
			}
		}
	}
}


