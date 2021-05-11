// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ISerializable.hpp
* @author JXMaster
* @date 2019/12/30
*/
#pragma once
#include "Variant.hpp"

namespace Luna
{
	//! @interface ISerializable
	//! Represents a object that can be serialized into a variant object.
	struct ISerializable : public IObject
	{
		luiid("{3e9c2eb4-15b1-4541-b23d-ea70426a44ef}");

		//! Serializes the data in the object into one variant object.
		virtual R<Variant> serialize() = 0;

		//! Deserializes the data from the data object.
		virtual RV deserialize(const Variant& obj) = 0;
	};
}