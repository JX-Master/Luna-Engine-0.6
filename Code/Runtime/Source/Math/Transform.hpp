// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Transform.hpp
* @author JXMaster
* @date 2019/1/5
 */
#pragma once
#include "Quaternion.hpp"

namespace Luna
{
	//! Represents the transform information for one object in engine.
	struct Tranform3D
	{
		Float3 position;
		Quaternion rotation;
		Float3 scale;

		Tranform3D() = default;
		Tranform3D(const Tranform3D&) = default;
		Tranform3D& operator=(const Tranform3D&) = default;
		Tranform3D(Tranform3D&&) = default;
		Tranform3D& operator=(Tranform3D&&) = default;

		constexpr Tranform3D(const Float3& _position, const Quaternion& _rotation, const Float3& _scale) :
			position(_position),
			rotation(_rotation),
			scale(_scale) {}

		static constexpr Tranform3D identity()
		{
			return Tranform3D(Float3::zero(), Quaternion::identity(), Float3::one());
		}
	};
}