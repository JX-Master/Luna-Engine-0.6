// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IDispatchQueue.hpp
* @author JXMaster
* @date 2020/3/8
*/
#pragma once
#include <Core/IRunnable.hpp>
namespace luna
{
	//! @interface IDispatchQueue
	//! @threadsafe
	//! IDispatchQueue is the basic component for task-based asynchronous programming.
	//! One dispatch queue holds an first-in-first-out queue in which tasks are fetched 
	//! and executed by multiple worker threads one after another. 
	//! 
	//! Every queue has a number that determines how many tasks in the queue can be executed concurrently,
	//! this number is called concurrency limit. The concurrency limit for the queue must be decided
	//! when the queue is created, and cannot be changed after that. There are two special concurrency value
	//! exist: 0 and 1. Value 0 means no concurrency limit, every task that is enqueued will be executed immediately
	//! if the thread pool is not fully busy. Value 1 means that there is at most 1 task that will be executed 
	//! at any given time for the queue, so the next task in the queue will be executed only after the previous 
	//! task has been finished, which makes the execution order of the tasks in the queue predicable.
	//! 
	//! One task will only be assigned to one worker thread, but different tasks may be assigned
	//! to different threads, even if they are in the same queue and get processed one after another. 
	//! The task can be blocked, which cause the worker thread to be blocked or suspended.
	//! 
	//! The worker threads are reused in one engine-level thread pool. When the engine starts, the pool is empty, and 
	//! worker threads are created and reused when tasks are pushed into the queue. For every platform, the maximum number
	//! of worker threads the pool will have is hard-coded, if this number exceeds, the succeeding added tasks will have to 
	//! wait to be executed after the previous tasks are finished.
	//! 
	//! Dispatch queues are much lighter to create, use and destroy than threads, so prefer using the 
	//! dispatch queue whenever possible.
	struct IDispatchQueue : public IObject
	{
		luiid("{96c2766b-4110-4be2-8987-bd0a4306b1ac}");

		//! Gets the concurrency limit value for this queue.
		//! 0 means no limit.
		virtual uint32 concurrency_limit() = 0;

		//! Enqueues one task to this dispatch queue, waiting to be processed by
		//! one of the worker threads.
		//! The dispatch queue holds one strong reference to the task object until
		//! the task has been finished.
		virtual void dispatch(IRunnable* task) = 0;

		//! Gets the number of tasks being enqueued to this dispatch queue.
		//! The counter number increases by 1 each time one task is enqueued by calling `dispatch`.
		virtual uint64 enqueue_count() = 0;

		//! Gets the number of tasks being dequeued and started executing.
		//! The counter number increases by 1 each time one task is dequeued from the dispatch queue
		//! and starts execution by one worker thread.
		virtual uint64 execution_count() = 0;

		//! Gets the number of tasks being finished executing.
		//! The counter number increases by 1 each time one task is finished executing by the worker thread.
		virtual uint64 completion_count() = 0;

	};
}