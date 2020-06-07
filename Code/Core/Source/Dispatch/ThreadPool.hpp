// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ThreadPool.hpp
* @author JXMaster
* @date 2020/3/8
*/
#pragma once
#include "DispatchQueue.hpp"
#include <Base/Vector.hpp>
#include <Base/RingDeque.hpp>

namespace luna
{
	class WorkerThreadContext : public IRunnable
	{
	public:
		lucid("{5fb64443-d440-4370-9a35-ab5b98d82b27}");
		luiimpl(WorkerThreadContext, IRunnable, IObject);

		// The signal to wake up this thread.
		// Triggered after this thread has been removed from idle thread
		// list and has one queue attached.
		P<ISignal> m_wake;

		// One worker thread is assigned to one queue at a time,
		// and only detaches from the queue if the queue is empty.
		// If the queue is a serialized queue, only one thread is
		// assigned for most, otherwise, multiple threads can be assigned.
		P<DispatchQueue> m_working_queue;

		// Marked when the engine is exiting.
		bool m_exiting;

		WorkerThreadContext(IAllocator* alloc) :
			luibind(alloc),
			m_exiting(false)
		{
			m_wake = new_signal(false);
		}

		virtual int run() override;
	};

	namespace dispatch
	{
		extern P<IMutex> m_mtx;
		extern Unconstructed<Vector<P<IThread>>> m_threads;
		extern Unconstructed < Vector<P<WorkerThreadContext>>> m_ctxs;
		extern Unconstructed < RingDeque<P<WorkerThreadContext>>> m_idle_threads;
		extern Unconstructed < RingDeque<WP<DispatchQueue>>> m_waiting_queue;

		static constexpr size_t max_thread_count_v = 64;

		inline void init(IAllocator* alloc)
		{
			m_threads.construct(alloc);
			m_ctxs.construct(alloc);
			m_idle_threads.construct(alloc);
			m_waiting_queue.construct(alloc);
			m_mtx = new_mutex();
		}

		inline void deinit()
		{
			MutexGuard g(m_mtx);
			for (auto& i : m_ctxs.get())
			{
				i->m_exiting = true;
				i->m_wake->trigger();
			}
			g.unlock();
			m_idle_threads.get().clear();
			m_mtx = nullptr;
			m_threads.destruct();
			m_ctxs.destruct();
			m_idle_threads.destruct();
			m_waiting_queue.destruct();
		}

		//! Consumes one idle worker thread, or creates a new one if all threads are busy and the physical limit is not reached.
		RP<WorkerThreadContext> consume_worker_thread();

		P<IDispatchQueue> new_dispatch_queue(uint32 concurrency_limit);
	}
}