// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Varient.cpp
* @author JXMaster
* @date 2020/1/16
*/
#include "Variant.hpp"

namespace luna
{
	Variant::~Variant()
	{
		lutsassert();
		EVariantType t = type();
		switch (t)
		{
		case EVariantType::string:
		{
			P<IString>* s = str_buf();
			size_t sz = size();
			for (size_t i = 0; i < sz; ++i)
			{
				s[i].clear();
			}
		}
		break;
		case EVariantType::name:
		{
			P<IName>* s = name_buf();
			size_t sz = size();
			for (size_t i = 0; i < sz; ++i)
			{
				s[i].clear();
			}
		}
		break;
		case EVariantType::path:
		{
			P<IPath>* s = path_buf();
			size_t sz = size();
			for (size_t i = 0; i < sz; ++i)
			{
				s[i].clear();
			}
		}
		break;
		case EVariantType::blob:
		{
			P<IBlob>* s = blob_buf();
			size_t sz = size();
			for (size_t i = 0; i < sz; ++i)
			{
				s[i].clear();
			}
		}
		break;
		case EVariantType::object:
		{
			P<IObject>* s = obj_buf();
			size_t sz = size();
			for (size_t i = 0; i < sz; ++i)
			{
				s[i].clear();
			}
		}
		break;
		case EVariantType::variant:
		{
			P<IVariant>* s = var_buf();
			size_t sz = size();
			for (size_t i = 0; i < sz; ++i)
			{
				s[i].clear();
			}
		}
		break;
		case EVariantType::table:
		{
			table_t* vec = (table_t*)buffer();
			size_t sz = size();
			for (size_t i = 0; i < sz; ++i)
			{
				(vec + i)->~table_t();
			}
		}
		break;
		default:
			break;
		}
		if (!embedded())
		{
			m_rc.m_alloc->free(m_u.m_data);
			m_u.m_data = nullptr;
		}
		m_flag = 0;
	}

	static size_t* alloc_buf(IAllocator* alloc, size_t header_sz, EVariantType t, size_t count)
	{
		luassert(t != EVariantType::null);
		size_t sz;
		if (t == EVariantType::boolean)
		{
			sz = header_sz + align_upper(count, 8) / 8;
		}
		else
		{
			sz = header_sz + Variant::elem_size(t) * count;
		}
		size_t* buf = (size_t*)alloc->allocate(sz);
		if (!buf)
		{
			return nullptr;
		}
		if ((uint8)t >= (uint8)EVariantType::string && (uint8)t <= (uint8)EVariantType::variant)
		{
			memzero(buf, sz);
		}
		if (t == EVariantType::table)
		{
			Variant::table_t* table_begin = (Variant::table_t*)((size_t)buf + header_sz);
			for (size_t i = 0; i < count; ++i)
			{
				new (table_begin + i) Variant::table_t(alloc);
			}
		}
		return buf;
	}

	void* Variant::init_1d(EVariantType t, size_t d1)
	{
		constexpr size_t header_sz = sizeof(size_t) * 2;
		size_t count = d1;
		size_t* buf = alloc_buf(m_rc.m_alloc, header_sz, t, count);
		if (!buf)
		{
			return nullptr;
		}
		buf[0] = d1;
		buf[1] = 1;
		return buf;
	}

	void* Variant::init_2d(EVariantType t, size_t d1, size_t d2)
	{
		constexpr size_t header_sz = sizeof(size_t) * 2;
		size_t count = d1 * d2;
		size_t* buf = alloc_buf(m_rc.m_alloc, header_sz, t, count);
		if (!buf)
		{
			return nullptr;
		}
		buf[0] = d1;
		buf[1] = d2;
		return buf;
	}

