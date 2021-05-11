// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file TypeBuilder.hpp
* @author JXMaster
* @date 2021/4/19
*/
#pragma once
#include "../TypeBuilder.hpp"
#include "Assembly.hpp"

namespace Luna
{
	struct TypeBuilder
	{
		Name name;
		AssemblyType build_type;

		TypeBuilder(const Name& name, UserDefinedTypeKind kind) :
			name(name),
			build_type(kind) {}
	};
}