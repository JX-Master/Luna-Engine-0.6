// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ThreadPool.cpp
* @author JXMaster
* @date 2020/3/8
*/
#include "ThreadPool.hpp"

namespace luna
{
	int WorkerThreadContext::run()
	{
		while (true)
		{
			m_wake->wait();
			if (m_exiting)
			{
				break;
			}
			// loop until the queue is empty.
			while (true)
			{
				// Lock the queue to fetch the next tasks.
				MutexGuard queue_guard(m_working_queue->m_mtx);
				if (m_working_queue->m_tasks.empty())
				{
					// Release the prior queue.
					atom_dec_u32(&(m_working_queue->m_num_attached_threads));
					queue_guard.unlock();
					m_working_queue = nullptr;

					// Checks if there is any queue in the waiting list.
					MutexGuard pool_mtx(dispatch::m_mtx);
					while (!dispatch::m_waiting_queue.get().empty())
					{
						m_working_queue = dispatch::m_waiting_queue.get().front().lock();
						dispatch::m_waiting_queue.get().pop_front();
						if (m_working_queue)
						{
							// Locks the new queue.
							queue_guard.unlock();
							queue_guard.lock(m_working_queue->m_mtx);
							break;
						}
					}

					if (!m_working_queue)
					{
						// No queue in the waiting list. Go to sleep.
						// This should never fail, since no allocation is occurred here.
						dispatch::m_idle_threads.get().push_back((WorkerThreadContext*)this);
						break;
					}
				}
				// Fetches the next task and releases the lock.
				P<IRunnable> task = m_working_queue->m_tasks.front();
				m_working_queue->m_tasks.pop_front();
				queue_guard.unlock();

				atom_inc_u64(&m_working_queue->m_execution_count);

				// Do the task.
				task->run();

				atom_inc_u64(&m_working_queue->m_completion_count);
			}
		}
		return 0;
	}

	namespace dispatch
	{
		P<IMutex> m_mtx;
		Unconstructed<Vector<P<IThread>>> m_threads;
		Unconstructed < Vector<P<WorkerThreadContext>>> m_ctxs;
		Unconstructed < RingDeque<P<WorkerThreadContext>>> m_idle_threads;
		Unconstructed < RingDeque<WP<DispatchQueue>>> m_waiting_queue;
		RP<WorkerThreadContext> consume_worker_thread()
		{
			MutexGuard guard(m_mtx);
			P<WorkerThreadContext> ctx;
			lutry
			{
				if (m_idle_threads.get().empty())
				{
					if (m_threads.get().size() < max_thread_count_v)
					{
						// If no thread is idle, creates a new thread.
						ctx = box_ptr(new_obj<WorkerThreadContext>(get_global_heap()));
						if (!ctx)
						{
							luthrow(e_bad_memory_alloc);
						}
						lulet(th, new_thread(ctx));
						m_threads.get().push_back(th);
						m_ctxs.get().push_back(ctx);
					}
					else
					{
						// Limit reached.
						luthrow(e_overflow);
					}
				}
				else
				{
					// The threads are waked by dispatch queue.
					ctx = m_idle_threads.get().front();
					m_idle_threads.get().pop_front();
				}
			}
			lucatchret;
			return ctx;
		}

		P<IDispatchQueue> new_dispatch_queue(uint32 concurrency_limit)
		{
			P<DispatchQueue> q;
			q = box_ptr(new_obj<DispatchQueue>(get_global_heap()));
			q->m_concurrency_limit = concurrency_limit;
			return q;
		}
	}
}