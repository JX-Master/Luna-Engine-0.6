// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ThreadPool.cpp
* @author JXMaster
* @date 2020/3/8
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT
#include "ThreadPool.hpp"
#include <Runtime/Platform.hpp>

namespace Luna
{
	void WorkerThreadContext::run()
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
					queue_guard.unlock();
					atom_dec_u32(&(m_working_queue->m_num_attached_threads));
					m_working_queue = nullptr;

					// Checks if there is any queue in the waiting list.
					MutexGuard pool_mtx(g_dispatch_waiting_queue_mtx);
					while (!g_dispatch_waiting_queue.get().empty())
					{
						m_working_queue = g_dispatch_waiting_queue.get().front().lock();
						g_dispatch_waiting_queue.get().pop_front();
						if (m_working_queue)
						{
							// Locks the new queue.
							queue_guard.lock(m_working_queue->m_mtx);
							if (m_working_queue->m_tasks.empty())
							{
								// This queue is not counted.
								queue_guard.unlock();
								m_working_queue = nullptr;
								continue;
							}
							else
							{
								break;
							}
						}
					}
					pool_mtx.unlock();

					// If this is true, then the `if` statement in the last while statement
					// is not executed, so we do not need to lock the pool again.
					if (!m_working_queue)
					{
						MutexGuard threads_mtx(g_dispatch_threads_mtx);
						// No queue in the waiting list. Go to sleep.
						// This should never fail, since no allocation is occurred here.
						g_dispatch_idle_threads.get().push_back((WorkerThreadContext*)this);
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
	}

	P<IMutex> g_dispatch_threads_mtx;
	Unconstructed<Vector<P<IThread>>> g_dispatch_threads;
	Unconstructed < Vector<P<WorkerThreadContext>>> g_dispatch_ctxs;
	Unconstructed < RingDeque<P<WorkerThreadContext>>> g_dispatch_idle_threads;
	
	P<IMutex> g_dispatch_waiting_queue_mtx;
	Unconstructed < RingDeque<WP<DispatchQueue>>> g_dispatch_waiting_queue;

	u32 g_dispatch_max_thread_count = 0;

	void dispatch_init()
	{
		g_dispatch_max_thread_count = Platform::get_num_processors();
		g_dispatch_threads.construct();
		g_dispatch_ctxs.construct();
		g_dispatch_idle_threads.construct();
		g_dispatch_waiting_queue.construct();
		g_dispatch_threads_mtx = new_mutex();
		g_dispatch_waiting_queue_mtx = new_mutex();
	}

	void dispatch_deinit()
	{
		MutexGuard g(g_dispatch_threads_mtx);
		for (auto& i : g_dispatch_ctxs.get())
		{
			i->m_exiting = true;
			i->m_wake->trigger();
		}
		g.unlock();
		g_dispatch_idle_threads.get().clear();
		g_dispatch_waiting_queue_mtx = nullptr;
		g_dispatch_threads_mtx = nullptr;
		g_dispatch_threads.destruct();
		g_dispatch_ctxs.destruct();
		g_dispatch_idle_threads.destruct();
		g_dispatch_waiting_queue.destruct();
	}

	RP<WorkerThreadContext> dispatch_consume_worker_thread()
	{
		MutexGuard threads_mtx(g_dispatch_threads_mtx);
		P<WorkerThreadContext> ctx;
		lutry
		{
			if (g_dispatch_idle_threads.get().empty())
			{
				if (g_dispatch_threads.get().size() < g_dispatch_max_thread_count)
				{
					// If no thread is idle, creates a new thread.
					ctx = newobj<WorkerThreadContext>();
					if (!ctx)
					{
						luthrow(BasicError::bad_memory_alloc());
					}
					lulet(th, new_thread(ctx));
					g_dispatch_threads.get().push_back(th);
					g_dispatch_ctxs.get().push_back(ctx);
				}
				else
				{
					// Limit reached.
					luthrow(BasicError::overflow());
				}
			}
			else
			{
				// The threads are waked by dispatch queue.
				ctx = g_dispatch_idle_threads.get().front();
				g_dispatch_idle_threads.get().pop_front();
			}
		}
		lucatchret;
		return ctx;
	}

	LUNA_CORE_API P<IDispatchQueue> new_dispatch_queue(u32 concurrency_limit)
	{
		P<DispatchQueue> q;
		q = newobj<DispatchQueue>();
		q->m_concurrency_limit = concurrency_limit;
		return q;
	}
}