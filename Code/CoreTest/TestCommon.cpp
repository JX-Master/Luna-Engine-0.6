// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestCommon.cpp
* @author JXMaster
* @date 2020/2/16
*/
#include "TestCommon.hpp"

namespace luna
{
	int64 TestObject::g_count = 0;
	int64 TestObject::g_ctor_count = 0;
	int64 TestObject::g_dtor_count = 0;
	int64 TestObject::g_default_ctor_count = 0;
	int64 TestObject::g_arg_ctor_count = 0;
	int64 TestObject::g_copy_ctor_count = 0;
	int64 TestObject::g_move_ctor_count = 0;
	int64 TestObject::g_copy_assign_count = 0;
	int64 TestObject::g_move_assign_count = 0;
	int32 TestObject::g_magic_error_count = 0;
}