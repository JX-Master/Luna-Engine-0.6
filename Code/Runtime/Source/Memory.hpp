// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Memory.hpp
* @author JXMaster
* @date 2021/5/10
*/
#pragma once
#include "../Memory.hpp"

namespace Luna
{
#ifdef LUNA_RUNTIME_CHECK_MEMORY_LEAK
	void check_memory_leak();
#endif
}