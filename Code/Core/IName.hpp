// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IName.hpp
* @author JXMaster
* @date 2019/8/6
*/
#pragma once
#include "IString.hpp"

namespace luna
{
	struct ISystem;
	//! @interface IName
	//! @threadsafe
	//! IName represent a reference-counted, immutable string used to represent a name. 
	//! All name strings are immutable and every unique name string will have at most one `IName` 
	//! instance which is shared between all objects reference the same name, 
	//! so the `IName` pointer can be used to compare when you want to compare the name string.
	//! 
	//! The name objects are managed by `ISystem` interface.
	struct IName : public IString
	{
		luiid("{47daa3cf-a5a2-4521-8bac-7b5e09e8ab1a}");

	};

	template <> struct Hash<P<IName>>
	{
		size_t operator()(const P<IName>& p) const
		{
			return size_t(uintptr_t(p.get()));
		}
	};

	template <> struct Hash<IName*>
	{
		size_t operator()(const IName* p) const
		{
			return size_t(uintptr_t(p));
		}
	};
}