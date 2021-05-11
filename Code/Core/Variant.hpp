// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Varient.hpp
* @author JXMaster
* @date 2020/1/14
*/
#pragma once
#include <Runtime/Blob.hpp>
#include <Runtime/String.hpp>
#include <Runtime/Name.hpp>
#include <Runtime/Vector.hpp>
#include <Runtime/HashMap.hpp>
#include <Runtime/Result.hpp>
#include <Runtime/Path.hpp>
#include "IObject.hpp"
#include <Runtime/TSAssert.hpp>

namespace Luna
{
	enum class EVariantType : u8
	{
		null = 0,		// The node is null, it does not have any data buffer.
		
		boolean = 1,		// Use bit_test, bit_set and bit_reset to operate on bits.
		u8 = 2,				// Reinterpret buffer as u8.
		i8 = 3,				// Reinterpret buffer as i8.

		u16 = 4,			// Reinterpret buffer as u16.
		i16 = 5,			// Reinterpret buffer as i16.

		u32 = 6,			// Reinterpret buffer as u32.
		i32 = 7,			// Reinterpret buffer as i32.
		f32 = 8,			// Reinterpret buffer as f32.

		u64 = 9,			// Reinterpret buffer as u64.
		i64 = 10,			// Reinterpret buffer as i64.
		f64 = 11,			// Reinterpret buffer as f64.

		string = 12,		// Reinterpret buffer as String.
		name = 13,			// Reinterpret buffer as Name.
		path = 14,			// Reinterpret buffer as Path.
		blob = 15,			// Reinterpret buffer as Blob.
		object = 16,		// Reinterpret buffer as P<IObject>. This type cannot be serialized, but is useful for passing data around the engine.

		variant = 17,		// Reinterpret buffer as Variant.
		table = 18,			// Do not fetch the buffer directly, use `get_fields`, `get_field` and `set_field` of `IVariant` instead.
	};

	class Variant;

	//! The npos variant is returned when no matching field is found in `Variant::field` call. The npos variant always has the
	//! `nil` type and cannot be changed. Most non-const operations to `npos` is forbidden and will throw errors.
	LUNA_CORE_API Variant& get_npos_variant();

	//! @interface Variant
	//! Variant object represents one data node that can store one specific type of data.
	//! One variant can be described in two ways: The type of the element, and the size of the element array/matrix.
	//! The variant supports multi-dimensional data form 1D(array) to 8D, one single data is recognized as 1-D data with length 1.
	class Variant
	{
	public:
		Variant();
		Variant(EVariantType t);
		Variant(EVariantType t, usize d1);
		Variant(EVariantType t, usize d1, usize d2);
		Variant(EVariantType t, usize d1, usize d2, usize d3);
		Variant(EVariantType t, usize d1, usize d2, usize d3, usize d4);
		Variant(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5);
		Variant(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5, usize d6);
		Variant(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5, usize d6, usize d7);
		Variant(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5, usize d6, usize d7, usize d8);
		Variant(const Variant& rhs);
		Variant(Variant&& rhs);
		Variant& operator=(const Variant& rhs);
		Variant& operator=(Variant&& rhs);
		~Variant();

		//! Gets the type of the data.
		EVariantType type() const;

		//! Gets the dimension of the data(1-8 for 1D to 8D).
		u8 dimension() const;

		//! Gets the length of the data in the specified dimension(1-8 for 1D to 8D).
		//! Returns 1 if the data does not have that dimension, returns 0 only if type is `EVariantType::null`.
		usize length(u8 dim) const;

		//! Gets the size of data buffer in bytes.
		usize buffer_size() const;

		//! Gets the number of elements in the buffer.
		usize size() const;

		//! Gets the variant set to the specified field.
		//! The reference is invalidated when the is being changed (insert/erase elements).
		//! If the field is not set, or if this variant is not a table type, one null variant will be returned.
		const Variant& field(usize table_index, const Name& field) const;
		Variant& field(usize table_index, const Name& field);

		//! Gets all field names.
		Vector<Name> fields(usize table_index) const;

		//! Sets the specified field of the table. This variant must be of a table type.
		//! Set `value` to `nullptr` to remove the field.
		void set_field(usize table_index, const Name& field, const Variant& value);
		void set_field(usize table_index, const Name& field, Variant&& value);

		//! Gets the data buffer. The user should offset the buffer pointer and reinterpret pointer to fetch the data.
		void* buffer();
		const void* buffer() const;

		//! Gets the data index in the data buffer. The index starts from 0.
		usize index(usize d1) const;
		usize index(usize d1, usize d2) const;
		usize index(usize d1, usize d2, usize d3) const;
		usize index(usize d1, usize d2, usize d3, usize d4) const;
		usize index(usize d1, usize d2, usize d3, usize d4, usize d5) const;
		usize index(usize d1, usize d2, usize d3, usize d4, usize d5, usize d6) const;
		usize index(usize d1, usize d2, usize d3, usize d4, usize d5, usize d6, usize d7) const;
		usize index(usize d1, usize d2, usize d3, usize d4, usize d5, usize d6, usize d7, usize d8) const;

		void* to_boolean_buf();
		const void* to_boolean_buf() const;
		R<void*> check_boolean_buf();
		R<const void*> check_boolean_buf() const;

		u8* to_u8_buf();
		const u8* to_u8_buf() const;
		R<u8*> check_u8_buf();
		R<const u8*> check_u8_buf() const;
		u8& to_u8();
		const u8& to_u8() const;
		R<ReferenceWrapper<u8>> check_u8();
		R<ReferenceWrapper<const u8>> check_u8() const;

		i8* to_i8_buf();
		const i8* to_i8_buf() const;
		R<i8*> check_i8_buf();
		R<const i8*> check_i8_buf() const;
		i8& to_i8();
		const i8& to_i8() const;
		R<ReferenceWrapper<i8>> check_i8();
		R<ReferenceWrapper<const i8>> check_i8() const;

		u16* to_u16_buf();
		const u16* to_u16_buf() const;
		R<u16*> check_u16_buf();
		R<const u16*> check_u16_buf() const;
		u16& to_u16();
		const u16& to_u16() const;
		R<ReferenceWrapper<u16>> check_u16();
		R<ReferenceWrapper<const u16>> check_u16() const;

		i16* to_i16_buf();
		const i16* to_i16_buf() const;
		R<i16*> check_i16_buf();
		R<const i16*> check_i16_buf() const;
		i16& to_i16();
		const i16& to_i16() const;
		R<ReferenceWrapper<i16>> check_i16();
		R<ReferenceWrapper<const i16>> check_i16() const;

		u32* to_u32_buf();
		const u32* to_u32_buf() const;
		R<u32*> check_u32_buf();
		R<const u32*> check_u32_buf() const;
		u32& to_u32();
		const u32& to_u32() const;
		R<ReferenceWrapper<u32>> check_u32();
		R<ReferenceWrapper<const u32>> check_u32() const;

		i32* to_i32_buf();
		const i32* to_i32_buf() const;
		R<i32*> check_i32_buf();
		R<const i32*> check_i32_buf() const;
		i32& to_i32();
		const i32& to_i32() const;
		R<ReferenceWrapper<i32>> check_i32();
		R<ReferenceWrapper<const i32>> check_i32() const;

		f32* to_f32_buf();
		const f32* to_f32_buf() const;
		R<f32*> check_f32_buf();
		R<const f32*> check_f32_buf() const;
		f32& to_f32();
		const f32& to_f32() const;
		R<ReferenceWrapper<f32>> check_f32();
		R<ReferenceWrapper<const f32>> check_f32() const;

		u64* to_u64_buf();
		const u64* to_u64_buf() const;
		R<u64*> check_u64_buf();
		R<const u64*> check_u64_buf() const;
		u64& to_u64();
		const u64& to_u64() const;
		R<ReferenceWrapper<u64>> check_u64();
		R<ReferenceWrapper<const u64>> check_u64() const;

