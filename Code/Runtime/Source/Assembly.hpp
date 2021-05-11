// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Assembly.hpp
* @author JXMaster
* @date 2021/4/19
*/
#pragma once
#include "../Runtime.hpp"
#include <Runtime/Vector.hpp>
#include <Runtime/HashMultiMap.hpp>

namespace Luna
{
	struct AssemblyTypeEnumerationItem
	{
		Name name;
		Name associated_name;
		i64 mapped_value;
	};
	struct AssemblyTypeEnumeration
	{
		bool is_multienum;
		Name underlying_type;
		Vector<AssemblyTypeEnumerationItem> items;

		AssemblyTypeEnumeration() :
			is_multienum(false),
			underlying_type(u8"i32") {}
	};
	struct AssemblyTypeStructureProperty
	{
		Name name;
		String type;
	};
	struct AssemblyTypeStructureMethod
	{

	};
	struct AssemblyTypeStructure
	{
		usize size;
		usize alignment;
		Vector<AssemblyTypeStructureProperty> properties;
		Vector<AssemblyTypeStructureMethod> methods;
	};
	//! Represents one type record stored in 
	//! the assembly.
	struct AssemblyType
	{
		UserDefinedTypeKind kind;
		union
		{
			AssemblyTypeEnumeration enumeration_type;
			AssemblyTypeStructure structure_type;
		};

		void destruct()
		{
			switch (kind)
			{
			case UserDefinedTypeKind::enumeration_type:
				enumeration_type.~AssemblyTypeEnumeration();
				break;
			case UserDefinedTypeKind::structure_type:
				structure_type.~AssemblyTypeStructure();
				break;
			default:
				break;
			}
		}
		AssemblyType(UserDefinedTypeKind kind)
		{
			this->kind = kind;
			switch (kind)
			{
			case UserDefinedTypeKind::enumeration_type:
				new (&enumeration_type) AssemblyTypeEnumeration();
				break;
			case UserDefinedTypeKind::structure_type:
				new (&structure_type) AssemblyTypeStructure();
				break;
			default:
				break;
			}
		}
		AssemblyType(const AssemblyType& rhs)
		{
			kind = rhs.kind;
			switch (kind)
			{
			case UserDefinedTypeKind::enumeration_type:
				new (&enumeration_type) AssemblyTypeEnumeration(rhs.enumeration_type);
				break;
			case UserDefinedTypeKind::structure_type:
				new (&structure_type) AssemblyTypeStructure(rhs.structure_type);
				break;
			default:
				break;
			}
		}
		AssemblyType(AssemblyType&& rhs)
		{
			kind = rhs.kind;
			switch (kind)
			{
			case UserDefinedTypeKind::enumeration_type:
				new (&enumeration_type) AssemblyTypeEnumeration(move(rhs.enumeration_type));
				break;
			case UserDefinedTypeKind::structure_type:
				new (&structure_type) AssemblyTypeStructure(move(rhs.structure_type));
				break;
			default:
				break;
			}
		}
		AssemblyType& operator=(const AssemblyType& rhs)
		{
			destruct();
			kind = rhs.kind;
			switch (kind)
			{
			case UserDefinedTypeKind::enumeration_type:
				new (&enumeration_type) AssemblyTypeEnumeration(rhs.enumeration_type);
				break;
			case UserDefinedTypeKind::structure_type:
				new (&structure_type) AssemblyTypeStructure(rhs.structure_type);
				break;
			default:
				break;
			}
			return *this;
		}
		AssemblyType& operator=(AssemblyType&& rhs)
		{
			destruct();
			kind = rhs.kind;
			switch (kind)
			{
			case UserDefinedTypeKind::enumeration_type:
				new (&enumeration_type) AssemblyTypeEnumeration(move(rhs.enumeration_type));
				break;
			case UserDefinedTypeKind::structure_type:
				new (&structure_type) AssemblyTypeStructure(move(rhs.structure_type));
				break;
			default:
				break;
			}
			return *this;
		}
		~AssemblyType()
		{
			destruct();
		}
	};

	struct Assembly
	{
		HashMultiMap<Name, AssemblyType> types;
	};
}