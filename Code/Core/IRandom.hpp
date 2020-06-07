// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IRandom.hpp
* @author JXMaster
* @date 2019/3/24
* @brief IRandomGenerator represents a random generate context.
*/
#pragma once
#include <Base/IObject.hpp>
namespace luna
{
	//! @interface IRandom
	//! Represents a Pseudo-Random number generator.
	struct IRandom : public IObject
	{
		luiid("{c5a542f4-36be-45e7-8dba-9cb74adff098}");

		virtual void set_seed(uint32 seed) = 0;

		virtual uint32 gen_u32() = 0;
		virtual int32 gen_i32() = 0;
		virtual uint64 gen_u64() = 0;
		virtual int64 gen_i64() = 0;
		virtual float32 gen_f32(float32 range_begin, float32 range_end) = 0;
		virtual float64 gen_f64(float64 range_begin, float64 range_end) = 0;
		virtual Guid gen_guid() = 0;
	};
}