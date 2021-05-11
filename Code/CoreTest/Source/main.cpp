// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file main.cpp
* @author JXMaster
* @date 2020/2/16
*/
#include "TestCommon.hpp"

using namespace Luna;

int main()
{
	init();

	data_test();
	vfs_test();

	close();

	return 0;
}