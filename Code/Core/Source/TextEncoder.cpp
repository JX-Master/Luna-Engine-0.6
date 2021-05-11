// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextEncoder.cpp
* @author JXMaster
* @date 2020/1/27
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT
#include "TextEncoder.hpp"
#include <Runtime/Base64.hpp>
#include "../Core.hpp"

namespace Luna
{
	RV TextEncoder::encode(const Variant& variant, IStream* stream)
	{
		luassert(stream);
		lutsassert();

		if (variant.type() == EVariantType::object)
		{
			return BasicError::bad_arguments();
		}

		m_buf = stream;

		lutry
		{
			m_indent = 0;
			luexp(encode_variant_object(variant));
			return RV();
		}
		lucatch
		{
			m_buf = nullptr;
			return lures;
		}
		return RV();
	}

	RV TextEncoder::encode_variant_object(const Variant& t)
	{
		lutry
		{
			if (t.type() == EVariantType::null)
			{
				luexp(append("null"));
			}
			else
			{
				luexp(encode_not_null_type(t.type()));
				// encode dimension.
				if (t.type() == EVariantType::variant || t.dimension() != 1 || t.length(1) != 1)
				{
					u8 dim = t.dimension();
					char numbuf[32];
					for (u8 i = dim; i > 0; --i)
					{
						luexp(push_back('['));
						sprintf_s(numbuf, "%llu", (u64)t.length(i));
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
		return RV();
	}

	RV TextEncoder::encode_not_null_type(EVariantType t)
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
		return RV();
	}
	RV TextEncoder::encode_variant_object_value(const Variant& v)
	{
		lutry
		{
			if (v.dimension() == 1 && v.length(1) == 1 && v.type() != EVariantType::variant)
			{
				luexp(encode_single_data_value(v, 0));
			}
			else
			{
				bool need_indent = (v.type() == EVariantType::table || v.type() == EVariantType::variant);
				luexp(append("{ "));
				if (need_indent)
				{
					++m_indent;
					luexp(push_back('\n'));
				}
				for (usize i = 0; i < v.size(); ++i)
				{
					if (need_indent)
					{
						luexp(push_tabs());
					}
					luexp(encode_single_data_value(v, i));
					if (i != v.size() - 1)
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
		return RV();	
	}

	RV TextEncoder::encode_single_data_value(const Variant& t, usize i)
	{
		lutry
		{
			switch (t.type())
			{
			case EVariantType::boolean:
				luexp(append(bit_test(t.to_boolean_buf(), i) ? "true" : "false"));
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
		return RV();
	}

	RV TextEncoder::encode_integer_value(const Variant& t, usize i)
	{
		lutry
		{
			char buf[32];
			switch (t.type())
			{
			case EVariantType::u8:
				sprintf_s(buf, "%u", (u32)t.to_u8_buf()[i]);
				break;
			case EVariantType::i8:
				sprintf_s(buf, "%d", (i32)t.to_i8_buf()[i]);
				break;
			case EVariantType::u16:
				sprintf_s(buf, "%u", (u32)t.to_u16_buf()[i]);
				break;
			case EVariantType::i16:
				sprintf_s(buf, "%d", (i32)t.to_i16_buf()[i]);
				break;
			case EVariantType::u32:
				sprintf_s(buf, "%u", t.to_u32_buf()[i]);
				break;
			case EVariantType::i32:
				sprintf_s(buf, "%d", t.to_i32_buf()[i]);
				break;
			case EVariantType::u64:
				sprintf_s(buf, "%llu", t.to_u64_buf()[i]);
				break;
			case EVariantType::i64:
				sprintf_s(buf, "%lld", t.to_i64_buf()[i]);
				break;
			default:
				lupanic();
				return BasicError::failure();
			}
			luexp(append(buf));
		}
		lucatchret;
		return RV();
	}

	RV TextEncoder::encode_float_value(const Variant& t, usize i)
	{
		lutry
		{
			char buf[64];
			switch (t.type())
			{
			case EVariantType::f32:
				sprintf_s(buf, "%f", t.to_f32_buf()[i]);
				break;
			case EVariantType::f64:
				sprintf_s(buf, "%f", t.to_f64_buf()[i]);
				break;
			default:
				lupanic();
				return BasicError::failure();
			}
			luexp(append(buf));
		}
		lucatchret;
		return RV();
	}

	RV TextEncoder::encode_string_value(const Variant& t, usize i)
	{
		lutry
		{
			String str;
			switch (t.type())
			{
			case EVariantType::string:
				str = t.to_str_buf()[i];
				break;
			case EVariantType::name:
				str = t.to_name_buf()[i];
				break;
			case EVariantType::path:
				str = t.to_path_buf()[i].encode();
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
		return RV();
	}

	RV TextEncoder::encode_blob_value(const Variant& t, usize i)
	{
		lutry
		{
			Blob blob = t.to_blob_buf()[i];
			usize sz = base64_get_encoded_size(blob.size());
			String buf;
			buf.resize(sz, '\0');
			base64_encode(buf.data(), buf.size() + 1, blob.data(), blob.size());
			luexp(append("\"base64??"));
			luexp(append("["));
			char count[64];
			sprintf_s(count, "%llu", (u64)buf.size());
			luexp(append(count));
			luexp(append("]"));
			luexp(append(buf.c_str()));
			luexp(push_back('"'));
		}
		lucatchret;
		return RV();
	}

	RV TextEncoder::encode_table_value(const Variant& t, usize i)
	{
		lutry
		{
			auto& table = t.to_var_table_buf()[i];

			if (table.empty())
			{
				luexp(append("{}"));
				return RV();
			}
			luexp(append("{ \n"));
			++m_indent;

			usize num_fields = table.size();
			auto iter = table.begin();
			for (usize j = 0; j < num_fields; ++j)
			{
				auto entry_name = iter->first;
				auto& entry = iter->second;
				if (entry.type() == EVariantType::object)
				{
					continue;
				}
				luexp(push_tabs());
				luexp(encode_table_entry(entry_name, entry));
				if (j != num_fields - 1)
				{
					luexp(append(",\n"));
				}
				++iter;
			}

			--m_indent;
			luexp(push_back('\n'));
			luexp(push_tabs());
			luexp(push_back('}'));
		}
		lucatchret;
		return RV();
	}

	RV TextEncoder::encode_table_entry(const Name& data_entry_name, const Variant& variant)
	{
		lutry
		{
			luexp(encode_identifier(data_entry_name));
			luexp(append(":"));
			luexp(encode_variant_object(variant));
		}
		lucatchret;
		return RV();
	}

	RV TextEncoder::encode_variant_value(const Variant& t, usize i)
	{
		lutry
		{
			auto& var = t.to_var_buf()[i];
			luexp(encode_variant_object(var));
		}
		lucatchret;
		return RV();
	}

	LUNA_CORE_API P<IEncoder> new_text_encoder()
	{
		return newobj<TextEncoder>();
	}
}