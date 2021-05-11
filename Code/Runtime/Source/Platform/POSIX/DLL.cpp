// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file DLL.cpp
* @author JXMaster
* @date 2020/9/22
*/
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX
#include "../../OS.hpp"
#include <dlfcn.h>

namespace Luna
{
    namespace OS
    {
        R<handle_t> load_library(const c8* filename)
        {
            lucheck(filename);
            void* handle = dlopen((const char*)filename, 0);
            if (handle)
            {
                return handle;
            }
            return BasicError::bad_system_call();
        }
        RV free_library(handle_t handle)
        {
            lucheck(handle);
            int r = dlclose(handle);
            if(r != 0)
            {
                //const char* err = dlerror();
                return BasicError::bad_system_call();
            }
            return RV();
        }
        R<void*> get_proc_address(handle_t handle, const c8* symbol_name)
        {
            lucheck(handle && symbol_name);
            void* r = dlsym(handle, symbol_name);
            if(r == nullptr)
            {
                //const char* err = dlerror();
                return BasicError::bad_system_call();
            }
            return r;
        }
    }
}

#endif
