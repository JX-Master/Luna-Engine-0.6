// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Thread.cpp
* @author JXMaster
* @date 2020/12/10
*/
#include "../PlatformDefines.hpp"
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "OS.hpp"
#include "../Thread.hpp"

namespace Luna
{
	LUNA_RUNTIME_API void sleep(u32 time_milliseconds)
	{
		OS::sleep(time_milliseconds);
	}
	LUNA_RUNTIME_API void fast_sleep(u32 time_microseconds)
	{
		OS::fast_sleep(time_microseconds);
	}
	LUNA_RUNTIME_API void yield_current_thread()
	{
		OS::yield_current_thread();
	}
	LUNA_RUNTIME_API u32 get_current_thread_id()
	{
		return OS::get_current_thread_id();
	}
	LUNA_RUNTIME_API R<handle_t> tls_alloc(tls_destructor* destructor)
	{
		return OS::tls_alloc(destructor);
	}
	LUNA_RUNTIME_API void tls_free(handle_t handle)
	{
		OS::tls_free(handle);
	}
	LUNA_RUNTIME_API RV tls_set(handle_t handle, void* ptr)
	{
		return OS::tls_set(handle, ptr);
	}
	LUNA_RUNTIME_API void* tls_get(handle_t handle)
	{
		return OS::tls_get(handle);
	}
}