// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file main.cpp
* @author JXMaster
* @date 2020/2/16
*/
#include "TestCommon.hpp"
#include <Runtime/Memory.hpp>

using namespace Luna;

int main()
{
	auto r = init();

	usize mem = get_allocated_memory();

	vector_test();

	//! Ensure there is not any memory leak in container.
	lutest(mem == get_allocated_memory());

	hash_test();

	lutest(mem == get_allocated_memory());

	ring_deque_test();

	lutest(mem == get_allocated_memory());

	string_test();

	lutest(mem == get_allocated_memory());

	name_test();

	path_test();
	//vfs_test();
	//data_test();

	time_test();

	lexical_test();

	close();

	return 0;
}