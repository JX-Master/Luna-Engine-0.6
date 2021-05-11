// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextEncoder.hpp
* @author JXMaster
* @date 2020/1/27
*/
#pragma once
#include "../IEncoder.hpp"
#include "../Interface.hpp"

namespace Luna
{
	class TextEncoder : public IEncoder
	{
	public:
		lucid("{244b432a-1938-4d1a-94df-bf3be1717123}");
		luiimpl(TextEncoder, IEncoder, IObject);
		lutsassert_lock();

		P<IStream> m_buf;
		u32 m_indent;

		TextEncoder() {}

		RV append(const char* s)
		{
			lutry
			{
				u32 sz = (u32)(strlen(s) * sizeof(char));
				usize written;
				luexp(m_buf->write(s, sz, &written));
				if (sz != written)
				{
					return BasicError::failure();
				}
			}
			lucatchret;
			return RV();
		}

		RV push_back(char ch)
		{
			lutry
			{
				usize written;
				luexp(m_buf->write(&ch, sizeof(char), &written));
				if (sizeof(char) != written)
				{
					return BasicError::failure();
				}
			}
			lucatchret;
			return RV();
		}

		RV push_tabs()
		{
			lutry
			{
				for (u32 i = 0; i < m_indent; ++i)
				{
					luexp(append("\t"));
				}
			}
			lucatchret;
			return RV();
		}

		virtual RV encode(const Variant& variant, IStream* stream) override;

		RV encode_string_literal(const String& from)
		{
			const char* s = from.c_str();
			usize len = from.size();
			lutry
			{
				for (usize j = 0; j < len; ++j)
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
			return RV();
		}

		bool is_identifier_char(char ch)
		{
			return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_');
		}

		RV encode_identifier(const Name& name)
		{
			const char* str = name.c_str();
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
					luexp(append(name.c_str()));
				}
			}
			lucatchret;
			return RV();
		}

		RV encode_variant_object(const Variant& t);
		RV encode_not_null_type(EVariantType t);
		RV encode_variant_object_value(const Variant& v);
		RV encode_single_data_value(const Variant& t, usize i);
		RV encode_integer_value(const Variant& t, usize i);
		RV encode_float_value(const Variant& t, usize i);
		RV encode_string_value(const Variant& t, usize i);
		RV encode_blob_value(const Variant& t, usize i);
		RV encode_table_value(const Variant& t, usize i);
		RV encode_table_entry(const Name& data_entry_name, const Variant& variant);
		RV encode_variant_value(const Variant& t, usize i);
	};
}