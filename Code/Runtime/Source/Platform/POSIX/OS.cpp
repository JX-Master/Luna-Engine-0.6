// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file OS.cpp
* @author JXMaster
* @date 2021/4/25
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#include "../../OS.hpp"
#include <sys/types.h>
#include <sys/sysctl.h>

namespace Luna
{
    namespace OS
    {
		void time_init();
		void file_init();

		void init()
		{
			time_init();
			file_init();
		}

		void close() {}

		u32 get_num_processors()
		{
#ifdef LUNA_PLATFORM_MACOS
			usize size;
			int name[2];
			size = 4;
			name[0] = CTL_HW;
			name[1] = HW_NCPU;
			int processor_count;
			if (sysctl(name, 2, &processor_count, &size, nullptr, 0) != 0)
			{
				processor_count = 1;
			}
			return (u32)processor_count;
#elif LUNA_PLATFORM_LINUX
			int processor_count = max(sysconf(_SC_NPROCESSORS_ONLN), 1);
			return (u32)processor_count;
#else
#error "get_num_processors not implemented."
#endif
		}
    }
}

#endif