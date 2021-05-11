// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestCommon.hpp
* @author JXMaster
* @date 2020/2/16
*/
#pragma once
#include <Core/Core.hpp>
#include <Runtime/Runtime.hpp>

namespace Luna
{
	void vfs_test();
	void data_test();
}

#define lutest luassert_always