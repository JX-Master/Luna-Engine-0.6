// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ICloneable.hpp
* @author JXMaster
* @date 2018/12/8
*/
#pragma once
#include "IObject.hpp"
namespace luna
{
	//! @interface ICloneable
	//! Represents one object that supports creating a copy of itself.
	//! Objects and interfaces that conform to `ICloneable` can be copied, 
	//! the copy behavior is defined by the object itself.
	//! @remark The cloned interface (the interface returned by `clone`) must 
	//! satisfy the following traits:
	//! 1. Querying `IObject` interface from the cloned interface returns a different
	//! interface compared to the `IObject` interface queried from `this`.
	//! 2. If `this` conforms interface `A`, the cloned interface also conforms interface
	//! `A`.
	struct ICloneable : public IObject
	{
		luiid("{9edce23d-e215-4df1-a59f-b7cc03248971}");

		//! Creates a copy of this object. This function always succeeds.
		virtual P<IObject> clone() = 0;
	};
}