		i64* to_i64_buf();
		const i64* to_i64_buf() const;
		R<i64*> check_i64_buf();
		R<const i64*> check_i64_buf() const;
		i64& to_i64();
		const i64& to_i64() const;
		R<ReferenceWrapper<i64>> check_i64();
		R<ReferenceWrapper<const i64>> check_i64() const;

		f64* to_f64_buf();
		const f64* to_f64_buf() const;
		R<f64*> check_f64_buf();
		R<const f64*> check_f64_buf() const;
		f64& to_f64();
		const f64& to_f64() const;
		R<ReferenceWrapper<f64>> check_f64();
		R<ReferenceWrapper<const f64>> check_f64() const;

		String* to_str_buf();
		const String* to_str_buf() const;
		R<String*> check_str_buf();
		R<const String*> check_str_buf() const;
		String& to_str();
		const String& to_str() const;
		R<ReferenceWrapper<String>> check_str();
		R<ReferenceWrapper<const String>> check_str() const;

		Name* to_name_buf();
		const Name* to_name_buf() const;
		R<Name*> check_name_buf();
		R<const Name*> check_name_buf() const;
		Name& to_name();
		const Name& to_name() const;
		R<ReferenceWrapper<Name>> check_name();
		R<ReferenceWrapper<const Name>> check_name() const;

		Path* to_path_buf();
		const Path* to_path_buf() const;
		R<Path*> check_path_buf();
		R<const Path*> check_path_buf() const;
		Path& to_path();
		const Path& to_path() const;
		R<ReferenceWrapper<Path>> check_path();
		R<ReferenceWrapper<const Path>> check_path() const;

		Blob* to_blob_buf();
		const Blob* to_blob_buf() const;
		R<Blob*> check_blob_buf();
		R<const Blob*> check_blob_buf() const;
		Blob& to_blob();
		const Blob& to_blob() const;
		R<ReferenceWrapper<Blob>> check_blob();
		R<ReferenceWrapper<const Blob>> check_blob() const;

		P<IObject>* to_obj_buf();
		const P<IObject>* to_obj_buf() const;
		R<P<IObject>*> check_obj_buf();
		R<const P<IObject>*> check_obj_buf() const;
		P<IObject>& to_obj();
		const P<IObject>& to_obj() const;
		R<ReferenceWrapper<P<IObject>>> check_obj();
		R<ReferenceWrapper<const P<IObject>>> check_obj() const;

		Variant* to_var_buf();
		const Variant* to_var_buf() const;
		R<Variant*> check_var_buf();
		R<const Variant*> check_var_buf() const;
		Variant& to_var();
		const Variant& to_var() const;
		R<ReferenceWrapper<Variant>> check_var();
		R<ReferenceWrapper<const Variant>> check_var() const;

		using VariantTable = HashMap<Name, Variant>;

		VariantTable* to_var_table_buf();
		const VariantTable* to_var_table_buf() const;
		R<VariantTable*> check_var_table_buf();
		R<const VariantTable*> check_var_table_buf() const;
		VariantTable& to_var_table();
		const VariantTable& to_var_table() const;
		R<ReferenceWrapper<VariantTable>> check_var_table();
		R<ReferenceWrapper<const VariantTable>> check_var_table() const;

	private:
		enum class EVariantFlag : u8
		{
			none = 0,
			// The data is placed in the variant directly.
			embedded = 0x01,
		};

		// Low 3 bits: dimension. High 5 bits: EVariantType.
		u8 m_flag;
		EVariantFlag m_flag2;

		// To reduce allocation, if there is only one element in the variant, we
		// embedded the element directly in the variant. This may happen only if:
		// 1. The variant is created with `Variant(EVariantType t)`. or
		// 2. The variant is created with `Variant(EVariantType t, usize d1)`, with d1 == 1.
		// The 2 cases above is actually the same.
		//
		// This part takes 8 bytes for most.
		union DataUnion
		{
			// Embedded part.
			u8 m_bool;
			u8 m_u8;
			i8 m_i8;
			u16 m_u16;
			i16 m_i16;
			u32 m_u32;
			i32 m_i32;
			f32 m_f32;
			u64 m_u64;
			i64 m_i64;
			f64 m_f64;
			Name m_name;
			P<IObject> m_obj;

			// The data block layout:
			// The first `n` usize value stores the length for each dimension.
			// `n` is:
			// * 2 for 1-D and 2-D
			// * 4 for 3-D and 4-D
			// * 6 for 5-D and 6-D
			// * 8 for 7-D and 8-D
			// (The unused dimension always filled with value 1).
			//
			// Then is the buffer for real data.
			void* m_data;

			DataUnion() {}
			~DataUnion() {}
		};

		DataUnion m_u;
		lutsassert_lock();

		static usize elem_size(EVariantType ty)
		{
			u8 t = (u8)ty;
			if (t == (u8)EVariantType::null)
			{
				return 0;
			}
			if (t <= (u8)EVariantType::i8)
			{
				return 1;
			}
			if (t <= (u8)EVariantType::i16)
			{
				return 2;
			}
			if (t <= (u8)EVariantType::f32)
			{
				return 4;
			}
			if (t <= (u8)EVariantType::f64)
			{
				return 8;
			}
			if (t == (u8)EVariantType::string)
			{
				return sizeof(String);
			}
			if (t == (u8)EVariantType::name)
			{
				return sizeof(Name);
			}
			if (t == (u8)EVariantType::path)
			{
				return sizeof(Path);
			}
			if (t == (u8)EVariantType::blob)
			{
				return sizeof(Blob);
			}
			if (t == (u8)EVariantType::object)
			{
				return sizeof(P<IObject>);
			}
			if (t == (u8)EVariantType::variant)
			{
				return sizeof(Variant);
			}
			if (t == (u8)EVariantType::table)
			{
				return sizeof(VariantTable);
			}
			lupanic();
			return 0;
		}

		// alloc_buf is used for non-embedded cases.
		static usize* alloc_buf(usize header_sz, EVariantType t, usize count)
		{
			luassert(t != EVariantType::null);
			usize sz;
			if (t == EVariantType::boolean)
			{
				sz = header_sz + align_upper(count, 8) / 8;
			}
			else
			{
				sz = header_sz + Variant::elem_size(t) * count;
			}
			usize* buf = (usize*)memalloc(sz);
			// Construct.
			void* data_begin = (void*)((usize)buf + header_sz);
			for (usize i = 0; i < count; ++i)
			{
				switch (t)
				{
				case EVariantType::string:
					new ((String*)data_begin + i) String(); break;
				case EVariantType::name:
					new ((Name*)data_begin + i) Name(); break;
				case EVariantType::path:
					new ((Path*)data_begin + i) Path(); break;
				case EVariantType::blob:
					new ((Blob*)data_begin + i) Blob(); break;
				case EVariantType::object:
					new ((P<IObject>*)data_begin + i) P<IObject>(); break;
				case EVariantType::variant:
					new ((Variant*)data_begin + i) Variant(EVariantType::null); break;
				case EVariantType::table:
					new ((VariantTable*)data_begin + i) VariantTable(); break;
				default: break;
				}
			}
			return buf;
		}

		static void* init_1d(EVariantType t, usize d1)
		{
			constexpr usize header_sz = sizeof(usize) * 2;
			usize count = d1;
			usize* buf = alloc_buf(header_sz, t, count);
			if (!buf)
			{
				return nullptr;
			}
			buf[0] = d1;
			buf[1] = 1;
			return buf;
		}

		static void* Variant::init_2d(EVariantType t, usize d1, usize d2)
		{
			constexpr usize header_sz = sizeof(usize) * 2;
			usize count = d1 * d2;
			usize* buf = alloc_buf(header_sz, t, count);
			if (!buf)
			{
				return nullptr;
			}
			buf[0] = d1;
			buf[1] = d2;
			return buf;
		}

