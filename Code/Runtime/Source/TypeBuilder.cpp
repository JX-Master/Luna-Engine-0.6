// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file TypeBuilder.cpp
* @author JXMaster
* @date 2021/4/19
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "TypeBuilder.hpp"
#include <Runtime/Assert.hpp>

namespace Luna
{
	LUNA_RUNTIME_API TypeBuilder* new_type_builder(const Name& name, UserDefinedTypeKind kind)
	{
		TypeBuilder* builder = memnew<TypeBuilder>(name, kind);
		return builder;
	}

	LUNA_RUNTIME_API void delete_type_builder(TypeBuilder* builder)
	{
		lucheck(builder);
		memdelete(builder);
	}

	LUNA_RUNTIME_API void reset_type_builder(TypeBuilder* builder, const Name& name, UserDefinedTypeKind kind)
	{
		lucheck(builder);
		builder->name = name;
		builder->build_type = AssemblyType(kind);
	}

	LUNA_RUNTIME_API void apply_type_builder(TypeBuilder* builder, Assembly* assembly)
	{
		lucheck(builder && assembly);
		assembly->types.insert(Pair<Name, AssemblyType>(builder->name, builder->build_type));
	}

	LUNA_RUNTIME_API void type_builder_set_underlying_type(TypeBuilder* builder, const Name& underlying_type)
	{
		lucheck(builder);
		if (builder->build_type.kind == UserDefinedTypeKind::enumeration_type)
		{
			builder->build_type.enumeration_type.underlying_type = underlying_type;
		}
	}

	LUNA_RUNTIME_API void type_builder_set_multienum(TypeBuilder* builder, bool is_multienum)
	{
		lucheck(builder);
		if (builder->build_type.kind == UserDefinedTypeKind::enumeration_type)
		{
			builder->build_type.enumeration_type.is_multienum = is_multienum;
		}
	}

	LUNA_RUNTIME_API void type_builder_add_enumeration_item(TypeBuilder* builder, const Name& item_name,
		i64 mapped_value, const Name& associated_type)
	{
		lucheck(builder);
		if (builder->build_type.kind == UserDefinedTypeKind::enumeration_type)
		{
			AssemblyTypeEnumerationItem item;
			item.name = item_name;
			item.mapped_value = mapped_value;
			item.associated_name = associated_type;
			builder->build_type.enumeration_type.items.push_back(item);
		}
	}
}