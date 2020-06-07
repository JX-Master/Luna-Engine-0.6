// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextEncoder.cpp
* @author JXMaster
* @date 2020/1/27
*/
#include "TextEncoder.hpp"

namespace luna
{
	RV TextEncoder::encode(IVariant* variant, IStream* stream)
	{
		luassert(variant && stream);
		lutsassert();

		if (variant->type() == EVariantType::object)
		{
			return e_bad_arguments;
		}

		m_buf = stream;

		lutry
		{
			m_indent = 0;
			luexp(encode_variant_object(variant));
			return s_ok;
		}
		lucatch
		{
			m_buf = nullptr;
			return lures;
		}
		return s_ok;
	}

	result_t TextEncoder::encode_variant_object(IVariant * t)
	{
		lutry
		{
			if (t->type() == EVariantType::null)
			{
				luexp(append("null"));
			}
			else
			{
				luexp(encode_not_null_type(t->type()));
				// encode dimension.
				if (t->type() == EVariantType::variant || t->dimension() != 1 || t->length(1) != 1)
				{
					uint8 dim = t->dimension();
					char numbuf[32];
					for (uint8 i = dim; i > 0; --i)
					{
						luexp(push_back('['));
						sprintf_s(numbuf, "%llu", (uint64)t->length(i));
						luexp(append(numbuf));
						luexp(push_back(']'));
					}
				}

				luexp(append(" = "));
				// encode value.
				luexp(encode_variant_object_value(t));
			}
		}
		lucatchret;
		return s_ok;
	}

	result_t TextEncoder::encode_not_null_type(EVariantType t)
	{
		lutry
		{
			switch (t)
			{
			case EVariantType::boolean:
				luexp(append("bool"));
				break;
			case EVariantType::u8:
				luexp(append("u8"));
				break;
			case EVariantType::i8:
				luexp(append("i8"));
				break;
			case EVariantType::u16:
				luexp(append("u16"));
				break;
			case EVariantType::i16:
				luexp(append("i16"));
				break;
			case EVariantType::u32:
				luexp(append("u32"));
				break;
			case EVariantType::i32:
				luexp(append("i32"));
				break;
			case EVariantType::f32:
				luexp(append("f32"));
				break;
			case EVariantType::u64:
				luexp(append("u64"));
				break;
			case EVariantType::i64:
				luexp(append("i64"));
				break;
			case EVariantType::f64:
				luexp(append("f64"));
				break;
			case EVariantType::string:
				luexp(append("string"));
				break;
			case EVariantType::name:
				luexp(append("label"));
				break;
			case EVariantType::path:
				luexp(append("path"));
				break;
			case EVariantType::blob:
				luexp(append("blob"));
				break;
			case EVariantType::variant:
				luexp(append("var"));
				break;
			case EVariantType::table:
				luexp(append("table"));
				break;
			default:
				break;
			}
		}
		lucatchret;
		return s_ok;
	}
	result_t TextEncoder::encode_variant_object_value(IVariant* v)
	{
		lutry
		{
			if (v->dimension() == 1 && v->length(1) == 1 && v->type() != EVariantType::variant)
			{
				luexp(encode_single_data_value(v, 0));
			}
			else
			{
				bool need_indent = (v->type() == EVariantType::table || v->type() == EVariantType::variant);
				luexp(append("{ "));
				if (need_indent)
				{
					++m_indent;
					luexp(push_back('\n'));
				}
				for (size_t i = 0; i < v->size(); ++i)
				{
					if (need_indent)
					{
						luexp(push_tabs());
					}
					luexp(encode_single_data_value(v, i));
					if (i != v->size() - 1)
					{
						luexp(append(", "));
						if (need_indent)
						{
							luexp(push_back('\n'));
						}
					}
				}
				if (need_indent)
				{
					--m_indent;
					luexp(push_back('\n'));
					luexp(push_tabs());
					luexp(push_back('}'));
				}
				else
				{
					luexp(append(" }"));
				}
			}
		}
		lucatchret;
		return s_ok;	
	}

	result_t TextEncoder::encode_single_data_value(IVariant* t, size_t i)
	{
		lutry
		{
			switch (t->type())
			{
			case EVariantType::boolean:
				luexp(append(bit_test(t->boolean_buf(), i) ? "true" : "false"));
				break;
			case EVariantType::u8:
			case EVariantType::i8:
			case EVariantType::u16:
			case EVariantType::i16:
			case EVariantType::u32:
			case EVariantType::i32:
			case EVariantType::u64:
			case EVariantType::i64:
				luexp(encode_integer_value(t, i));
				break;
			case EVariantType::f32:
			case EVariantType::f64:
				luexp(encode_float_value(t, i));
				break;
			case EVariantType::string:
			case EVariantType::name:
			case EVariantType::path:
				luexp(encode_string_value(t, i));
				break;
			case EVariantType::blob:
				luexp(encode_blob_value(t, i));
				break;
			case EVariantType::variant:
				luexp(encode_variant_value(t, i));
				break;
			case EVariantType::table:
				luexp(encode_table_value(t, i));
				break;
			default:
				break;
			}
		}
		lucatchret;
		return s_ok;
	}

