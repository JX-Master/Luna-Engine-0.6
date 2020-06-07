// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IVarient.hpp
* @author JXMaster
* @date 2020/1/14
*/
#pragma once
#include "IBlob.hpp"
#include "IString.hpp"
#include "IName.hpp"
#include <Base/Vector.hpp>

namespace luna
{
	enum class EVariantType : uint8
	{
		null = 0,		// The node is null, it does not have any data buffer.
		
		boolean = 1,		// Use bit_test, bit_set and bit_reset to operate on bits.
		u8 = 2,				// Reinterpret buffer as uint8.
		i8 = 3,				// Reinterpret buffer as int8.

		u16 = 4,			// Reinterpret buffer as uint16.
		i16 = 5,			// Reinterpret buffer as int16.

		u32 = 6,			// Reinterpret buffer as uint32.
		i32 = 7,			// Reinterpret buffer as int32.
		f32 = 8,			// Reinterpret buffer as float32.

		u64 = 9,			// Reinterpret buffer as uint64.
		i64 = 10,			// Reinterpret buffer as int64.
		f64 = 11,			// Reinterpret buffer as float64.

		string = 12,		// Reinterpret buffer as P<IString>.
		name = 13,			// Reinterpret buffer as P<IName>.
		path = 14,			// Reinterpret buffer as P<IPath>.
		blob = 15,			// Reinterpret buffer as P<IBlob>.
		object = 16,		// Reinterpret buffer as P<IObject>. This type cannot be serialized, but is useful for passing data around the engine.

		variant = 17,		// Reinterpret buffer as P<IVariant>.
		table = 18,			// Do not fetch the buffer directly, use `get_fields`, `get_field` and `set_field` of `IVariant` instead.
	};

	//! @interface IVarient
	//! IVarient object represents one data node that can store one specific type of data.
	//! One variant can be described in two ways: The type of the element, and the size of the element array/matrix.
	//! The variant supports multi-dimensional data form 1D(array) to 8D, one single data is recognized as 1-D data with length 1.
	struct IVariant : public IObject
	{
		luiid("{761d1d4d-2c6e-40fb-a802-1b4bc2428655}");

		//! Gets the type of the data.
		virtual EVariantType type() = 0;

		//! Gets the dimension of the data(1-8 for 1D to 8D).
		virtual uint8 dimension() = 0;

		//! Gets the length of the data in the specified dimension(1-8 for 1D to 8D).
		//! Returns 1 if the data does not have that dimension, returns 0 only if type is `EVariantType::null`.
		virtual size_t length(uint8 dim) = 0;

		//! Gets the size of data buffer in bytes.
		virtual size_t buffer_size() = 0;

		//! Gets the number of elements in the buffer.
		virtual size_t size() = 0;

		//! Gets the data buffer. The user should offset the buffer pointer and reinterpret pointer to fetch the data.
		virtual void* buffer() = 0;

		//! Allocates a new data buffer, sets the new data buffer to the variant object and 
		//! returns the old data buffer as a new variant object.
		//! If this operation fails, the old data buffer is unchanged.
		//! Resets the variant with only one element, which can be `EVariantType::null`.
		virtual P<IVariant> reset(EVariantType t, size_t d1 = 1, size_t d2 = 1, size_t d3 = 1, size_t d4 = 1, 
			size_t d5 = 1, size_t d6 = 1, size_t d7 = 1, size_t d8 = 1) = 0;

		//! Gets all fields of the table at `table_index`.
		virtual R<Vector<P<IName>>> fields(size_t table_index) = 0;
		//! Gets the variant set to the specified field.
		//! The table keeps a strong reference to the variant until the field has been changed/removed.
		virtual R<IVariant*> field(size_t table_index, IName* field) = 0;
		//! Sets the specified field of the table.
		//! Set `value` to `nullptr` to remove the field.
		virtual void set_field(size_t table_index, IName* field, IVariant* value) = 0;

