// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Assert.hpp
* @author JXMaster
* @date 2018/10/26
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_WINDOWS

#include "../../OS.hpp"
#include <Runtime/Unicode.hpp>
#include <assert.h>

namespace Luna
{
    namespace OS
    {
		void assert_fail(const c8* msg, const c8* file, u32 line)
		{
			c8 buf[1024];
			sprintf_s((char*)buf, 1024, "%s", (const char*)msg);
			usize msg_len = utf8_to_utf16_len(buf);
			usize file_len = utf8_to_utf16_len(file);
#ifdef LUNA_DEBUG
			c16* wbuf = (c16*)alloca(sizeof(c16) * (msg_len + 1));
			c16* wfile = (c16*)alloca(sizeof(c16) * (file_len + 1));
			utf8_to_utf16(wbuf, msg_len, buf);
			utf8_to_utf16(wfile, file_len, file);
			_wassert((wchar_t*)wbuf, (wchar_t*)wfile, line);
#else
			printf("Assertion Failed: %s FILE: %s, LINE: %d", buf, file, line);
			abort();
#endif
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