	result_t TextEncoder::encode_integer_value(IVariant* t, size_t i)
	{
		lutry
		{
			char buf[32];
			switch (t->type())
			{
			case EVariantType::u8:
				sprintf_s(buf, "%u", (uint32)t->u8_buf()[i]);
				break;
			case EVariantType::i8:
				sprintf_s(buf, "%d", (int32)t->i8_buf()[i]);
				break;
			case EVariantType::u16:
				sprintf_s(buf, "%u", (uint32)t->u16_buf()[i]);
				break;
			case EVariantType::i16:
				sprintf_s(buf, "%d", (int32)t->i16_buf()[i]);
				break;
			case EVariantType::u32:
				sprintf_s(buf, "%u", t->u32_buf()[i]);
				break;
			case EVariantType::i32:
				sprintf_s(buf, "%d", t->i32_buf()[i]);
				break;
			case EVariantType::u64:
				sprintf_s(buf, "%llu", t->u64_buf()[i]);
				break;
			case EVariantType::i64:
				sprintf_s(buf, "%lld", t->i64_buf()[i]);
				break;
			default:
				lupanic();
				return e_failure;
			}
			luexp(append(buf));
		}
		lucatchret;
		return s_ok;
	}

	result_t TextEncoder::encode_float_value(IVariant* t, size_t i)
	{
		lutry
		{
			char buf[64];
			switch (t->type())
			{
			case EVariantType::f32:
				sprintf_s(buf, "%f", t->f32_buf()[i]);
				break;
			case EVariantType::f64:
				sprintf_s(buf, "%f", t->f64_buf()[i]);
				break;
			default:
				lupanic();
				return e_failure;
			}
			luexp(append(buf));
		}
		lucatchret;
		return s_ok;
	}

	result_t TextEncoder::encode_string_value(IVariant* t, size_t i)
	{
		lutry
		{
			P<IString> str;
			switch (t->type())
			{
			case EVariantType::string:
				str = t->str_buf()[i];
				break;
			case EVariantType::name:
				str = t->name_buf()[i];
				break;
			case EVariantType::path:
				str = t->path_buf()[i]->encode();
				break;
			default:
				lupanic();
				break;
			}
			luexp(push_back('"'));
			luexp(encode_string_literal(str));
			luexp(push_back('"'));
		}
		lucatchret;
		return s_ok;
	}

	result_t TextEncoder::encode_blob_value(IVariant* t, size_t i)
	{
		lutry
		{
			P<IBlob> blob = t->blob_buf()[i];
			auto buf = base64_encode_buffered(blob->data(), blob->size());
			luexp(append("\"base64??"));
			luexp(append("["));
			char count[64];
			sprintf_s(count, "%llu", (uint64)buf->size());
			luexp(append(count));
			luexp(append("]"));
			luexp(append(buf->c_str()));
			luexp(push_back('"'));
		}
		lucatchret;
		return s_ok;
	}

	result_t TextEncoder::encode_table_value(IVariant* t, size_t i)
	{
		lutry
		{
			lulet(fields, t->fields(i));

			if (fields.empty())
			{
				append("{}");
				return s_ok;
			}
			luexp(append("{ \n"));
			++m_indent;

			size_t num_fields = fields.size();
			for (size_t j = 0; j < num_fields; ++j)
			{
				auto entry_name = fields[j];
				lulet(entry, t->field(i, entry_name));
				if (entry->type() == EVariantType::object)
				{
					continue;
				}
				luexp(push_tabs());
				luexp(encode_table_entry(entry_name, entry));
				if (j != num_fields - 1)
				{
					luexp(append(",\n"));
				}
			}

			--m_indent;
			luexp(push_back('\n'));
			luexp(push_tabs());
			luexp(push_back('}'));
		}
		lucatchret;
		return s_ok;
	}

	result_t TextEncoder::encode_table_entry(IName* data_entry_name, IVariant* variant)
	{
		lutry
		{
			luexp(encode_identifier(data_entry_name));
			luexp(append(":"));
			luexp(encode_variant_object(variant));
		}
		lucatchret;
		return s_ok;
	}

	result_t TextEncoder::encode_variant_value(IVariant* t, size_t i)
	{
		lutry
		{
			auto var = t->var_buf()[i];
			luexp(encode_variant_object(var));
		}
		lucatchret;
		return s_ok;
	}
}