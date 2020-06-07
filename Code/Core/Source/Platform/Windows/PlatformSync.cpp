// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformSync.cpp
* @author JXMaster
* @date 2019/12/26
 */
#include "../../Platform.hpp"
#ifdef LUNA_PLATFORM_WINDOWS
#include "Signal.hpp"
#include "Mutex.hpp"
#include "Semaphore.hpp"
#include "ReadWriteLock.hpp"
#include "Heap.hpp"
namespace luna
{
	namespace platform
	{
		using namespace win;
		P<ISignal> new_signal(bool manual_reset)
		{
			P<Signal> signal;
			signal.attach(get_global_heap()->new_obj<Signal>(get_global_heap()));
#ifdef LUNA_USE_LIGHT_EVENT
			signal->m_manual_reset = manual_reset;
#else
			signal->m_handle = ::CreateEventW(NULL, manual_reset, FALSE, NULL);
			if (!signal->m_handle)
			{
				lupanic_msg_always("CreateEventW failed.");
			}
#endif
			return signal;
		}
		P<IMutex> new_mutex()
		{
			P<Mutex> mtx;
			mtx.attach(get_global_heap()->new_obj<Mutex>(get_global_heap()));
			/*mtx->m_handle = ::CreateMutexW(NULL, FALSE, NULL);
			if (!mtx->m_handle)
			{
			mtx = nullptr;
			return { mtx, e_bad_system_call };
			}*/
			return mtx;
		}
		P<ISemaphore> new_semaphore(int32 initial_count, int32 max_count)
		{
			P<Semaphore> sema;
			sema.attach(get_global_heap()->new_obj<Semaphore>(get_global_heap()));
			sema->m_handle = ::CreateSemaphoreW(NULL, initial_count, max_count, NULL);
			if (!sema->m_handle)
			{
				lupanic_msg_always("CreateSemaphoreW failed.");
			}
			return sema;
		}
		P<IReadWriteLock> new_read_write_lock()
		{
			P<IReadWriteLock> loc;
			loc.attach(get_global_heap()->new_obj<ReadWriteLock>(get_global_heap()));
			return loc;
		}
	}
}

#endif