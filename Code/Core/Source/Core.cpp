// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Core.cpp
* @author JXMaster
* @date 2019/1/10
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT
#include "../Core.hpp"
#include <Runtime/Module.hpp>
#include "Signal.hpp"
#include "Mutex.hpp"
#include "Semaphore.hpp"
#include "Vfs.hpp"

namespace Luna
{
	void update_loop_init();
	void thread_init();
	void thread_close();
	void dispatch_init();
	void dispatch_deinit();
	void random_init();
	void random_deinit();
	RV core_init()
	{
		thread_init();
		update_loop_init();
		dispatch_init();
		random_init();
		vfs_init();
		return RV();
	}

	void core_close()
	{
		vfs_deinit();
		random_deinit();
		dispatch_deinit();
		thread_close();
	}

    StaticRegisterModule g_core(u8"Core", u8"", core_init, core_close);

    namespace CoreError
    {
		LUNA_CORE_API errcode_t expired()
		{
			static errcode_t e = get_error_code_by_name(u8"CoreError::expired");
			return e;
		}
    }

	LUNA_CORE_API P<ISignal> new_signal(bool manual_reset)
	{
        return newobj<Signal>(manual_reset);
	}
    LUNA_CORE_API P<IMutex> new_mutex()
    {
        return newobj<Mutex>();
    }
	LUNA_CORE_API P<ISemaphore> new_semaphore(i32 initial_count, i32 max_count)
	{
		return newobj<Semaphore>(initial_count, max_count);
	}
}