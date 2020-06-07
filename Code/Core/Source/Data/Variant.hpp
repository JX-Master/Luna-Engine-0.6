// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Varient.hpp
* @author JXMaster
* @date 2020/1/16
*/
#pragma once
#include "../CoreHeader.hpp"
#include <Base/Vector.hpp>
#include <Base/TSAssert.hpp>
namespace luna
{
	enum class EVariantFlag : uint8
	{
		none = 0,
		// The data is placed in the variant directly.
		embedded = 0x01,
	};

	class Variant final : public IVariant
	{
	public:
		lucid("{b4307cb5-27c6-4578-a593-b6c9e53feb4e}");
		luiimpl(Variant, IVariant, IObject);

		using table_t = Vector<Pair<P<IName>, P<IVariant>>>;

		// Low 3 bits: dimension. High 5 bits: type.
		uint8 m_flag;
		EVariantFlag m_flag2;

		// To reduce allocation, if there is only one element in the variant, we
		// embedded the element directly in the variant. This may happen only if:
		// 1. The variant is created with `new_var`. or
		// 2. The variant is created with `new_var1`, with d1 == 1.
		// The 2 cases above is actually the same.
		//
		// This part takes 8 bytes for most.
		union DataUnion
		{
			// Embedded part.
			uint8 m_bool;
			uint8 m_u8;
			int8 m_i8;
			uint16 m_u16;
			int16 m_i16;
			uint32 m_u32;
			int32 m_i32;
			float32 m_f32;
			uint64 m_u64;
			int64 m_i64;
			float64 m_f64;

			IString* m_str;
			IName* m_name;
			IPath* m_path;
			IBlob* m_blob;
			IObject* m_obj;
			IVariant* m_var;

			// The data block layout:
			// The first `n` size_t value stores the length for each dimension.
			// `n` is:
			// * 2 for 1-D and 2-D
			// * 4 for 3-D and 4-D
			// * 6 for 5-D and 6-D
			// * 8 for 7-D and 8-D
			// (The unused dimension always filled with value 1).
			//
			// Then is the buffer for real data.
			void* m_data;
		};

		DataUnion m_u;

		lutsassert_lock();

		Variant(IAllocator* alloc) :
			luibind(alloc),
			m_flag(0),
			m_flag2(EVariantFlag::none)
		{
			set_type(EVariantType::null);
			set_dimension(1);
			set_embedded(true);
			memzero(&m_u, sizeof(DataUnion));
		}

		~Variant();

		virtual EVariantType type() override
		{
			return (EVariantType)((m_flag & 0xF8) >> 3);
		}
		void set_type(EVariantType t)
		{
			m_flag = (m_flag & 0x07) + ((uint8)t << 3);
		}
		virtual uint8 dimension() override
		{
			return (m_flag & 0x07) + 1;
		}
		void set_dimension(uint8 dim)
		{
			--dim;
			m_flag = (m_flag & 0xF8) + (dim & 0x07);
		}
		//! If `true`, then the data is embedded, else the data is stored in 
		//! dedicated buffer.
		//! 
		//! Only one elements with 1-D dimension can be embedded.
		bool embedded()
		{
			return ((m_flag2 & EVariantFlag::embedded) != EVariantFlag::none);
		}

		void set_embedded(bool e)
		{
			if (e)
			{
				m_flag2 |= EVariantFlag::embedded;
			}
			else
			{
				m_flag2 &= (EVariantFlag)(~(uint8)(EVariantFlag::embedded));
			}
		}

		virtual size_t length(uint8 dim) override
		{
			if (type() == EVariantType::null)
			{
				return 0;
			}
			if (embedded() || dim > dimension())
			{
				return 1;
			}
			return ((size_t*)(m_u.m_data))[dim - 1];
		}
		static size_t elem_size(EVariantType ty)
		{
			uint8 t = (uint8)ty;
			if (t == (uint8)EVariantType::null)
			{
				return 0;
			}
			if (t <= (uint8)EVariantType::i8)
			{
				return 1;
			}
			if (t <= (uint8)EVariantType::i16)
			{
				return 2;
			}
			if (t <= (uint8)EVariantType::f32)
			{
				return 4;
			}
			if (t <= (uint8)EVariantType::f64)
			{
				return 8;
			}
			if (t == (uint8)EVariantType::table)
			{
				return sizeof(table_t);
			}
			return sizeof(size_t);
		}
		virtual size_t buffer_size() override
		{
			lutsassert();
			EVariantType t = type();
			if (embedded())
			{
				return elem_size(t);
			}
			uint8 dim = dimension() - 1;
			size_t* dims = (size_t*)(m_u.m_data);
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
				size_t elem_sz = elem_size(t);
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
		virtual size_t size() override
		{
			lutsassert();
			if (embedded())
			{
				return 1;
			}
			uint8 dim = dimension() - 1;
			size_t* dims = (size_t*)(m_u.m_data);
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
		virtual void* buffer() override
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
				case EVariantType::string:
					return &m_u.m_str;
				case EVariantType::name:
					return &m_u.m_name;
				case EVariantType::path:
					return &m_u.m_path;
				case EVariantType::blob:
					return &m_u.m_blob;
				case EVariantType::object:
					return &m_u.m_obj;
				case EVariantType::variant:
					return &m_u.m_var;
				default:
					lupanic();
					return nullptr;
				}
			}
			uint8 dim = dimension() - 1;
			if (dim < 2)
			{
				return (void*)(((size_t*)m_u.m_data) + 2);
			}
			if (dim < 4)
			{
				return (void*)(((size_t*)m_u.m_data) + 4);
			}
			if (dim < 6)
			{
				return (void*)(((size_t*)m_u.m_data) + 6);
			}
			return (void*)(((size_t*)m_u.m_data) + 8);
		}

		//! Creates an initializes a new data memory with 1D-8D.
		void* init_1d(EVariantType t, size_t d1);
		void* init_2d(EVariantType t, size_t d1, size_t d2);
		void* init_3d(EVariantType t, size_t d1, size_t d2, size_t d3);
		void* init_4d(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4);
		void* init_5d(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5);
		void* init_6d(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6);
		void* init_7d(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7);
		void* init_8d(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7, size_t d8);

		//! Creates a new variant, transfers the data to that 
		//! variant, and sets `new_data` to this variant.
		Variant* transfer_data();

		virtual P<IVariant> reset(EVariantType t, size_t d1 = 1, size_t d2 = 1, size_t d3 = 1, size_t d4 = 1,
			size_t d5 = 1, size_t d6 = 1, size_t d7 = 1, size_t d8 = 1) override;

		virtual R<Vector<P<IName>>> fields(size_t table_index) override;
		virtual R<IVariant*> field(size_t table_index, IName* field) override;
		virtual void set_field(size_t table_index, IName* field, IVariant* value) override;
	};
}