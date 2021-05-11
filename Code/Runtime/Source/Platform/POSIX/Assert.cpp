// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Assert.cpp
* @author JXMaster
* @date 2020/9/22
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#include "../../OS.hpp"
#include <Runtime/Unicode.hpp>
#include <assert.h>

namespace Luna
{
    namespace OS
    {
        void assert_fail(const c8* msg, const c8* file, u32 line)
        {
            printf ("%s:%d: failed assertion `%s'\n", file, line, msg);
            abort();
        }

        void debug_break()
        {
#ifdef LUNA_DEBUG
			__debugbreak();
#endif
        }

    }
}

#endif