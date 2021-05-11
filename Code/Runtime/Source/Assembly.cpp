// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Assembly.cpp
* @author JXMaster
* @date 2021/4/19
*/
#include "../PlatformDefines.hpp"
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "Assembly.hpp"
#include "../Memory.hpp"

namespace Luna
{
	LUNA_RUNTIME_API Assembly* new_assembly()
	{
		return memnew<Assembly>();
	}

	LUNA_RUNTIME_API void delete_assembly(Assembly* assembly)
	{
		memdelete(assembly);
	}

	LUNA_RUNTIME_API RV load_assembly_to_runtime(Assembly* assembly)
	{
		return BasicError::not_supported();
	}
}