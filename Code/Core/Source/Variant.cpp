// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Varient.cpp
* @author JXMaster
* @date 2021/5/10
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT
#include "../Variant.hpp"

namespace Luna
{
	LUNA_CORE_API Variant& get_npos_variant()
	{
		static Variant var(EVariantType::null);
		return var;
	}
}