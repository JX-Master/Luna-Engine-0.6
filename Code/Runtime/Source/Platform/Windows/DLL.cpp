// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file DLL.cpp
* @author JXMaster
* @date 2020/8/20
*/
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_WINDOWS
#include "MiniWin.hpp"
#include <Runtime/Unicode.hpp>
#include "../../OS.hpp"
#include <Runtime/Result.hpp>

namespace Luna
{
	namespace OS
	{
		R<handle_t> load_library(const c8* filename)
		{
			size_t path_len = utf8_to_utf16_len(filename) + 1;
			wchar_t* path = (wchar_t*)alloca(sizeof(char) * path_len);
			utf8_to_utf16((char16_t*)path, path_len, filename);
			HMODULE h = ::LoadLibraryW(path);
			if (h)
			{
				return h;
			}
			return BasicError::bad_system_call();
		}
		RV free_library(handle_t handle)
		{
			BOOL r = ::FreeLibrary((HMODULE)handle);
			return r ? RV() : BasicError::bad_system_call();
		}
		R<void*> get_proc_address(handle_t handle, const c8* symbol_name)
		{
			lucheck(handle && symbol_name);
			FARPROC p = ::GetProcAddress((HMODULE)handle, symbol_name);
			if (p == NULL)
			{
				return BasicError::bad_system_call();
			}
			return p;
		}
	}
	
}

#endif