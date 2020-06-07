// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextEncoder.hpp
* @author JXMaster
* @date 2020/1/27
*/
#pragma once
#include "../CoreHeader.hpp"
#include <Base/TSAssert.hpp>

namespace luna
{
	class TextEncoder : public IEncoder
	{
	public:
		lucid("{244b432a-1938-4d1a-94df-bf3be1717123}");
		luiimpl(TextEncoder, IEncoder, IObject);
		lutsassert_lock();

		P<IStream> m_buf;
		uint32 m_indent;

		TextEncoder() :
			luibind(get_global_heap()) {}

		result_t append(const char* s)
		{
			lutry
			{
				uint32 sz = (uint32)(strlen(s) * sizeof(char));
				size_t written;
				luexp(m_buf->write(s, sz, &written));
				if (sz != written)
				{
					return e_failure;
				}
			}
			lucatchret;
			return s_ok;
		}

		result_t push_back(char ch)
		{
			lutry
			{
				size_t written;
				luexp(m_buf->write(&ch, sizeof(char), &written));
				if (sizeof(char) != written)
				{
					return e_failure;
				}
			}
			lucatchret;
			return s_ok;
		}

		result_t push_tabs()
		{
			lutry
			{
				for (uint32 i = 0; i < m_indent; ++i)
				{
					luexp(append("\t"));
				}
			}
			lucatchret;
			return s_ok;
		}

		virtual RV encode(IVariant* variant, IStream* stream) override;

		result_t encode_string_literal(IString* from)
		{
			const char* s = from->c_str();
			size_t len = from->size();
			lutry
			{
				for (size_t j = 0; j < len; ++j)
				{
					char ch = s[j];
					switch (ch)
					{
					case '\"':
						luexp(append("\\\""));
						break;
					case '\'':
						luexp(append("\\\'"));
						break;
					case '\\':
						luexp(append("\\\\"));
						break;
					case '\a':
						luexp(append("\\\a"));
						break;
					case '\b':
						luexp(append("\\\b"));
						break;
					case '\f':
						luexp(append("\\\f"));
						break;
					case '\n':
						luexp(append("\\\n"));
						break;
					case '\r':
						luexp(append("\\\r"));
						break;
					case '\t':
						luexp(append("\\\t"));
						break;
					case '\v':
						luexp(append("\\\v"));
						break;
					default:
						luexp(push_back(ch));
					}
				}
			}
			lucatchret;
			return s_ok;
		}

		bool is_identifier_char(char ch)
		{
			return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_');
		}

		result_t encode_identifier(IName* name)
		{
			const char* str = name->c_str();
			bool is_special = false;
			while (*str)
			{
				if (!is_identifier_char(*str))
				{
					is_special = true;
					break;
				}
				++str;
			}
			lutry
			{
				if (is_special)
				{
					luexp(append("\""));
					luexp(encode_string_literal(name));
					luexp(append("\""));
				}
				else
				{
					luexp(append(name->c_str()));
				}
			}
			lucatchret;
			return s_ok;
		}

		result_t encode_variant_object(IVariant* t);
		result_t encode_not_null_type(EVariantType t);
		result_t encode_variant_object_value(IVariant* v);
		result_t encode_single_data_value(IVariant* t, size_t i);
		result_t encode_integer_value(IVariant* t, size_t i);
		result_t encode_float_value(IVariant* t, size_t i);
		result_t encode_string_value(IVariant* t, size_t i);
		result_t encode_blob_value(IVariant* t, size_t i);
		result_t encode_table_value(IVariant* t, size_t i);
		result_t encode_table_entry(IName* data_entry_name, IVariant* variant);
		result_t encode_variant_value(IVariant* t, size_t i);
	};
}