// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Platform.cpp
* @author JXMaster
* @date 2020/1/10
*/
#pragma once
#include "../../Platform.hpp"
#ifdef LUNA_PLATFORM_WINDOWS

namespace luna
{
	namespace platform
	{
		void platform_heap_init();
		void platform_file_init();
		void platform_thread_init();
		void platform_time_init();
		void platform_random_init();
		void platform_thread_close();

		RV init()
		{
			platform_heap_init();
			platform_file_init();
			platform_thread_init();
			platform_time_init();
			platform_random_init();
			return s_ok;
		}

		void close()
		{
			platform_thread_close();
		}
	}
}

#endif