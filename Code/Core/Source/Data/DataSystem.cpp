// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file DataSystem.cpp
* @author JXMaster
* @date 2020/1/1
*/
#include "DataSystem.hpp"
#include "Variant.hpp"
#include "TextEncoder.hpp"
#include "TextDecoder.hpp"
namespace luna
{
	namespace data
	{
		P<IVariant> new_var(EVariantType t)
		{
			Variant* v = new_obj<Variant>(get_global_heap());
			if (t == EVariantType::table)
			{
				void* new_data = v->init_1d(EVariantType::table, 1);
				v->set_embedded(false);
				v->m_u.m_data = new_data;
			}
			v->set_type(t);
			return box_ptr<IVariant>(v);
		}
		P<IVariant> new_var1(EVariantType t, size_t d1)
		{
			if (d1 == 1)
			{
				return data::new_var(t);
			}
			luassert_usr(t != EVariantType::null);
			Variant* v = new_obj<Variant>(get_global_heap());
			void* new_data = v->init_1d(t, d1);
			v->m_u.m_data = new_data;
			v->set_type(t);
			v->set_dimension(1);
			v->set_embedded(false);
			return box_ptr<IVariant>(v);
		}
		P<IVariant> new_var2(EVariantType t, size_t d1, size_t d2)
		{
			Variant* v = new_obj<Variant>(get_global_heap());
			luassert_usr(t != EVariantType::null);
			void* new_data = v->init_2d(t, d1, d2);
			v->m_u.m_data = new_data;
			v->set_type(t);
			v->set_dimension(2);
			v->set_embedded(false);
			return box_ptr<IVariant>(v);
		}
		P<IVariant> new_var3(EVariantType t, size_t d1, size_t d2, size_t d3)
		{
			Variant* v = new_obj<Variant>(get_global_heap());
			luassert_usr(t != EVariantType::null);
			void* new_data = v->init_3d(t, d1, d2, d3);
			v->m_u.m_data = new_data;
			v->set_type(t);
			v->set_dimension(3);
			v->set_embedded(false);
			return box_ptr<IVariant>(v);
		}
		P<IVariant> new_var4(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4)
		{
			Variant* v = new_obj<Variant>(get_global_heap());
			luassert_usr(t != EVariantType::null);
			void* new_data = v->init_4d(t, d1, d2, d3, d4);
			v->m_u.m_data = new_data;
			v->set_type(t);
			v->set_dimension(4);
			v->set_embedded(false);
			return box_ptr<IVariant>(v);
		}
		P<IVariant> new_var5(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5)
		{
			Variant* v = new_obj<Variant>(get_global_heap());
			luassert_usr(t != EVariantType::null);
			void* new_data = v->init_5d(t, d1, d2, d3, d4, d5);
			v->m_u.m_data = new_data;
			v->set_type(t);
			v->set_dimension(5);
			v->set_embedded(false);
			return box_ptr<IVariant>(v);
		}
		P<IVariant> new_var6(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6)
		{
			Variant* v = new_obj<Variant>(get_global_heap());
			luassert_usr(t != EVariantType::null);
			void* new_data = v->init_6d(t, d1, d2, d3, d4, d5, d6);
			v->m_u.m_data = new_data;
			v->set_type(t);
			v->set_dimension(6);
			v->set_embedded(false);
			return box_ptr<IVariant>(v);
		}
		P<IVariant> new_var7(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7)
		{
			Variant* v = new_obj<Variant>(get_global_heap());
			luassert_usr(t != EVariantType::null);
			void* new_data = v->init_7d(t, d1, d2, d3, d4, d5, d6, d7);
			v->m_u.m_data = new_data;
			v->set_type(t);
			v->set_dimension(7);
			v->set_embedded(false);
			return box_ptr<IVariant>(v);
		}
		P<IVariant> new_var8(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7, size_t d8)
		{
			Variant* v = new_obj<Variant>(get_global_heap());
			luassert_usr(t != EVariantType::null);
			void* new_data = v->init_8d(t, d1, d2, d3, d4, d5, d6, d7, d8);
			v->m_u.m_data = new_data;
			v->set_type(t);
			v->set_dimension(8);
			v->set_embedded(false);
			return box_ptr<IVariant>(v);
		}
		P<IEncoder> new_text_encoder()
		{
			TextEncoder* encoder = new_obj<TextEncoder>();
			return box_ptr<IEncoder>(encoder);
		}
		P<IDecoder> new_text_decoder()
		{
			TextDecoder* encoder = new_obj<TextDecoder>();
			return box_ptr<IDecoder>(encoder);
		}
	}
}