		//! Gets the data index in the data buffer. The index starts from 0.
		size_t index(size_t d1)
		{
			size_t l1 = length(1);
			luassert(d1 < l1);
			return d1;
		}
		size_t index(size_t d1, size_t d2)
		{
			size_t l1 = length(1);
			size_t l2 = length(2);
			luassert(d1 < l1 && d2 < l2);
			return d1 + d2 * l1;
		}
		size_t index(size_t d1, size_t d2, size_t d3)
		{
			size_t l1 = length(1);
			size_t l2 = length(2);
			size_t l3 = length(3);
			luassert(d1 < l1 && d2 < l2 && d3 < l3);
			return d1 + d2 * l1 + d3 * l1 * l2;
		}
		size_t index(size_t d1, size_t d2, size_t d3, size_t d4)
		{
			size_t l1 = length(1);
			size_t l2 = length(2);
			size_t l3 = length(3);
			size_t l4 = length(4);
			luassert(d1 < l1 && d2 < l2 && d3 < l3 && d4 < l4);
			return d1 + d2 * l1 + d3 * l1 * l2 + d4 * l1 * l2 * l3;
		}
		size_t index(size_t d1, size_t d2, size_t d3, size_t d4, size_t d5)
		{
			size_t l1 = length(1);
			size_t l2 = length(2);
			size_t l3 = length(3);
			size_t l4 = length(4);
			size_t l5 = length(5);
			luassert(d1 < l1 && d2 < l2 && d3 < l3 && d4 < l4 && d5 < l5);
			return d1 + d2 * l1 + d3 * l1 * l2 + d4 * l1 * l2 * l3 + d5 * l1 * l2 * l3 * l4;
		}
		size_t index(size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6)
		{
			size_t l1 = length(1);
			size_t l2 = length(2);
			size_t l3 = length(3);
			size_t l4 = length(4);
			size_t l5 = length(5);
			size_t l6 = length(6);
			luassert(d1 < l1 && d2 < l2 && d3 < l3 && d4 < l4 && d5 < l5 && d6 < l6);
			return d1 + d2 * l1 +
				d3 * l1 * l2 +
				d4 * l1 * l2 * l3 +
				d5 * l1 * l2 * l3 * l4 +
				d6 * l1 * l2 * l3 * l4 * l5;
		}
		size_t index(size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7)
		{
			size_t l1 = length(1);
			size_t l2 = length(2);
			size_t l3 = length(3);
			size_t l4 = length(4);
			size_t l5 = length(5);
			size_t l6 = length(6);
			size_t l7 = length(7);
			luassert(d1 < l1 && d2 < l2 && d3 < l3 && d4 < l4 && d5 < l5 && d6 < l6 && d7 < l7);
			return d1 + d2 * l1 +
				d3 * l1 * l2 +
				d4 * l1 * l2 * l3 +
				d5 * l1 * l2 * l3 * l4 +
				d6 * l1 * l2 * l3 * l4 * l5 +
				d7 * l1 * l2 * l3 * l4 * l5 * l6;
		}
		size_t index(size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7, size_t d8)
		{
			size_t l1 = length(1);
			size_t l2 = length(2);
			size_t l3 = length(3);
			size_t l4 = length(4);
			size_t l5 = length(5);
			size_t l6 = length(6);
			size_t l7 = length(7);
			size_t l8 = length(8);
			luassert(d1 < l1 && d2 < l2 && d3 < l3 && d4 < l4 && d5 < l5 && d6 < l6 && d7 < l7 && d8 < l8);
			return d1 + d2 * l1 +
				d3 * l1 * l2 +
				d4 * l1 * l2 * l3 +
				d5 * l1 * l2 * l3 * l4 +
				d6 * l1 * l2 * l3 * l4 * l5 +
				d7 * l1 * l2 * l3 * l4 * l5 * l6 +
				d8 * l1 * l2 * l3 * l4 * l5 * l6 * l7;
		}

