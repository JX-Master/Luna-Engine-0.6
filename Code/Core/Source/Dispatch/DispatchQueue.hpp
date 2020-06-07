// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file DispatchQueue.hpp
* @author JXMaster
* @date 2020/3/8
*/
#pragma once
#include "../CoreHeader.hpp"
#include <Base/RingDeque.hpp>
#include <Base/Interface.hpp>

namespace luna
{
	class DispatchQueue : public IDispatchQueue
	{
	public:
		lucid("{e957d23f-dd80-4aa1-9646-2e62923ac896}");
		luiimpl(DispatchQueue, IDispatchQueue, IObject);

		volatile uint64 m_enqueue_count;
		volatile uint64 m_execution_count;
		volatile uint64 m_completion_count;

		//! The mutex to operate the task queue.
		P<IMutex> m_mtx;
		RingDeque<P<IRunnable>> m_tasks;

		// Number of threads that are working on this queue, 
		// for a serialized queue, this is 1 for most.
		uint32 m_num_attached_threads;
		//! The concurrency limit for this queue. Compare this
		//! against `m_num_attached_threads` to check if the 
		//! concurrency limit is reached.
		uint32 m_concurrency_limit;

		DispatchQueue(IAllocator* alloc) :
			luibind(alloc),
			m_tasks(alloc),
			m_enqueue_count(0),
			m_execution_count(0),
			m_completion_count(0),
			m_num_attached_threads(0) 
		{
			m_mtx = new_mutex();
		}

		virtual uint32 concurrency_limit() override
		{
			return m_concurrency_limit;
		}

		virtual void dispatch(IRunnable* task) override;

		virtual uint64 enqueue_count() override
		{
			return m_enqueue_count;
		}
		virtual uint64 execution_count() override
		{
			return m_execution_count;
		}
		virtual uint64 completion_count() override
		{
			return m_completion_count;
		}
	};
}