	void* Variant::init_3d(EVariantType t, size_t d1, size_t d2, size_t d3)
	{
		constexpr size_t header_sz = sizeof(size_t) * 4;
		size_t count = d1 * d2 * d3;
		size_t* buf = alloc_buf(m_rc.m_alloc, header_sz, t, count);
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

	void* Variant::init_4d(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4)
	{
		constexpr size_t header_sz = sizeof(size_t) * 4;
		size_t count = d1 * d2 * d3 * d4;
		size_t* buf = alloc_buf(m_rc.m_alloc, header_sz, t, count);
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

	void* Variant::init_5d(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5)
	{
		constexpr size_t header_sz = sizeof(size_t) * 6;
		size_t count = d1 * d2 * d3 * d4 * d5;
		size_t* buf = alloc_buf(m_rc.m_alloc, header_sz, t, count);
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

	void* Variant::init_6d(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6)
	{
		constexpr size_t header_sz = sizeof(size_t) * 6;
		size_t count = d1 * d2 * d3 * d4 * d5 * d6;
		size_t* buf = alloc_buf(m_rc.m_alloc, header_sz, t, count);
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

	void* Variant::init_7d(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7)
	{
		constexpr size_t header_sz = sizeof(size_t) * 8;
		size_t count = d1 * d2 * d3 * d4 * d5 * d6 * d7;
		size_t* buf = alloc_buf(m_rc.m_alloc, header_sz, t, count);
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

	void* Variant::init_8d(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7, size_t d8)
	{
		constexpr size_t header_sz = sizeof(size_t) * 8;
		size_t count = d1 * d2 * d3 * d4 * d5 * d6 * d7 * d8;
		size_t* buf = alloc_buf(m_rc.m_alloc, header_sz, t, count);
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

	//! Transfers the data in this variant to a new variant.
	Variant* Variant::transfer_data()
	{
		Variant* v = m_rc.m_alloc->new_obj<Variant>(m_rc.m_alloc);
		if (!v)
		{
			return nullptr;
		}
		// Transfer data.
		if (embedded())
		{
			switch (type())
			{
			case EVariantType::boolean:
				v->m_u.m_bool = m_u.m_bool;
				break;
			case EVariantType::u8:
				v->m_u.m_u8 = m_u.m_u8;
				break;
			case EVariantType::i8:
				v->m_u.m_i8 = m_u.m_i8;
				break;
			case EVariantType::u16:
				v->m_u.m_u16 = m_u.m_u16;
				break;
			case EVariantType::i16:
				v->m_u.m_i16 = m_u.m_i16;
				break;
			case EVariantType::u32:
				v->m_u.m_u32 = m_u.m_u32;
				break;
			case EVariantType::i32:
				v->m_u.m_i32 = m_u.m_i32;
				break;
			case EVariantType::f32:
				v->m_u.m_f32 = m_u.m_f32;
				break;
			case EVariantType::u64:
				v->m_u.m_u64 = m_u.m_u64;
				break;
			case EVariantType::i64:
				v->m_u.m_i64 = m_u.m_i64;
				break;
			case EVariantType::f64:
				v->m_u.m_f64 = m_u.m_f64;
				break;
			case EVariantType::string:
				v->m_u.m_str = m_u.m_str;
				break;
			case EVariantType::name:
				v->m_u.m_name = m_u.m_name;
				break;
			case EVariantType::path:
				v->m_u.m_path = m_u.m_path;
				break;
			case EVariantType::blob:
				v->m_u.m_blob = m_u.m_blob;
				break;
			case EVariantType::object:
				v->m_u.m_obj = m_u.m_obj;
				break;
			case EVariantType::variant:
				v->m_u.m_var = m_u.m_var;
				break;
			default:
				lupanic();
			}
		}
		else
		{
			v->m_u.m_data = m_u.m_data;
		}
		v->m_flag = m_flag;
		v->m_flag2 = m_flag2;
		return v;
	}

	P<IVariant> Variant::reset(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7, size_t d8)
	{
		lutsassert();
		uint8 dim = 8;
		if (t == EVariantType::null)
		{
			// Null type always get dimension 1.
			dim = 1;
			d1 = 1;
		}
		else
		{
			// Determines the dimension
			if (d8 == 1)
			{
				--dim;
				if (d7 == 1)
				{
					--dim;
					if (d6 == 1)
					{
						--dim;
						if (d5 == 1)
						{
							--dim;
							if (d4 == 1)
							{
								--dim;
								if (d3 == 1)
								{
									--dim;
									if (d2 == 1)
									{
										--dim;
									}
								}
							}
						}
					}
				}
			}
		}

		// Check embedded case.
		if (dim == 1 && d1 == 1)
		{
			Variant* v;
			if (t == EVariantType::table)
			{
				void* new_data = init_1d(EVariantType::table, 1);
				v = transfer_data();
				set_embedded(false);
				m_u.m_data = new_data;
			}
			else
			{
				v = transfer_data();
				memzero(&m_u, sizeof(DataUnion));
				set_embedded(true);
			}
			set_dimension(1);
			set_type(t);
			return box_ptr<IVariant>(v);
		}
		else
		{
			void* new_data;
			switch (dim)
			{
			case 1:
				new_data = init_1d(t, d1);
				break;
			case 2:
				new_data = init_2d(t, d1, d2);
				break;
			case 3:
				new_data = init_3d(t, d1, d2, d3);
				break;
			case 4:
				new_data = init_4d(t, d1, d2, d3, d4);
				break;
			case 5:
				new_data = init_5d(t, d1, d2, d3, d4, d5);
				break;
			case 6:
				new_data = init_6d(t, d1, d2, d3, d4, d5, d6);
				break;
			case 7:
				new_data = init_7d(t, d1, d2, d3, d4, d5, d6, d7);
				break;
			case 8:
				new_data = init_8d(t, d1, d2, d3, d4, d5, d6, d7, d8);
				break;
			default:
				lupanic();
			}
			Variant* v = transfer_data();
			set_dimension(dim);
			set_type(t);
			set_embedded(false);
			return box_ptr<IVariant>(v);
		}
	}

	R<Vector<P<IName>>> Variant::fields(size_t table_index)
	{
		lutsassert();
		if (type() != EVariantType::table)
		{
			return e_not_available;
		}
		table_t* t = (table_t*)buffer() + table_index;
		size_t sz = t->size();

		Vector<P<IName>> fields(m_rc.m_alloc);
		fields.reserve(sz);
		for (size_t i = 0; i < sz; ++i)
		{
			fields.push_back(t->at(i).first);
		}
		return fields;
	}

	R<IVariant*> Variant::field(size_t table_index, IName* field)
	{
		lutsassert();
		luassert_usr(field);
		if (type() != EVariantType::table)
		{
			return e_not_available;
		}
		table_t* t = (table_t*)buffer() + table_index;
		for (auto iter = t->begin(); iter != t->end(); ++iter)
		{
			if (iter->first.get() == field)
			{
				return iter->second.get();
			}
		}
		set_err(e_item_not_exist, "IVariant::field - Field %s on table %u cannot be found", field->c_str(), (uint32)table_index);
		return e_user_failure;
	}

	void Variant::set_field(size_t table_index, IName* field, IVariant* value)
	{
		lutsassert();
		luassert_usr(field);
		luassert_usr(type() == EVariantType::table);
		table_t* t = (table_t*)buffer() + table_index;
		for (auto iter = t->begin(); iter != t->end(); ++iter)
		{
			if (iter->first.get() == field)
			{
				if (value)
				{
					iter->second = value;
					return;
				}
				t->erase(iter);
				return;
			}
		}
		if (value)
		{
			t->push_back(make_pair(P<IName>(field), P<IVariant>(value)));
		}
		return;
	}
}