// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Runtime.hpp
* @author JXMaster
* @date 2020/7/22
* @brief The root interface of Engine Runtime. All APIs listed here except `Luna::init` is available only after
* `Luna::init` and before `Luna::close`.
*/
#pragma once
#include "Result.hpp"
#include "Name.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	enum class TypeKind : u32
	{
		primitive_type = 1,
		array_type = 2,
		tuple_type = 3,
		reference_type = 4,
		enumeration_type = 5,
		structure_type = 6,
		class_type = 7,
		interface_type = 8,
		typeinfo_type = 9,
		closure_type = 10,
		pointer_type = 11,
		aliasing_type = 12,
		error_type = 13,
	};

	//! The types that can be defined by the user. 
	enum class UserDefinedTypeKind : u32
	{
		enumeration_type = 1,
		structure_type = 2,
		class_type = 3,
		interface_type = 4,
		aliasing_type = 5,
		error_type = 6,
	};

	enum class PrimitiveTypeKind : u32
	{
		u8_type = 1,
		i8_type = 2,
		u16_type = 3,
		i16_type = 4,
		u32_type = 5,
		i32_type = 6,
		u64_type = 7,
		i64_type = 8,
		usize_type = 9,
		isize_type = 10,
		f32_type = 11,
		f64_type = 12,
		boolean_type = 13,
		void_type = 14,
		c8_type = 15,
		c16_type = 16,
		c32_type = 17,
	};

	struct Assembly;
	using typeinfo_t = handle_t;

	//! Initializes and starts Luna Runtime.
	//! @return 
	//! * `s_ok` on success.
	//! * Platform-specific error code otherwise.
	LUNA_RUNTIME_API RV init();

	//! Closes Luna Runtime.
	LUNA_RUNTIME_API void close();

	//! Creates one new empty assembly object.
	//! This is used for creating one assembly from C++, if you want to load one existing assembly, use
	//! `load_assembly` instead.
	//! @return Returns the created assembly object.
	LUNA_RUNTIME_API Assembly* new_assembly();

	//! Destroys one assembly object, this assembly can either be the assembly created from `new_assembly`, 
	//! or the assembly loaded from `load_assembly`.
	//! @param[in] assembly The assembly object to destroy, 
	LUNA_RUNTIME_API void delete_assembly(Assembly* assembly);

	//! Loads the assembly to Luna Runtime.
	//! @param[in] assembly The assembly to load to the runtime.
	//! @remark The assembly will be validated when the runtime loads it.
	LUNA_RUNTIME_API RV load_assembly_to_runtime(Assembly* assembly);
}