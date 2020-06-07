// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformSharedLibrary.cpp
* @author JXMaster
* @date 2019/12/26
*/
#include "../../Platform.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
#include <Base/Unicode.hpp>
#include <malloc.h>
namespace luna
{
	namespace platform
	{
		R<handle_t> load_library(const char * filename)
		{
			size_t path_len = utf8_to_utf16_len(filename) + 1;
			wchar_t* path = (wchar_t*)alloca(sizeof(char) * path_len);
			utf8_to_utf16((char16_t*)path, path_len, filename);
			HMODULE h = ::LoadLibraryW(path);
			if (h)
			{
				return { h, s_ok };
			}
			return e_bad_system_call;
		}
		RV free_library(handle_t handle)
		{
			BOOL r = ::FreeLibrary((HMODULE)handle);
			return r ? s_ok : e_failure;
		}
		void* get_proc_address(handle_t handle, const char* symbol_name)
		{
			luassert_usr(handle && symbol_name);
			FARPROC p = ::GetProcAddress((HMODULE)handle, symbol_name);
			return p;
		}
	}
}

#endif