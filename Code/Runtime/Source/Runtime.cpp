// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Runtime.cpp
* @author JXMaster
* @date 2020/12/10
*/
#pragma once
#include "../PlatformDefines.hpp"
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "../Runtime.hpp"
#include "Name.hpp"
#include "Module.hpp"
#include "OS.hpp"
#include "Memory.hpp"
namespace Luna
{
	void time_init();
	void error_init();
	void error_close();

	LUNA_RUNTIME_API RV init()
	{
		OS::init();
		error_init();
		name_init();
		time_init();
		auto r = module_init();
		if (!r.valid())
		{
			name_close();
			error_close();
			return r;
		}
		return RV();
	}
	LUNA_RUNTIME_API void close()
	{
		module_close();
		name_close();
		error_close();
		OS::close();
#ifdef LUNA_RUNTIME_CHECK_MEMORY_LEAK
		check_memory_leak();
#endif
	}
}