// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Debug.hpp
* @author JXMaster
* @date 2020/12/26
*/
#pragma once
#include "Base.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	//! Prints a message to the debug console attached to this process.
	LUNA_RUNTIME_API void debug_printf(const c8* fmt, ...);

	//! Prints a message to the debug console attached to this process with `VarList` as arguments.
	LUNA_RUNTIME_API void debug_vprintf(const c8* fmt, VarList args);
}