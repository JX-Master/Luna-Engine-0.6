// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file main.cpp
* @author JXMaster
* @date 2020/2/16
*/
#include "TestCommon.hpp"

using namespace luna;

int main()
{
	init();

	size_t mem = get_global_heap()->allocated();

	vector_test();

	//! Ensure there is not any memory leak in container.
	lutest(mem == get_global_heap()->allocated());

	hash_test();

	lutest(mem == get_global_heap()->allocated());

	ring_deque_test();

	lutest(mem == get_global_heap()->allocated());

	strlib_test();
	vfs_test();
	data_test();

	shutdown();

	return 0;
}