		static void* Variant::init_3d(EVariantType t, usize d1, usize d2, usize d3)
		{
			constexpr usize header_sz = sizeof(usize) * 4;
			usize count = d1 * d2 * d3;
			usize* buf = alloc_buf(header_sz, t, count);
			if (!buf)
			{
				return nullptr;
			}
			buf[0] = d1;
			buf[1] = d2;
			buf[2] = d3;
			buf[3] = 1;
			return buf;
		}

		static void* Variant::init_4d(EVariantType t, usize d1, usize d2, usize d3, usize d4)
		{
			constexpr usize header_sz = sizeof(usize) * 4;
			usize count = d1 * d2 * d3 * d4;
			usize* buf = alloc_buf(header_sz, t, count);
			if (!buf)
			{
				return nullptr;
			}
			buf[0] = d1;
			buf[1] = d2;
			buf[2] = d3;
			buf[3] = d4;
			return buf;
		}

		static void* Variant::init_5d(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5)
		{
			constexpr usize header_sz = sizeof(usize) * 6;
			usize count = d1 * d2 * d3 * d4 * d5;
			usize* buf = alloc_buf(header_sz, t, count);
			if (!buf)
			{
				return nullptr;
			}
			buf[0] = d1;
			buf[1] = d2;
			buf[2] = d3;
			buf[3] = d4;
			buf[4] = d5;
			buf[5] = 1;
			return buf;
		}

		static void* Variant::init_6d(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5, usize d6)
		{
			constexpr usize header_sz = sizeof(usize) * 6;
			usize count = d1 * d2 * d3 * d4 * d5 * d6;
			usize* buf = alloc_buf(header_sz, t, count);
			if (!buf)
			{
				return nullptr;
			}
			buf[0] = d1;
			buf[1] = d2;
			buf[2] = d3;
			buf[3] = d4;
			buf[4] = d5;
			buf[5] = d6;
			return buf;
		}

		static void* Variant::init_7d(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5, usize d6, usize d7)
		{
			constexpr usize header_sz = sizeof(usize) * 8;
			usize count = d1 * d2 * d3 * d4 * d5 * d6 * d7;
			usize* buf = alloc_buf(header_sz, t, count);
			if (!buf)
			{
				return nullptr;
			}
			buf[0] = d1;
			buf[1] = d2;
			buf[2] = d3;
			buf[3] = d4;
			buf[4] = d5;
			buf[5] = d6;
			buf[6] = d7;
			buf[7] = 1;
			return buf;
		}

		static void* Variant::init_8d(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5, usize d6, usize d7, usize d8)
		{
			constexpr usize header_sz = sizeof(usize) * 8;
			usize count = d1 * d2 * d3 * d4 * d5 * d6 * d7 * d8;
			usize* buf = alloc_buf(header_sz, t, count);
			if (!buf)
			{
				return nullptr;
			}
			buf[0] = d1;
			buf[1] = d2;
			buf[2] = d3;
			buf[3] = d4;
			buf[4] = d5;
			buf[5] = d6;
			buf[6] = d7;
			buf[7] = d8;
			return buf;
		}

		void default_init()
		{
			m_flag = 0;
			m_flag2 = EVariantFlag::none;
			set_type(EVariantType::null);
			set_dimension(1);
			set_embedded(true);
			memzero(&m_u, sizeof(DataUnion));
		}

		void copy_init(const Variant& rhs)
		{
			default_init();
			m_flag = rhs.m_flag;
			m_flag2 = rhs.m_flag2;
			// Copy data.
			if (rhs.embedded())
			{
				switch (rhs.type())
				{
				case EVariantType::null: break;
				case EVariantType::boolean:
					m_u.m_bool = rhs.m_u.m_bool;
					break;
				case EVariantType::u8:
					m_u.m_u8 = rhs.m_u.m_u8;
					break;
				case EVariantType::i8:
					m_u.m_i8 = rhs.m_u.m_i8;
					break;
				case EVariantType::u16:
					m_u.m_u16 = rhs.m_u.m_u16;
					break;
				case EVariantType::i16:
					m_u.m_i16 = rhs.m_u.m_i16;
					break;
				case EVariantType::u32:
					m_u.m_u32 = rhs.m_u.m_u32;
					break;
				case EVariantType::i32:
					m_u.m_i32 = rhs.m_u.m_i32;
					break;
				case EVariantType::f32:
					m_u.m_f32 = rhs.m_u.m_f32;
					break;
				case EVariantType::u64:
					m_u.m_u64 = rhs.m_u.m_u64;
					break;
				case EVariantType::i64:
					m_u.m_i64 = rhs.m_u.m_i64;
					break;
				case EVariantType::f64:
					m_u.m_f64 = rhs.m_u.m_f64;
					break;
				case EVariantType::name:
					m_u.m_name = rhs.m_u.m_name;
					break;
				case EVariantType::object:
					m_u.m_obj = rhs.m_u.m_obj;
					break;
				default:
					lupanic();
				}
			}
			else
			{
				usize dim = rhs.dimension();
				switch (dim)
				{
				case 1: m_u.m_data = init_1d(rhs.type(), rhs.length(1)); break;
				case 2: m_u.m_data = init_2d(rhs.type(), rhs.length(1), rhs.length(2)); break;
				case 3: m_u.m_data = init_3d(rhs.type(), rhs.length(1), rhs.length(2), rhs.length(3)); break;
				case 4: m_u.m_data = init_4d(rhs.type(), rhs.length(1), rhs.length(2), rhs.length(3), rhs.length(4)); break;
				case 5: m_u.m_data = init_5d(rhs.type(), rhs.length(1), rhs.length(2), rhs.length(3), rhs.length(4), rhs.length(5)); break;
				case 6: m_u.m_data = init_6d(rhs.type(), rhs.length(1), rhs.length(2), rhs.length(3), rhs.length(4), rhs.length(5), rhs.length(6)); break;
				case 7: m_u.m_data = init_7d(rhs.type(), rhs.length(1), rhs.length(2), rhs.length(3), rhs.length(4), rhs.length(5), rhs.length(6), rhs.length(7)); break;
				case 8: m_u.m_data = init_8d(rhs.type(), rhs.length(1), rhs.length(2), rhs.length(3), rhs.length(4), rhs.length(5), rhs.length(6), rhs.length(7), rhs.length(8)); break;
				default: lupanic(); break;
				}
				// Copy elements.
				if ((u8)rhs.type() <= (u8)EVariantType::f64)
				{
					memcpy(buffer(), rhs.buffer(), rhs.buffer_size());
				}
				else
				{
					for (usize i = 0; i < rhs.size(); ++i)
					{
						switch (rhs.type())
						{
						case EVariantType::string: to_str_buf()[i] = rhs.to_str_buf()[i]; break;
						case EVariantType::name: to_name_buf()[i] = rhs.to_name_buf()[i]; break;
						case EVariantType::path: to_path_buf()[i] = rhs.to_path_buf()[i]; break;
						case EVariantType::blob: to_blob_buf()[i] = rhs.to_blob_buf()[i]; break;
						case EVariantType::object: to_obj_buf()[i] = rhs.to_obj_buf()[i]; break;
						case EVariantType::variant: to_var_buf()[i] = rhs.to_var_buf()[i]; break;
						case EVariantType::table: to_var_table_buf()[i] = rhs.to_var_table_buf()[i]; break;
						default: lupanic(); break;
						}
					}
				}
			}
		}

