// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file TypeBuilder.hpp
* @author JXMaster
* @date 2021/4/23
*/
#pragma once
#include "Runtime.hpp"

namespace Luna
{
	struct TypeBuilder;

	//! Creates one type builder instance.
	//! @param[in] name The full name of the type to build.
	//! @param[in] kind The target type kind to build for.
	//! @return Returns the new created type builder instance.
	LUNA_RUNTIME_API TypeBuilder* new_type_builder(const Name& name, UserDefinedTypeKind kind);

	//! Destroys the specified builder.
	//! @param[in] builder The builder to delete.
	LUNA_RUNTIME_API void delete_type_builder(TypeBuilder* builder);

	//! Resets the type builder so that it can be reused to build another different type.
	//! @param[in] builder The type builder to reset.
	//! @param[in] name The full name of the new type to build.
	//! @param[in] kind The new target type kind to build for.
	LUNA_RUNTIME_API void reset_type_builder(TypeBuilder* builder, const Name& name, UserDefinedTypeKind kind);

	//! Applies the type contained in the builder to the specified assembly.
	//! @param[in] builder The operated builder.
	//! @param[in] assembly The assembly to apply the type builder to.
	LUNA_RUNTIME_API void apply_type_builder(TypeBuilder* builder, Assembly* assembly);

	//! Sets the underlying type for the type builder. This is only valid if the type builder kind
	//! is "enumeration".
	//! @param[in] builder The operated builder.
	//! @param[in] underlying_type The underlying type to set. This must refer to one primitive integer type 
	//! (for example "u32"). The default underlying type is `i32`.
	LUNA_RUNTIME_API void type_builder_set_underlying_type(TypeBuilder* builder, const Name& underlying_type);

	//! Specify if the enumeration type in the type builder is a multiple enumeration type.
	//! @param[in] builder The operated builder.
	//! @param[in] is_multienum If `true`, this type is a multiple enumeration type; if `false`, this type is not.
	//! @remark The enumeration type is not a multiple enumeration type by default.
	LUNA_RUNTIME_API void type_builder_set_multienum(TypeBuilder* builder, bool is_multienum);

	//! Adds one enumeration item to the type builder.
	//! @param[in] builder The operated builder.
	//! @param[in] item_name The name of the item.
	//! @param[in] mapped_value The mapped value of the item.
	//! @param[in] associated_type The associated type of the item. Specify one empty name if this item 
	//! does not have associated type.
	LUNA_RUNTIME_API void type_builder_add_enumeration_item(TypeBuilder* builder, const Name& item_name,
		i64 mapped_value, const Name& associated_type = Name());
}