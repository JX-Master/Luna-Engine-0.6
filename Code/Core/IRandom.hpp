// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IRandom.hpp
* @author JXMaster
* @date 2019/3/24
* @brief IRandomGenerator represents a random generate context.
*/
#pragma once
#include <Core/IObject.hpp>
namespace Luna
{
	//! @interface IRandom
	//! Represents a Pseudo-Random number generator.
	struct IRandom : public IObject
	{
		luiid("{c5a542f4-36be-45e7-8dba-9cb74adff098}");

		virtual void set_seed(u32 seed) = 0;

		virtual u32 gen_u32() = 0;
		virtual i32 gen_i32() = 0;
		virtual u64 gen_u64() = 0;
		virtual i64 gen_i64() = 0;
		virtual f32 gen_f32(f32 range_begin, f32 range_end) = 0;
		virtual f64 gen_f64(f64 range_begin, f64 range_end) = 0;
		virtual Guid gen_guid() = 0;
	};
}