		void move_init(Variant&& rhs)
		{
			default_init();
			m_flag = rhs.m_flag;
			m_flag2 = rhs.m_flag2;
			// Transfer data.
			if (rhs.embedded())
			{
				switch (rhs.type())
				{
				case EVariantType::null:break;
				case EVariantType::boolean:
					m_u.m_bool = rhs.m_u.m_bool;
					break;
				case EVariantType::u8:
					m_u.m_u8 = rhs.m_u.m_u8;
					break;
				case EVariantType::i8:
					m_u.m_i8 = rhs.m_u.m_i8;
					break;
				case EVariantType::u16:
					m_u.m_u16 = rhs.m_u.m_u16;
					break;
				case EVariantType::i16:
					m_u.m_i16 = rhs.m_u.m_i16;
					break;
				case EVariantType::u32:
					m_u.m_u32 = rhs.m_u.m_u32;
					break;
				case EVariantType::i32:
					m_u.m_i32 = rhs.m_u.m_i32;
					break;
				case EVariantType::f32:
					m_u.m_f32 = rhs.m_u.m_f32;
					break;
				case EVariantType::u64:
					m_u.m_u64 = rhs.m_u.m_u64;
					break;
				case EVariantType::i64:
					m_u.m_i64 = rhs.m_u.m_i64;
					break;
				case EVariantType::f64:
					m_u.m_f64 = rhs.m_u.m_f64;
					break;
				case EVariantType::name:
					m_u.m_name = move(rhs.m_u.m_name);
					break;
				case EVariantType::object:
					m_u.m_obj = rhs.m_u.m_obj;
					break;
				default:
					lupanic();
				}
			}
			else
			{
				m_u.m_data = rhs.m_u.m_data;
			}
			// Reset rhs.
			rhs.default_init();
		}

		void destruct()
		{
			lutsassert();
			EVariantType t = type();
			switch (t)
			{
			case EVariantType::string:
			{
				String* s = to_str_buf();
				usize sz = size();
				for (usize i = 0; i < sz; ++i)
				{
					s[i].~String();
				}
			}
			break;
			case EVariantType::name:
			{
				Name* s = to_name_buf();
				usize sz = size();
				for (usize i = 0; i < sz; ++i)
				{
					s[i].~Name();
				}
			}
			break;
			case EVariantType::path:
			{
				Path* s = to_path_buf();
				usize sz = size();
				for (usize i = 0; i < sz; ++i)
				{
					s[i].~Path();
				}
			}
			break;
			case EVariantType::blob:
			{
				Blob* s = to_blob_buf();
				usize sz = size();
				for (usize i = 0; i < sz; ++i)
				{
					s[i].~Blob();
				}
			}
			break;
			case EVariantType::object:
			{
				P<IObject>* s = to_obj_buf();
				usize sz = size();
				for (usize i = 0; i < sz; ++i)
				{
					s[i].clear();
				}
			}
			break;
			case EVariantType::variant:
			{
				Variant* s = to_var_buf();
				usize sz = size();
				for (usize i = 0; i < sz; ++i)
				{
					s[i].~Variant();
				}
			}
			break;
			case EVariantType::table:
			{
				VariantTable* s = (VariantTable*)buffer();
				usize sz = size();
				for (usize i = 0; i < sz; ++i)
				{
					s[i].~VariantTable();
				}
			}
			break;
			default:
				break;
			}
			if (!embedded())
			{
				memfree(m_u.m_data);
				m_u.m_data = nullptr;
			}
			m_flag = 0;
		}

		void set_type(EVariantType t)
		{
			m_flag = (m_flag & 0x07) + ((u8)t << 3);
		}

		void set_dimension(u8 dim)
		{
			--dim;
			m_flag = (m_flag & 0xF8) + (dim & 0x07);
		}

		//! If `true`, then the data is embedded, else the data is stored in 
		//! dedicated buffer.
		//! 
		//! Only one elements with 1-D dimension can be embedded.
		bool embedded() const
		{
			return ((m_flag2 & EVariantFlag::embedded) != EVariantFlag::none);
		}

		static bool is_embedded_type(EVariantType t)
		{
			if ((t == EVariantType::string) || (t == EVariantType::path) || (t == EVariantType::blob) || (t == EVariantType::variant) || (t == EVariantType::table))
			{
				return false;
			}
			return true;
		}

