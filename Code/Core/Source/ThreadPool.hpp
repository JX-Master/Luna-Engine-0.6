// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ThreadPool.hpp
* @author JXMaster
* @date 2020/3/8
*/
#pragma once
#include "DispatchQueue.hpp"
#include <Runtime/Vector.hpp>

namespace Luna
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

		WorkerThreadContext() :
			m_exiting(false)
		{
			m_wake = new_signal(false);
		}

		virtual void run() override;
	};

	extern P<IMutex> g_dispatch_threads_mtx;
	extern Unconstructed<Vector<P<IThread>>> g_dispatch_threads;
	extern Unconstructed < Vector<P<WorkerThreadContext>>> g_dispatch_ctxs;
	extern Unconstructed < RingDeque<P<WorkerThreadContext>>> g_dispatch_idle_threads;

	extern P<IMutex> g_dispatch_waiting_queue_mtx;
	extern Unconstructed < RingDeque<WP<DispatchQueue>>> g_dispatch_waiting_queue;

	//! Consumes one idle worker thread, or creates a new one if all threads are busy and the physical limit is not reached.
	RP<WorkerThreadContext> dispatch_consume_worker_thread();
}