		//! Creates a copy of this variant.
		//! The following types will be cloned:
		//! * Any numeric type (int, uint, float, bool, null).
		//! * string, path, blob
		//! * variant and table
		//! The following types will not be cloned, the two variant object points to the same object:
		//! * name (because name cannot be cloned).
		//! * object
		//virtual RP<IVariant> clone() = 0;
		
		// Helpers to get typed buffer pointer.
		// The `check`ed version checks the type of the buffer, while the non-`check`ed version
		// does not. If the type is wrong for non-`check`ed version, assertion failure 
		// will be triggered.

		void* boolean_buf()
		{
			luassert(type() == EVariantType::boolean);
			return buffer();
		}
		R<void*> check_boolean_buf()
		{
			if (type() == EVariantType::boolean)
			{
				return buffer();
			}
			return e_type_dismatch;
		}
		uint8* u8_buf()
		{
			luassert(type() == EVariantType::u8);
			return reinterpret_cast<uint8*>(buffer());
		}
		R<uint8*> check_u8_buf()
		{
			if (type() == EVariantType::u8)
			{
				return reinterpret_cast<uint8*>(buffer());
			}
			return e_type_dismatch;
		}
		uint8& u8()
		{
			return u8_buf()[0];
		}
		R<ReferenceWrapper<uint8>> check_u8()
		{
			if (type() == EVariantType::u8)
			{
				return ReferenceWrapper<uint8>(reinterpret_cast<uint8*>(buffer())[0]);
			}
			return e_type_dismatch;
		}
		int8* i8_buf()
		{
			luassert(type() == EVariantType::i8);
			return reinterpret_cast<int8*>(buffer());
		}
		R<int8*> check_i8_buf()
		{
			if (type() == EVariantType::i8)
			{
				return reinterpret_cast<int8*>(buffer());
			}
			return e_type_dismatch;
		}
		int8& i8()
		{
			return i8_buf()[0];
		}
		R<ReferenceWrapper<int8>> check_i8()
		{
			if (type() == EVariantType::i8)
			{
				return ReferenceWrapper<int8>(reinterpret_cast<int8*>(buffer())[0]);
			}
			return e_type_dismatch;
		}
		uint16* u16_buf()
		{
			luassert(type() == EVariantType::u16);
			return reinterpret_cast<uint16*>(buffer());
		}
		R<uint16*> check_u16_buf()
		{
			if (type() == EVariantType::u16)
			{
				return reinterpret_cast<uint16*>(buffer());
			}
			return e_type_dismatch;
		}
		uint16& u16()
		{
			return u16_buf()[0];
		}
		R<ReferenceWrapper<uint16>> check_u16()
		{
			if (type() == EVariantType::u16)
			{
				return ReferenceWrapper<uint16>(reinterpret_cast<uint16*>(buffer())[0]);
			}
			return e_type_dismatch;
		}
		int16* i16_buf()
		{
			luassert(type() == EVariantType::i16);
			return reinterpret_cast<int16*>(buffer());
		}
		R<int16*> check_i16_buf()
		{
			if (type() == EVariantType::i16)
			{
				return reinterpret_cast<int16*>(buffer());
			}
			return e_type_dismatch;
		}
		int16& i16()
		{
			return i16_buf()[0];
		}
		R<ReferenceWrapper<int16>> check_i16()
		{
			if (type() == EVariantType::i16)
			{
				return ReferenceWrapper<int16>(reinterpret_cast<int16*>(buffer())[0]);
			}
			return e_type_dismatch;
		}
		uint32* u32_buf()
		{
			luassert(type() == EVariantType::u32);
			return reinterpret_cast<uint32*>(buffer());
		}
		R<uint32*> check_u32_buf()
		{
			if (type() == EVariantType::u32)
			{
				return reinterpret_cast<uint32*>(buffer());
			}
			return e_type_dismatch;
		}
		uint32& u32()
		{
			return u32_buf()[0];
		}
		R<ReferenceWrapper<uint32>> check_u32()
		{
			if (type() == EVariantType::u32)
			{
				return ReferenceWrapper<uint32>(reinterpret_cast<uint32*>(buffer())[0]);
			}
			return e_type_dismatch;
		}
		int32* i32_buf()
		{
			luassert(type() == EVariantType::i32);
			return reinterpret_cast<int32*>(buffer());
		}
		R<int32*> check_i32_buf()
		{
			if (type() == EVariantType::i32)
			{
				return reinterpret_cast<int32*>(buffer());
			}
			return e_type_dismatch;
		}
		int32& i32()
		{
			return i32_buf()[0];
		}
		R<ReferenceWrapper<int32>> check_i32()
		{
			if (type() == EVariantType::i32)
			{
				return R<ReferenceWrapper<int32>>::success(reinterpret_cast<int32*>(buffer())[0]);
			}
			return e_type_dismatch;
		}
		float32* f32_buf()
		{
			luassert(type() == EVariantType::f32);
			return reinterpret_cast<float32*>(buffer());
		}
		R<float32*> check_f32_buf()
		{
			if (type() == EVariantType::f32)
			{
				return reinterpret_cast<float32*>(buffer());
			}
			return e_type_dismatch;
		}
		float32& f32()
		{
			return f32_buf()[0];
		}
		R<ReferenceWrapper<float32>> check_f32()
		{
			if (type() == EVariantType::f32)
			{
				return ReferenceWrapper<float32>(reinterpret_cast<float32*>(buffer())[0]);
			}
			return e_type_dismatch;
		}
		uint64* u64_buf()
		{
			luassert(type() == EVariantType::u64);
			return reinterpret_cast<uint64*>(buffer());
		}
		R<uint64*> check_u64_buf()
		{
			if (type() == EVariantType::u64)
			{
				return reinterpret_cast<uint64*>(buffer());
			}
			return e_type_dismatch;
		}
		uint64& u64()
		{
			return u64_buf()[0];
		}
		R<ReferenceWrapper<uint64>> check_u64()
		{
			if (type() == EVariantType::u64)
			{
				return ReferenceWrapper<uint64>(reinterpret_cast<uint64*>(buffer())[0]);
			}
			return e_type_dismatch;
		}
		int64* i64_buf()
		{
			luassert(type() == EVariantType::i64);
			return reinterpret_cast<int64*>(buffer());
		}
		R<int64*> check_i64_buf()
		{
			if (type() == EVariantType::i64)
			{
				return reinterpret_cast<int64*>(buffer());
			}
			return e_type_dismatch;
		}
		int64& i64()
		{
			return i64_buf()[0];
		}
		R<ReferenceWrapper<int64>> check_i64()
		{
			if (type() == EVariantType::i64)
			{
				return ReferenceWrapper<int64>(reinterpret_cast<int64*>(buffer())[0]);
			}
			return e_type_dismatch;
		}
		float64* f64_buf()
		{
			luassert(type() == EVariantType::f64);
			return reinterpret_cast<float64*>(buffer());
		}
		R<float64*> check_f64_buf()
		{
			if (type() == EVariantType::f64)
			{
				return reinterpret_cast<float64*>(buffer());
			}
			return e_type_dismatch;
		}
		float64& f64()
		{
			return f64_buf()[0];
		}
		R<ReferenceWrapper<float64>> check_f64()
		{
			if (type() == EVariantType::f64)
			{
				return ReferenceWrapper<float64>(reinterpret_cast<float64*>(buffer())[0]);
			}
			return e_type_dismatch;
		}
		P<IString>* str_buf()
		{
			luassert(type() == EVariantType::string);
			return reinterpret_cast<P<IString>*>(buffer());
		}
		R<P<IString>*> check_str_buf()
		{
			if (type() == EVariantType::string)
			{
				return reinterpret_cast<P<IString>*>(buffer());
			}
			return e_type_dismatch;
		}
		P<IString>& str()
		{
			return str_buf()[0];
		}
		R<ReferenceWrapper<P<IString>>> check_str()
		{
			if (type() == EVariantType::string)
			{
				return reinterpret_cast<P<IString>*>(buffer())[0];
			}
			return e_type_dismatch;
		}
		P<IName>* name_buf()
		{
			luassert(type() == EVariantType::name);
			return reinterpret_cast<P<IName>*>(buffer());
		}
		R<P<IName>*> check_name_buf()
		{
			if (type() == EVariantType::name)
			{
				return reinterpret_cast<P<IName>*>(buffer());
			}
			return e_type_dismatch;
		}
		P<IName>& name()
		{
			return name_buf()[0];
		}
		R<ReferenceWrapper<P<IName>>> check_name()
		{
			if (type() == EVariantType::name)
			{
				return reinterpret_cast<P<IName>*>(buffer())[0];
			}
			return e_type_dismatch;
		}
		P<IPath>* path_buf()
		{
			luassert(type() == EVariantType::path);
			return reinterpret_cast<P<IPath>*>(buffer());
		}
		R<P<IPath>*> check_path_buf()
		{
			if (type() == EVariantType::path)
			{
				return reinterpret_cast<P<IPath>*>(buffer());
			}
			return e_type_dismatch;
		}
		P<IPath>& path()
		{
			return path_buf()[0];
		}
		R<ReferenceWrapper<P<IPath>>> check_path()
		{
			if (type() == EVariantType::path)
			{
				return reinterpret_cast<P<IPath>*>(buffer())[0];
			}
			return e_type_dismatch;
		}
		P<IBlob>* blob_buf()
		{
			luassert(type() == EVariantType::blob);
			return reinterpret_cast<P<IBlob>*>(buffer());
		}
		R<P<IBlob>*> check_blob_buf()
		{
			if (type() == EVariantType::blob)
			{
				return reinterpret_cast<P<IBlob>*>(buffer());
			}
			return e_type_dismatch;
		}
		P<IBlob>& blob()
		{
			return blob_buf()[0];
		}
		R<ReferenceWrapper<P<IBlob>>> check_blob()
		{
			if (type() == EVariantType::blob)
			{
				return reinterpret_cast<P<IBlob>*>(buffer())[0];
			}
			return e_type_dismatch;
		}
		P<IObject>* obj_buf()
		{
			luassert(type() == EVariantType::object);
			return reinterpret_cast<P<IObject>*>(buffer());
		}
		R<P<IObject>*> check_obj_buf()
		{
			if (type() == EVariantType::object)
			{
				return reinterpret_cast<P<IObject>*>(buffer());
			}
			return e_type_dismatch;
		}
		P<IObject>& obj()
		{
			return obj_buf()[0];
		}
		R<ReferenceWrapper<P<IObject>>> check_obj()
		{
			if (type() == EVariantType::object)
			{
				return reinterpret_cast<P<IObject>*>(buffer())[0];
			}
			return e_type_dismatch;
		}
		P<IVariant>* var_buf()
		{
			luassert(type() == EVariantType::variant);
			return reinterpret_cast<P<IVariant>*>(buffer());
		}
		R<P<IVariant>*> check_var_buf()
		{
			if (type() == EVariantType::variant)
			{
				return reinterpret_cast<P<IVariant>*>(buffer());
			}
			return e_type_dismatch;
		}
		P<IVariant>& var()
		{
			return var_buf()[0];
		}
		R<ReferenceWrapper<P<IVariant>>> check_var()
		{
			if (type() == EVariantType::variant)
			{
				return reinterpret_cast<P<IVariant>*>(buffer())[0];
			}
			return e_type_dismatch;
		}
	};
}