		void set_embedded(bool e)
		{
			if (e)
			{
				m_flag2 |= EVariantFlag::embedded;
			}
			else
			{
				m_flag2 &= (EVariantFlag)(~(u8)(EVariantFlag::embedded));
			}
		}
	};
	inline Variant::Variant()
	{
		default_init();
	}
	inline Variant::Variant(EVariantType t)
	{
		default_init();
		if (!is_embedded_type(t))
		{
			set_embedded(false);
			m_u.m_data = init_1d(t, 1);
		}
		set_type(t);
	}
	inline Variant::Variant(EVariantType t, usize d1)
	{
		default_init();
		if (d1 == 1)
		{
			if (!is_embedded_type(t))
			{
				set_embedded(false);
				m_u.m_data = init_1d(t, 1);
			}
			set_type(t);
		}
		else
		{
			lucheck(t != EVariantType::null);
			m_u.m_data = init_1d(t, d1);
			set_type(t);
			set_dimension(1);
			set_embedded(false);
		}
	}
	inline Variant::Variant(EVariantType t, usize d1, usize d2)
	{
		default_init();
		lucheck(t != EVariantType::null);
		m_u.m_data = init_2d(t, d1, d2);
		set_type(t);
		set_dimension(2);
		set_embedded(false);
	}
	inline Variant::Variant(EVariantType t, usize d1, usize d2, usize d3)
	{
		default_init();
		lucheck(t != EVariantType::null);
		m_u.m_data = init_3d(t, d1, d2, d3);
		set_type(t);
		set_dimension(3);
		set_embedded(false);
	}
	inline Variant::Variant(EVariantType t, usize d1, usize d2, usize d3, usize d4)
	{
		default_init();
		lucheck(t != EVariantType::null);
		m_u.m_data = init_4d(t, d1, d2, d3, d4);
		set_type(t);
		set_dimension(4);
		set_embedded(false);
	}
	inline Variant::Variant(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5)
	{
		default_init();
		lucheck(t != EVariantType::null);
		m_u.m_data = init_5d(t, d1, d2, d3, d4, d5);
		set_type(t);
		set_dimension(5);
		set_embedded(false);
	}
	inline Variant::Variant(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5, usize d6)
	{
		default_init();
		lucheck(t != EVariantType::null);
		m_u.m_data = init_6d(t, d1, d2, d3, d4, d5, d6);
		set_type(t);
		set_dimension(6);
		set_embedded(false);
	}
	inline Variant::Variant(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5, usize d6, usize d7)
	{
		default_init();
		lucheck(t != EVariantType::null);
		m_u.m_data = init_7d(t, d1, d2, d3, d4, d5, d6, d7);
		set_type(t);
		set_dimension(7);
		set_embedded(false);
	}
	inline Variant::Variant(EVariantType t, usize d1, usize d2, usize d3, usize d4, usize d5, usize d6, usize d7, usize d8)
	{
		default_init();
		lucheck(t != EVariantType::null);
		m_u.m_data = init_8d(t, d1, d2, d3, d4, d5, d6, d7, d8);
		set_type(t);
		set_dimension(8);
		set_embedded(false);
	}
	inline Variant::Variant(const Variant& rhs)
	{
		copy_init(rhs);
	}
	inline Variant::Variant(Variant&& rhs)
	{
		move_init(move(rhs));
	}
	inline Variant& Variant::operator=(const Variant& rhs)
	{
		luassert(this != &get_npos_variant());
		destruct();
		copy_init(rhs);
		return *this;
	}
	inline Variant& Variant::operator=(Variant&& rhs)
	{
		luassert(this != &get_npos_variant());
		destruct();
		move_init(move(rhs));
		return *this;
	}
	inline Variant::~Variant()
	{
		destruct();
	}
	inline EVariantType Variant::type() const
	{
		return (EVariantType)((m_flag & 0xF8) >> 3);
	}
	inline u8 Variant::dimension() const
	{
		return (m_flag & 0x07) + 1;
	}
	inline usize Variant::length(u8 dim) const
	{
		lutsassert();
		if (type() == EVariantType::null)
		{
			return 0;
		}
		if (embedded() || dim > dimension())
		{
			return 1;
		}
		return ((usize*)(m_u.m_data))[dim - 1];
	}
	inline usize Variant::buffer_size() const
	{
		lutsassert();
		EVariantType t = type();
		if (embedded())
		{
			return elem_size(t);
		}
		u8 dim = dimension() - 1;
		usize* dims = (usize*)(m_u.m_data);
		if (t == EVariantType::boolean)
		{
			if (dim <= 1)
			{
				return align_upper(dims[0] * dims[1], 8) / 8;
			}
			if (dim <= 3)
			{
				return align_upper(dims[0] * dims[1] * dims[2] * dims[3], 8) / 8;
			}
			if (dim <= 5)
			{
				return align_upper(dims[0] * dims[1] * dims[2] * dims[3] * dims[4] * dims[5], 8) / 8;
			}
			return align_upper(dims[0] * dims[1] * dims[2] * dims[3] * dims[4] * dims[5] * dims[6] * dims[7], 8) / 8;
		}
		else
		{
			usize elem_sz = elem_size(t);
			if (dim <= 1)
			{
				return elem_sz * dims[0] * dims[1];
			}
			if (dim <= 3)
			{
				return elem_sz * dims[0] * dims[1] * dims[2] * dims[3];
			}
			if (dim <= 5)
			{
				return elem_sz * dims[0] * dims[1] * dims[2] * dims[3] * dims[4] * dims[5];
			}
			return elem_sz * dims[0] * dims[1] * dims[2] * dims[3] * dims[4] * dims[5] * dims[6] * dims[7];
		}
	}
	inline usize Variant::size() const
	{
		lutsassert();
		if (embedded())
		{
			return 1;
		}
		u8 dim = dimension() - 1;
		usize* dims = (usize*)(m_u.m_data);
		if (dim < 2)
		{
			return dims[0] * dims[1];
		}
		if (dim < 4)
		{
			return dims[0] * dims[1] * dims[2] * dims[3];
		}
		if (dim < 6)
		{
			return dims[0] * dims[1] * dims[2] * dims[3] * dims[4] * dims[5];
		}
		return dims[0] * dims[1] * dims[2] * dims[3] * dims[4] * dims[5] * dims[6] * dims[7];
	}
	inline const Variant& Variant::field(usize table_index, const Name& field) const
	{
		lutsassert();
		if (type() != EVariantType::table)
		{
			return get_npos_variant();
		}
		VariantTable* t = (VariantTable*)buffer() + table_index;
		auto iter = t->find(field);
		if (iter != t->end())
		{
			return iter->second;
		}
		return get_npos_variant();
	}
	inline Variant& Variant::field(usize table_index, const Name& field)
	{
		lutsassert();
		if (type() != EVariantType::table)
		{
			return get_npos_variant();
		}
		VariantTable* t = (VariantTable*)buffer() + table_index;
		auto iter = t->find(field);
		if (iter != t->end())
		{
			return iter->second;
		}
		return get_npos_variant();
	}
	inline Vector<Name> Variant::fields(usize table_index) const
	{
		lutsassert();
		if (type() != EVariantType::table)
		{
			return Vector<Name>();
		}
		Vector<Name> r;
		auto& t = to_var_table_buf()[table_index];
		for (auto& i : t)
		{
			r.push_back(i.first);
		}
		return r;
	}
	inline void Variant::set_field(usize table_index, const Name& field, const Variant& value)
	{
		lutsassert();
		luassert(type() == EVariantType::table);
		luassert(this != &get_npos_variant());
		VariantTable* t = (VariantTable*)buffer() + table_index;
		auto iter = t->find(field);
		if (iter != t->end())
		{
			if (value.type() == EVariantType::null)
			{
				t->erase(iter);
				return;
			}
			else
			{
				iter->second = value;
			}
		}
		else
		{
			if (value.type() != EVariantType::null)
			{
				t->insert(make_pair(field, value));
			}
		}
	}
	inline void Variant::set_field(usize table_index, const Name& field, Variant&& value)
	{
		lutsassert();
		luassert(type() == EVariantType::table);
		luassert(this != &get_npos_variant());
		VariantTable* t = (VariantTable*)buffer() + table_index;
		auto iter = t->find(field);
		if (iter != t->end())
		{
			if (value.type() == EVariantType::null)
			{
				t->erase(iter);
				return;
			}
			else
			{
				iter->second = move(value);
			}
		}
		else
		{
			if (value.type() != EVariantType::null)
			{
				t->insert(make_pair(field, move(value)));
			}
		}
	}
	inline void* Variant::buffer()
	{
		lutsassert();
		EVariantType t = type();
		if (t == EVariantType::null)
		{
			return nullptr;
		}
		if (embedded())
		{
			switch (t)
			{
			case EVariantType::boolean:
				return &m_u.m_bool;
			case EVariantType::u8:
				return &m_u.m_u8;
			case EVariantType::i8:
				return &m_u.m_i8;
			case EVariantType::u16:
				return &m_u.m_u16;
			case EVariantType::i16:
				return &m_u.m_i16;
			case EVariantType::u32:
				return &m_u.m_u32;
			case EVariantType::i32:
				return &m_u.m_i32;
			case EVariantType::f32:
				return &m_u.m_f32;
			case EVariantType::u64:
				return &m_u.m_u64;
			case EVariantType::i64:
				return &m_u.m_i64;
			case EVariantType::f64:
				return &m_u.m_f64;
			case EVariantType::name:
				return &m_u.m_name;
			case EVariantType::object:
				return &m_u.m_obj;
			default:
				lupanic();
				return nullptr;
			}
		}
		u8 dim = dimension() - 1;
		if (dim < 2)
		{
			return (void*)(((usize*)m_u.m_data) + 2);
		}
		if (dim < 4)
		{
			return (void*)(((usize*)m_u.m_data) + 4);
		}
		if (dim < 6)
		{
			return (void*)(((usize*)m_u.m_data) + 6);
		}
		return (void*)(((usize*)m_u.m_data) + 8);
	}
	inline const void* Variant::buffer() const
	{
		lutsassert();
		EVariantType t = type();
		if (t == EVariantType::null)
		{
			return nullptr;
		}
		if (embedded())
		{
			switch (t)
			{
			case EVariantType::boolean:
				return &m_u.m_bool;
			case EVariantType::u8:
				return &m_u.m_u8;
			case EVariantType::i8:
				return &m_u.m_i8;
			case EVariantType::u16:
				return &m_u.m_u16;
			case EVariantType::i16:
				return &m_u.m_i16;
			case EVariantType::u32:
				return &m_u.m_u32;
			case EVariantType::i32:
				return &m_u.m_i32;
			case EVariantType::f32:
				return &m_u.m_f32;
			case EVariantType::u64:
				return &m_u.m_u64;
			case EVariantType::i64:
				return &m_u.m_i64;
			case EVariantType::f64:
				return &m_u.m_f64;
			case EVariantType::name:
				return &m_u.m_name;
			case EVariantType::object:
				return &m_u.m_obj;
			default:
				lupanic();
				return nullptr;
			}
		}
		u8 dim = dimension() - 1;
		if (dim < 2)
		{
			return (void*)(((usize*)m_u.m_data) + 2);
		}
		if (dim < 4)
		{
			return (void*)(((usize*)m_u.m_data) + 4);
		}
		if (dim < 6)
		{
			return (void*)(((usize*)m_u.m_data) + 6);
		}
		return (void*)(((usize*)m_u.m_data) + 8);
	}
	inline usize Variant::index(usize d1) const
	{
		usize l1 = length(1);
		luassert(d1 < l1);
		return d1;
	}
	inline usize Variant::index(usize d1, usize d2) const
	{
		usize l1 = length(1);
		usize l2 = length(2);
		luassert(d1 < l1&& d2 < l2);
		return d1 + d2 * l1;
	}
	inline usize Variant::index(usize d1, usize d2, usize d3) const
	{
		usize l1 = length(1);
		usize l2 = length(2);
		usize l3 = length(3);
		luassert(d1 < l1&& d2 < l2&& d3 < l3);
		return d1 + d2 * l1 + d3 * l1 * l2;
	}
	inline usize Variant::index(usize d1, usize d2, usize d3, usize d4) const
	{
		usize l1 = length(1);
		usize l2 = length(2);
		usize l3 = length(3);
		usize l4 = length(4);
		luassert(d1 < l1&& d2 < l2&& d3 < l3&& d4 < l4);
		return d1 + d2 * l1 + d3 * l1 * l2 + d4 * l1 * l2 * l3;
	}
	inline usize Variant::index(usize d1, usize d2, usize d3, usize d4, usize d5) const
	{
		usize l1 = length(1);
		usize l2 = length(2);
		usize l3 = length(3);
		usize l4 = length(4);
		usize l5 = length(5);
		luassert(d1 < l1&& d2 < l2&& d3 < l3&& d4 < l4&& d5 < l5);
		return d1 + d2 * l1 + d3 * l1 * l2 + d4 * l1 * l2 * l3 + d5 * l1 * l2 * l3 * l4;
	}
	inline usize Variant::index(usize d1, usize d2, usize d3, usize d4, usize d5, usize d6) const
	{
		usize l1 = length(1);
		usize l2 = length(2);
		usize l3 = length(3);
		usize l4 = length(4);
		usize l5 = length(5);
		usize l6 = length(6);
		luassert(d1 < l1&& d2 < l2&& d3 < l3&& d4 < l4&& d5 < l5&& d6 < l6);
		return d1 + d2 * l1 +
			d3 * l1 * l2 +
			d4 * l1 * l2 * l3 +
			d5 * l1 * l2 * l3 * l4 +
			d6 * l1 * l2 * l3 * l4 * l5;
	}
	inline usize Variant::index(usize d1, usize d2, usize d3, usize d4, usize d5, usize d6, usize d7) const
	{
		usize l1 = length(1);
		usize l2 = length(2);
		usize l3 = length(3);
		usize l4 = length(4);
		usize l5 = length(5);
		usize l6 = length(6);
		usize l7 = length(7);
		luassert(d1 < l1&& d2 < l2&& d3 < l3&& d4 < l4&& d5 < l5&& d6 < l6&& d7 < l7);
		return d1 + d2 * l1 +
			d3 * l1 * l2 +
			d4 * l1 * l2 * l3 +
			d5 * l1 * l2 * l3 * l4 +
			d6 * l1 * l2 * l3 * l4 * l5 +
			d7 * l1 * l2 * l3 * l4 * l5 * l6;
	}
	inline usize Variant::index(usize d1, usize d2, usize d3, usize d4, usize d5, usize d6, usize d7, usize d8) const
	{
		usize l1 = length(1);
		usize l2 = length(2);
		usize l3 = length(3);
		usize l4 = length(4);
		usize l5 = length(5);
		usize l6 = length(6);
		usize l7 = length(7);
		usize l8 = length(8);
		luassert(d1 < l1&& d2 < l2&& d3 < l3&& d4 < l4&& d5 < l5&& d6 < l6&& d7 < l7&& d8 < l8);
		return d1 + d2 * l1 +
			d3 * l1 * l2 +
			d4 * l1 * l2 * l3 +
			d5 * l1 * l2 * l3 * l4 +
			d6 * l1 * l2 * l3 * l4 * l5 +
			d7 * l1 * l2 * l3 * l4 * l5 * l6 +
			d8 * l1 * l2 * l3 * l4 * l5 * l6 * l7;
	}
	inline void* Variant::to_boolean_buf()
	{
		luassert(type() == EVariantType::boolean);
		return buffer();
	}
	inline const void* Variant::to_boolean_buf() const
	{
		luassert(type() == EVariantType::boolean);
		return buffer();
	}
	inline R<void*> Variant::check_boolean_buf()
	{
		if (type() == EVariantType::boolean)
		{
			return buffer();
		}
		return BasicError::not_supported();
	}
	inline R<const void*> Variant::check_boolean_buf() const
	{
		if (type() == EVariantType::boolean)
		{
			return buffer();
		}
		return BasicError::not_supported();
	}
	inline u8* Variant::to_u8_buf()
	{
		luassert(type() == EVariantType::u8);
		return reinterpret_cast<u8*>(buffer());
	}
	inline const u8* Variant::to_u8_buf() const
	{
		luassert(type() == EVariantType::u8);
		return reinterpret_cast<const u8*>(buffer());
	}
	inline R<u8*> Variant::check_u8_buf()
	{
		if (type() == EVariantType::u8)
		{
			return reinterpret_cast<u8*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const u8*> Variant::check_u8_buf() const
	{
		if (type() == EVariantType::u8)
		{
			return reinterpret_cast<const u8*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline u8& Variant::to_u8()
	{
		return to_u8_buf()[0];
	}
	inline const u8& Variant::to_u8() const
	{
		return to_u8_buf()[0];
	}
	inline R<ReferenceWrapper<u8>> Variant::check_u8()
	{
		if (type() == EVariantType::u8)
		{
			return ReferenceWrapper<u8>(reinterpret_cast<u8*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const u8>> Variant::check_u8() const
	{
		if (type() == EVariantType::u8)
		{
			return ReferenceWrapper<const u8>(reinterpret_cast<const u8*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline i8* Variant::to_i8_buf()
	{
		luassert(type() == EVariantType::i8);
		return reinterpret_cast<i8*>(buffer());
	}
	inline const i8* Variant::to_i8_buf() const
	{
		luassert(type() == EVariantType::i8);
		return reinterpret_cast<const i8*>(buffer());
	}
	inline R<i8*> Variant::check_i8_buf()
	{
		if (type() == EVariantType::i8)
		{
			return reinterpret_cast<i8*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const i8*> Variant::check_i8_buf() const
	{
		if (type() == EVariantType::i8)
		{
			return reinterpret_cast<const i8*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline i8& Variant::to_i8()
	{
		return to_i8_buf()[0];
	}
	inline const i8& Variant::to_i8() const
	{
		return to_i8_buf()[0];
	}
	inline R<ReferenceWrapper<i8>> Variant::check_i8()
	{
		if (type() == EVariantType::i8)
		{
			return ReferenceWrapper<i8>(reinterpret_cast<i8*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const i8>> Variant::check_i8() const
	{
		if (type() == EVariantType::i8)
		{
			return ReferenceWrapper<const i8>(reinterpret_cast<const i8*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline u16* Variant::to_u16_buf()
	{
		luassert(type() == EVariantType::u16);
		return reinterpret_cast<u16*>(buffer());
	}
	inline const u16* Variant::to_u16_buf() const
	{
		luassert(type() == EVariantType::u16);
		return reinterpret_cast<const u16*>(buffer());
	}
	inline R<u16*> Variant::check_u16_buf()
	{
		if (type() == EVariantType::u16)
		{
			return reinterpret_cast<u16*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const u16*> Variant::check_u16_buf() const
	{
		if (type() == EVariantType::u16)
		{
			return reinterpret_cast<const u16*>(buffer());
		}
		return BasicError::not_supported();
	}

	inline u16& Variant::to_u16()
	{
		return to_u16_buf()[0];
	}
	inline const u16& Variant::to_u16() const
	{
		return to_u16_buf()[0];
	}
	inline R<ReferenceWrapper<u16>> Variant::check_u16()
	{
		if (type() == EVariantType::u16)
		{
			return ReferenceWrapper<u16>(reinterpret_cast<u16*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const u16>> Variant::check_u16() const
	{
		if (type() == EVariantType::u16)
		{
			return ReferenceWrapper<const u16>(reinterpret_cast<const u16*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline i16* Variant::to_i16_buf()
	{
		luassert(type() == EVariantType::i16);
		return reinterpret_cast<i16*>(buffer());
	}
	inline const i16* Variant::to_i16_buf() const
	{
		luassert(type() == EVariantType::i16);
		return reinterpret_cast<const i16*>(buffer());
	}
	inline R<i16*> Variant::check_i16_buf()
	{
		if (type() == EVariantType::i16)
		{
			return reinterpret_cast<i16*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const i16*> Variant::check_i16_buf() const
	{
		if (type() == EVariantType::i16)
		{
			return reinterpret_cast<const i16*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline i16& Variant::to_i16()
	{
		return to_i16_buf()[0];
	}
	inline const i16& Variant::to_i16() const
	{
		return to_i16_buf()[0];
	}
	inline R<ReferenceWrapper<i16>> Variant::check_i16()
	{
		if (type() == EVariantType::i16)
		{
			return ReferenceWrapper<i16>(reinterpret_cast<i16*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const i16>> Variant::check_i16() const
	{
		if (type() == EVariantType::i16)
		{
			return ReferenceWrapper<const i16>(reinterpret_cast<const i16*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline u32* Variant::to_u32_buf()
	{
		luassert(type() == EVariantType::u32);
		return reinterpret_cast<u32*>(buffer());
	}
	inline const u32* Variant::to_u32_buf() const
	{
		luassert(type() == EVariantType::u32);
		return reinterpret_cast<const u32*>(buffer());
	}
	inline R<u32*> Variant::check_u32_buf()
	{
		if (type() == EVariantType::u32)
		{
			return reinterpret_cast<u32*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const u32*> Variant::check_u32_buf() const
	{
		if (type() == EVariantType::u32)
		{
			return reinterpret_cast<const u32*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline u32& Variant::to_u32()
	{
		return to_u32_buf()[0];
	}
	inline const u32& Variant::to_u32() const
	{
		return to_u32_buf()[0];
	}
	inline R<ReferenceWrapper<u32>> Variant::check_u32()
	{
		if (type() == EVariantType::u32)
		{
			return ReferenceWrapper<u32>(reinterpret_cast<u32*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const u32>> Variant::check_u32() const
	{
		if (type() == EVariantType::u32)
		{
			return ReferenceWrapper<const u32>(reinterpret_cast<const u32*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline i32* Variant::to_i32_buf()
	{
		luassert(type() == EVariantType::i32);
		return reinterpret_cast<i32*>(buffer());
	}
	inline const i32* Variant::to_i32_buf() const
	{
		luassert(type() == EVariantType::i32);
		return reinterpret_cast<const i32*>(buffer());
	}
	inline R<i32*> Variant::check_i32_buf()
	{
		if (type() == EVariantType::i32)
		{
			return reinterpret_cast<i32*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const i32*> Variant::check_i32_buf() const
	{
		if (type() == EVariantType::i32)
		{
			return reinterpret_cast<const i32*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline i32& Variant::to_i32()
	{
		return to_i32_buf()[0];
	}
	inline const i32& Variant::to_i32() const
	{
		return to_i32_buf()[0];
	}
	inline R<ReferenceWrapper<i32>> Variant::check_i32()
	{
		if (type() == EVariantType::i32)
		{
			return R<ReferenceWrapper<i32>>::success(reinterpret_cast<i32*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const i32>> Variant::check_i32() const
	{
		if (type() == EVariantType::i32)
		{
			return R<ReferenceWrapper<const i32>>::success(reinterpret_cast<const i32*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline f32* Variant::to_f32_buf()
	{
		luassert(type() == EVariantType::f32);
		return reinterpret_cast<f32*>(buffer());
	}
	inline const f32* Variant::to_f32_buf() const
	{
		luassert(type() == EVariantType::f32);
		return reinterpret_cast<const f32*>(buffer());
	}
	inline R<f32*> Variant::check_f32_buf()
	{
		if (type() == EVariantType::f32)
		{
			return reinterpret_cast<f32*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const f32*> Variant::check_f32_buf() const
	{
		if (type() == EVariantType::f32)
		{
			return reinterpret_cast<const f32*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline f32& Variant::to_f32()
	{
		return to_f32_buf()[0];
	}
	inline const f32& Variant::to_f32() const
	{
		return to_f32_buf()[0];
	}
	inline R<ReferenceWrapper<f32>> Variant::check_f32()
	{
		if (type() == EVariantType::f32)
		{
			return ReferenceWrapper<f32>(reinterpret_cast<f32*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const f32>> Variant::check_f32() const
	{
		if (type() == EVariantType::f32)
		{
			return ReferenceWrapper<const f32>(reinterpret_cast<const f32*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline u64* Variant::to_u64_buf()
	{
		luassert(type() == EVariantType::u64);
		return reinterpret_cast<u64*>(buffer());
	}
	inline const u64* Variant::to_u64_buf() const
	{
		luassert(type() == EVariantType::u64);
		return reinterpret_cast<const u64*>(buffer());
	}
	inline R<u64*> Variant::check_u64_buf()
	{
		if (type() == EVariantType::u64)
		{
			return reinterpret_cast<u64*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const u64*> Variant::check_u64_buf() const
	{
		if (type() == EVariantType::u64)
		{
			return reinterpret_cast<const u64*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline u64& Variant::to_u64()
	{
		return to_u64_buf()[0];
	}
	inline const u64& Variant::to_u64() const
	{
		return to_u64_buf()[0];
	}
	inline R<ReferenceWrapper<u64>> Variant::check_u64()
	{
		if (type() == EVariantType::u64)
		{
			return ReferenceWrapper<u64>(reinterpret_cast<u64*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const u64>> Variant::check_u64() const
	{
		if (type() == EVariantType::u64)
		{
			return ReferenceWrapper<const u64>(reinterpret_cast<const u64*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline i64* Variant::to_i64_buf()
	{
		luassert(type() == EVariantType::i64);
		return reinterpret_cast<i64*>(buffer());
	}
	inline const i64* Variant::to_i64_buf() const
	{
		luassert(type() == EVariantType::i64);
		return reinterpret_cast<const i64*>(buffer());
	}
	inline R<i64*> Variant::check_i64_buf()
	{
		if (type() == EVariantType::i64)
		{
			return reinterpret_cast<i64*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const i64*> Variant::check_i64_buf() const
	{
		if (type() == EVariantType::i64)
		{
			return reinterpret_cast<const i64*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline i64& Variant::to_i64()
	{
		return to_i64_buf()[0];
	}
	inline const i64& Variant::to_i64() const
	{
		return to_i64_buf()[0];
	}
	inline R<ReferenceWrapper<i64>> Variant::check_i64()
	{
		if (type() == EVariantType::i64)
		{
			return ReferenceWrapper<i64>(reinterpret_cast<i64*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const i64>> Variant::check_i64() const
	{
		if (type() == EVariantType::i64)
		{
			return ReferenceWrapper<const i64>(reinterpret_cast<const i64*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline f64* Variant::to_f64_buf()
	{
		luassert(type() == EVariantType::f64);
		return reinterpret_cast<f64*>(buffer());
	}
	inline const f64* Variant::to_f64_buf() const
	{
		luassert(type() == EVariantType::f64);
		return reinterpret_cast<const f64*>(buffer());
	}
	inline R<f64*> Variant::check_f64_buf()
	{
		if (type() == EVariantType::f64)
		{
			return reinterpret_cast<f64*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const f64*> Variant::check_f64_buf() const
	{
		if (type() == EVariantType::f64)
		{
			return reinterpret_cast<const f64*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline f64& Variant::to_f64()
	{
		return to_f64_buf()[0];
	}
	inline const f64& Variant::to_f64() const
	{
		return to_f64_buf()[0];
	}
	inline R<ReferenceWrapper<f64>> Variant::check_f64()
	{
		if (type() == EVariantType::f64)
		{
			return ReferenceWrapper<f64>(reinterpret_cast<f64*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const f64>> Variant::check_f64() const
	{
		if (type() == EVariantType::f64)
		{
			return ReferenceWrapper<const f64>(reinterpret_cast<const f64*>(buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline String* Variant::to_str_buf()
	{
		luassert(type() == EVariantType::string);
		return reinterpret_cast<String*>(buffer());
	}
	inline const String* Variant::to_str_buf() const
	{
		luassert(type() == EVariantType::string);
		return reinterpret_cast<const String*>(buffer());
	}
	inline R<String*> Variant::check_str_buf()
	{
		if (type() == EVariantType::string)
		{
			return reinterpret_cast<String*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const String*> Variant::check_str_buf() const
	{
		if (type() == EVariantType::string)
		{
			return reinterpret_cast<const String*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline String& Variant::to_str()
	{
		return to_str_buf()[0];
	}
	inline const String& Variant::to_str() const
	{
		return to_str_buf()[0];
	}
	inline R<ReferenceWrapper<String>> Variant::check_str()
	{
		if (type() == EVariantType::string)
		{
			return reinterpret_cast<String*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const String>> Variant::check_str() const
	{
		if (type() == EVariantType::string)
		{
			return reinterpret_cast<const String*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
	inline Name* Variant::to_name_buf()
	{
		luassert(type() == EVariantType::name);
		return reinterpret_cast<Name*>(buffer());
	}
	inline const Name* Variant::to_name_buf() const
	{
		luassert(type() == EVariantType::name);
		return reinterpret_cast<const Name*>(buffer());
	}
	inline R<Name*> Variant::check_name_buf()
	{
		if (type() == EVariantType::name)
		{
			return reinterpret_cast<Name*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const Name*> Variant::check_name_buf() const
	{
		if (type() == EVariantType::name)
		{
			return reinterpret_cast<const Name*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline Name& Variant::to_name()
	{
		return to_name_buf()[0];
	}
	inline const Name& Variant::to_name() const
	{
		return to_name_buf()[0];
	}
	inline R<ReferenceWrapper<Name>> Variant::check_name()
	{
		if (type() == EVariantType::name)
		{
			return ReferenceWrapper<Name>((((Name*)buffer())[0]));
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const Name>> Variant::check_name() const
	{
		if (type() == EVariantType::name)
		{
			return ReferenceWrapper<const Name>(((const Name*)buffer())[0]);
		}
		return BasicError::not_supported();
	}
	inline Path* Variant::to_path_buf()
	{
		luassert(type() == EVariantType::path);
		return reinterpret_cast<Path*>(buffer());
	}
	inline const Path* Variant::to_path_buf() const
	{
		luassert(type() == EVariantType::path);
		return reinterpret_cast<const Path*>(buffer());
	}
	inline R<Path*> Variant::check_path_buf()
	{
		if (type() == EVariantType::path)
		{
			return reinterpret_cast<Path*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const Path*> Variant::check_path_buf() const
	{
		if (type() == EVariantType::path)
		{
			return reinterpret_cast<const Path*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline Path& Variant::to_path()
	{
		return to_path_buf()[0];
	}
	inline const Path& Variant::to_path() const
	{
		return to_path_buf()[0];
	}
	inline R<ReferenceWrapper<Path>> Variant::check_path()
	{
		if (type() == EVariantType::path)
		{
			return reinterpret_cast<Path*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const Path>> Variant::check_path() const
	{
		if (type() == EVariantType::path)
		{
			return reinterpret_cast<const Path*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
	inline Blob* Variant::to_blob_buf()
	{
		luassert(type() == EVariantType::blob);
		return reinterpret_cast<Blob*>(buffer());
	}
	inline const Blob* Variant::to_blob_buf() const
	{
		luassert(type() == EVariantType::blob);
		return reinterpret_cast<const Blob*>(buffer());
	}
	inline R<Blob*> Variant::check_blob_buf()
	{
		if (type() == EVariantType::blob)
		{
			return reinterpret_cast<Blob*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const Blob*> Variant::check_blob_buf() const
	{
		if (type() == EVariantType::blob)
		{
			return reinterpret_cast<const Blob*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline Blob& Variant::to_blob()
	{
		return to_blob_buf()[0];
	}
	inline const Blob& Variant::to_blob() const
	{
		return to_blob_buf()[0];
	}
	inline R<ReferenceWrapper<Blob>> Variant::check_blob()
	{
		if (type() == EVariantType::blob)
		{
			return reinterpret_cast<Blob*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const Blob>> Variant::check_blob() const
	{
		if (type() == EVariantType::blob)
		{
			return reinterpret_cast<const Blob*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
	inline P<IObject>* Variant::to_obj_buf()
	{
		luassert(type() == EVariantType::object);
		return reinterpret_cast<P<IObject>*>(buffer());
	}
	inline const P<IObject>* Variant::to_obj_buf() const
	{
		luassert(type() == EVariantType::object);
		return reinterpret_cast<const P<IObject>*>(buffer());
	}
	inline R<P<IObject>*> Variant::check_obj_buf()
	{
		if (type() == EVariantType::object)
		{
			return reinterpret_cast<P<IObject>*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const P<IObject>*> Variant::check_obj_buf() const
	{
		if (type() == EVariantType::object)
		{
			return reinterpret_cast<const P<IObject>*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline P<IObject>& Variant::to_obj()
	{
		return to_obj_buf()[0];
	}
	inline const P<IObject>& Variant::to_obj() const
	{
		return to_obj_buf()[0];
	}
	inline R<ReferenceWrapper<P<IObject>>> Variant::check_obj()
	{
		if (type() == EVariantType::object)
		{
			return reinterpret_cast<P<IObject>*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const P<IObject>>> Variant::check_obj() const
	{
		if (type() == EVariantType::object)
		{
			return reinterpret_cast<const P<IObject>*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
	inline Variant* Variant::to_var_buf()
	{
		luassert(type() == EVariantType::variant);
		return reinterpret_cast<Variant*>(buffer());
	}
	inline const Variant* Variant::to_var_buf() const
	{
		luassert(type() == EVariantType::variant);
		return reinterpret_cast<const Variant*>(buffer());
	}
	inline R<Variant*> Variant::check_var_buf()
	{
		if (type() == EVariantType::variant)
		{
			return reinterpret_cast<Variant*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const Variant*> Variant::check_var_buf() const
	{
		if (type() == EVariantType::variant)
		{
			return reinterpret_cast<const Variant*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline Variant& Variant::to_var()
	{
		return to_var_buf()[0];
	}
	inline const Variant& Variant::to_var() const
	{
		return to_var_buf()[0];
	}
	inline R<ReferenceWrapper<Variant>> Variant::check_var()
	{
		if (type() == EVariantType::variant)
		{
			return reinterpret_cast<Variant*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const Variant>> Variant::check_var() const
	{
		if (type() == EVariantType::variant)
		{
			return reinterpret_cast<const Variant*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
	inline Variant::VariantTable* Variant::to_var_table_buf()
	{
		luassert(type() == EVariantType::table);
		return reinterpret_cast<VariantTable*>(buffer());
	}
	inline const Variant::VariantTable* Variant::to_var_table_buf() const
	{
		luassert(type() == EVariantType::table);
		return reinterpret_cast<const VariantTable*>(buffer());
	}
	inline R<Variant::VariantTable*> Variant::check_var_table_buf()
	{
		if (type() == EVariantType::table)
		{
			return reinterpret_cast<VariantTable*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline R<const Variant::VariantTable*> Variant::check_var_table_buf() const
	{
		if (type() == EVariantType::table)
		{
			return reinterpret_cast<const VariantTable*>(buffer());
		}
		return BasicError::not_supported();
	}
	inline Variant::VariantTable& Variant::to_var_table()
	{
		return to_var_table_buf()[0];
	}
	inline const Variant::VariantTable& Variant::to_var_table() const
	{
		return to_var_table_buf()[0];
	}
	inline R<ReferenceWrapper<Variant::VariantTable>> Variant::check_var_table()
	{
		if (type() == EVariantType::table)
		{
			return reinterpret_cast<VariantTable*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
	inline R<ReferenceWrapper<const Variant::VariantTable>> Variant::check_var_table() const
	{
		if (type() == EVariantType::table)
		{
			return reinterpret_cast<const VariantTable*>(buffer())[0];
		}
		return BasicError::not_supported();
	}
}