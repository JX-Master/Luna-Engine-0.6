// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextDecoder.cpp
* @author JXMaster
* @date 2020/1/30
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT
#include "TextDecoder.hpp"
#include <Runtime/Base64.hpp>
#include "../Core.hpp"

namespace Luna
{
	RV TextDecoder::skip_null()
	{
		while (true)
		{
			c8 b;
			RV r;
			r = read_char(b);
			if (failed(r))
			{
				return RV();
			}
			if (b == ' ' || b == '\t' || b == '\r' || b == '\n')
			{
				continue;
			}
			if (b == '/')
			{
				// may be a comment?
				r = read_char(b);
				if (failed(r))
				{
					return RV();
				}
				if (b == '/')
				{
					// skip until next line.
					while (true)
					{
						r = read_char(b);
						if (failed(r))
						{
							return RV();
						}
						if (b == '\n')
						{
							break;
						}
					}
				}
				else if(b == '*')
				{
					// skip until '*/'
					while (true)
					{
						r = read_char(b);
						if (failed(r))
						{
							return RV();
						}
						if (b == '*')
						{
							r = read_char(b);
							if (failed(r))
							{
								return RV();
							}
							if (b == '/')
							{
								break;
							}
						}
					}
				}
				else
				{
					lutry
					{
						luexp(m_stream->seek(-2, ESeekMode::current));
					}
					lucatchret;
					return RV();
				}
			}
			else
			{
				seek_back_one(b);
				return RV();
			}
		}
	}

	inline bool is_hex(c8 c)
	{
		return (c >= '0' && c <= '9') ||
			(c >= 'a' && c <= 'f') ||
			(c >= 'A' && c <= 'F');
	}

	inline bool is_digit(c8 c)
	{
		return (c >= 0 && c <= 9);
	}

	RV TextDecoder::decode_string_literal(String& to)
	{
		lutry
		{
			c8 b;
			while (true)
			{
				luexp(read_char_and_report_eof(b));
				if (b == '\"')
				{
					break;
				}
				else if (b == '\\')
				{
					luexp(read_char_and_report_eof(b));
					switch (b)
					{
					case '\"':
						to.push_back('\"');
						break;
					case '\'':
						to.push_back('\'');
						break;
					case '\\':
						to.push_back('\\');
						break;
					case 'a':
						to.push_back('\a');
						break;
					case 'b':
						to.push_back('\b');
						break;
					case 'f':
						to.push_back('\f');
						break;
					case 'n':
						to.push_back('\n');
						break;
					case 'r':
						to.push_back('\r');
						break;
					case 't':
						to.push_back('\t');
						break;
					case 'v':
						to.push_back('\v');
						break;
					case 'x':
					{
						luexp(read_char_and_report_eof(b));
						u8 ch_high = atohex(b);
						luexp(read_char_and_report_eof(b));
						if (is_hex(b))
						{
							u8 ch_low = atohex(b);
							to.push_back((ch_high << 4) + ch_low);
						}
						else
						{
							to.push_back(ch_high);
							seek_back_one(b);
						}
					}
					break;
					default:
						if (is_digit(b))
						{
							// 3 digits most.
							u8 ch = b - '0';
							luexp(read_char_and_report_eof(b));
							if (is_digit(b))
							{
								ch = ch * 10 + (b - '0');
								luexp(read_char_and_report_eof(b));
								if (is_digit(b))
								{
									ch = ch * 10 + (b - '0');
									to.push_back(ch);
								}
								else
								{
									to.push_back(ch);
									seek_back_one(b);
								}
							}
							else
							{
								to.push_back(ch);
								seek_back_one(b);
							}
						}
						else
						{
							return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Unknown escape character got: \\%c.", m_line, m_idx, b);
						}
					}
				}
				else
				{
					to.push_back(b);
				}
			}
		}
		lucatchret;
		return RV();
	}

	RV TextDecoder::decode_identifier()
	{
		m_temp_buf.clear();
		c8 b;
		lutry
		{
			luexp(read_char_and_report_eof(b));
			
			if (b == '"')
			{
				// Decode special.
				luexp(decode_string_literal(m_temp_buf));
			}
			else if(is_identifier_char(b))
			{
				// Decode normal.
				m_temp_buf.push_back(b);
				while (true)
				{
					if (failed(read_char(b)))
					{
						return RV();
					}
					if (!is_identifier_char(b))
					{
						seek_back_one(b);
						return RV();
					}
					m_temp_buf.push_back(b);
				}
			}
			else
			{
				// an identifier expected, but not got.
				return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Identifier expected, got '%c'", m_line, m_idx, b);
			}
		}
		lucatchret;
		return RV();
	}

	R<Variant> TextDecoder::decode(IStream* source_stream)
	{
		lucheck(source_stream);
		lutsassert();

		m_stream = source_stream;
		m_line = 1;
		m_idx = 1;
		m_tmp_char = -1;
		auto flags = source_stream->flags();
		if ((flags & EStreamFlag::readable) == EStreamFlag::none)
		{
			return BasicError::not_supported();
		}

		lutry
		{
			m_temp_buf = String();
			lulet(var, decode_variant_object());
			m_stream.clear();
			return move(var);
		}
		lucatch
		{
			m_stream.clear();
			m_temp_buf.clear();
			return lures;
		}
		return BasicError::failure();
	}

	R<Variant> TextDecoder::decode_variant_object()
	{
		lutry
		{
			luexp(skip_null());
			// the variant object is expected to start with a string.
			luexp(decode_identifier());
		}
		lucatchret;
		const char* ty = m_temp_buf.data();
		if (!strcmp(ty, "null"))
		{
			return Variant(EVariantType::null);
		}
		EVariantType t;
		if (!strcmp(ty, "u8")) { t = EVariantType::u8; }
		else if (!strcmp(ty, "i8")) { t = EVariantType::i8; }
		else if (!strcmp(ty, "u16")) { t = EVariantType::u16; }
		else if (!strcmp(ty, "i16")) { t = EVariantType::i16; }
		else if (!strcmp(ty, "u32")) { t = EVariantType::u32; }
		else if (!strcmp(ty, "i32")) { t = EVariantType::i32; }
		else if (!strcmp(ty, "u64")) { t = EVariantType::u64; }
		else if (!strcmp(ty, "i64")) { t = EVariantType::i64; }
		else if (!strcmp(ty, "f32")) { t = EVariantType::f32; }
		else if (!strcmp(ty, "f64")) { t = EVariantType::f64; }
		else if (!strcmp(ty, "bool")) { t = EVariantType::boolean; }
		else if (!strcmp(ty, "string")) { t = EVariantType::string; }
		else if (!strcmp(ty, "blob")) { t = EVariantType::blob; }
		else if (!strcmp(ty, "label")) { t = EVariantType::name; }
		else if (!strcmp(ty, "path")) { t = EVariantType::path; }
		else if (!strcmp(ty, "table")) { t = EVariantType::table; }
		else if (!strcmp(ty, "var")) { t = EVariantType::variant; }
		else
		{
			return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Unknown type indentifier : \"%s\"", m_line, m_idx, ty);
		}
		lutry2
		{
			bool no_branket;
			lulet2(var, decode_demension_and_create_var(t, no_branket));
			luexp2(decode_variant_object_value(var, no_branket));
			return move(var);
		}
		lucatchret2;
		lupanic();
		return BasicError::failure();
	}
	R<Variant> TextDecoder::decode_demension_and_create_var(EVariantType t, bool& no_branket)
	{
		lutry
		{
			no_branket = false;
			usize len[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
			usize dim = 0;
			while (dim < 8)
			{
				luexp(skip_null());
				c8 ch;
				luexp(read_char_and_report_eof(ch));
				if (ch == '=')
				{
					if (dim == 0)
					{
						dim = 1;
						no_branket = true;
					}
					break;
				}
				if (ch != '[')
				{
					return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : '[' expected to start a type size field, got '%c'.", m_line, m_idx, ch);
				}
				luexp(skip_null());
				m_temp_buf.clear();
				luexp(read_char_and_report_eof(ch));
				while (ch != ']')
				{
					m_temp_buf.push_back(ch);
					luexp(read_char_and_report_eof(ch));
				}
				u64 dim_num = strtoull(m_temp_buf.data(), nullptr, 0);
				if (!dim_num)
				{
					return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Invalid dimension number is specified before.", m_line, m_idx);
				}
	#ifdef LUNA_PLATFORM_WINDOWS
				if (dim_num > usize_max)
				{
					return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : The dimension number before is too big for this platform: %llu", m_line, m_idx, dim_num);
				}
	#endif
				len[dim] = (usize)dim_num;
				++dim;
			}
			if (dim == 1)
			{
				return Variant(t, len[0]);
			}
			if (dim == 2)
			{
				return Variant(t, len[1], len[0]);
			}
			if (dim == 3)
			{
				return Variant(t, len[2], len[1], len[0]);
			}
			if (dim == 4)
			{
				return Variant(t, len[3], len[2], len[1], len[0]);
			}
			if (dim == 5)
			{
				return Variant(t, len[4], len[3], len[2], len[1], len[0]);
			}
			if (dim == 6)
			{
				return Variant(t, len[5], len[4], len[3], len[2], len[1], len[0]);
			}
			if (dim == 7)
			{
				return Variant(t, len[6], len[5], len[4], len[3], len[2], len[1], len[0]);
			}
			if (dim == 8)
			{
				return Variant(t, len[7], len[6], len[5], len[4], len[3], len[2], len[1], len[0]);
			}
			lupanic();
			return BasicError::failure();
		}
		lucatchret;
		lupanic();
		return BasicError::failure();
	}

	RV TextDecoder::decode_variant_object_value(Variant& var, bool no_branket)
	{
		lutry
		{
			luexp(skip_null());

			if (no_branket)
			{
				luexp(decode_single_data_value(var, 0));
			}
			else
			{
				c8 ch;
				luexp(read_char_and_report_eof(ch));
				if (ch == '{')
				{
					luexp(skip_null());
					usize index = 0;
					while (true)
					{
						luexp(read_char_and_report_eof(ch));
						if (ch == '}')
						{
							break;
						}
						else
						{
							seek_back_one(ch);
						}
						luexp(decode_single_data_value(var, index));
						luexp(skip_null());
						luexp(read_char_and_report_eof(ch));
						if (ch == '}')
						{
							break;
						}
						else if (ch != ',')
						{
							return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Unknown character detected: '%c'", m_line, m_idx, ch);
						}
						luexp(skip_null());
						++index;
					}
				}
				else
				{
					return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : '{' expected to start the variant object value array, got '%c'", m_line, m_idx, ch);
				}
			}
		}
		lucatchret;
		return RV();
	}

	RV TextDecoder::decode_single_data_value(Variant& var, usize index)
	{
		lutry
		{
			EVariantType t = var.type();
			switch (t)
			{
			case EVariantType::u8:
			case EVariantType::i8:
			case EVariantType::u16:
			case EVariantType::i16:
			case EVariantType::u32:
			case EVariantType::i32:
			case EVariantType::u64:
			case EVariantType::i64:
				luexp(decode_integer_value(var, index));
				break;
			case EVariantType::f32:
			case EVariantType::f64:
				luexp(decode_float_value(var, index));
				break;
			case EVariantType::boolean:
				luexp(decode_boolean_value(var, index));
				break;
			case EVariantType::string:
			case EVariantType::name:
			case EVariantType::path:
				luexp(decode_string_value(var, index));
				break;
			case EVariantType::blob:
				luexp(decode_blob_value(var, index));
				break;
			case EVariantType::table:
				luexp(decode_table_value(var, index));
				break;
			case EVariantType::variant:
				luexp(decode_variant_value(var, index));
				break;
			default:
				lupanic();
			}
		}
		lucatchret;
		return RV();
	}

	RV TextDecoder::decode_integer_value(Variant& var, usize index)
	{
		m_temp_buf.clear();
		c8 b;
		RV r = RV();
		while (succeeded(r = read_char(b)))
		{
			if (b == '}' || b == ',')
			{
				seek_back_one(b);
				break;
			}
			m_temp_buf.push_back(b);
		}
		if (failed(r) && get_errcode(r) != BasicError::end_of_file())
		{
			return r;
		}
		if (m_temp_buf.empty())
		{
			return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Integer expected, got none.", m_line, m_idx);
		}
		EVariantType t = var.type();
		if (t == EVariantType::u64)
		{
			char* endptr;
			u64 v = strtoull(m_temp_buf.data(), &endptr, 0);
			if (endptr == m_temp_buf.data())
			{
				return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Integer expected, got none.", m_line, m_idx);
			}
			var.to_u64_buf()[index] = v;
		}
		else
		{
			char* endptr;
			i64 v = strtoll(m_temp_buf.data(), &endptr, 0);
			if (endptr == m_temp_buf.data())
			{
				return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Integer expected, got none.", m_line, m_idx);
			}
			switch (t)
			{
			case Luna::EVariantType::u8:
				var.to_u8_buf()[index] = (u8)v;
				break;
			case Luna::EVariantType::i8:
				var.to_i8_buf()[index] = (i8)v;
				break;
			case Luna::EVariantType::u16:
				var.to_u16_buf()[index] = (u16)v;
				break;
			case Luna::EVariantType::i16:
				var.to_i16_buf()[index] = (i16)v;
				break;
			case Luna::EVariantType::u32:
				var.to_u32_buf()[index] = (u32)v;
				break;
			case Luna::EVariantType::i32:
				var.to_i32_buf()[index] = (i32)v;
				break;
			case Luna::EVariantType::i64:
				var.to_i64_buf()[index] = (i64)v;
				break;
			default:
				lupanic();
				break;
			}
		}
		return RV();
	}

	RV TextDecoder::decode_float_value(Variant& var, usize index)
	{
		m_temp_buf.clear();
		c8 b;
		RV r = RV();
		while (succeeded(r = read_char(b)))
		{
			if (b == '}' || b == ',')
			{
				seek_back_one(b);
				break;
			}
			m_temp_buf.push_back(b);
		}
		if (failed(r) && get_errcode(r) != BasicError::end_of_file())
		{
			return r;
		}
		if (m_temp_buf.empty())
		{
			return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Float expected, got none.", m_line, m_idx);
		}
		EVariantType t = var.type();
		if (t == EVariantType::f32)
		{
			char* endptr;
			f32 v = strtof(m_temp_buf.data(), &endptr);
			if (endptr == m_temp_buf.data())
			{
				return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Float expected, got none.", m_line, m_idx);
			}
			var.to_f32_buf()[index] = v;
		}
		else if (t == EVariantType::f64)
		{
			char* endptr;
			f64 v = strtod(m_temp_buf.data(), &endptr);
			if (endptr == m_temp_buf.data())
			{
				return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Float expected, got none.", m_line, m_idx);
			}
			var.to_f64_buf()[index] = v;
		}
		else
		{
			lupanic();
		}
		return RV();
	}

	RV TextDecoder::decode_boolean_value(Variant& var, usize index)
	{
		lutry
		{
			c8 v[6];
			c8 b;
			luexp(read_char_and_report_eof(b));
			v[0] = b;
			if (b == 't')
			{
				luexp(read_char_and_report_eof(b));
				v[1] = b;
				luexp(read_char_and_report_eof(b));
				v[2] = b;
				luexp(read_char_and_report_eof(b));
				v[3] = b;
				v[4] = '\0';
				if (!strcmp(v, "true"))
				{
					bit_set(var.to_boolean_buf(), index);
				}
				else
				{
					return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Invalid character, boolean expected, got \"%s\".", m_line, m_idx, v);
				}
			}
			else if (b == 'f')
			{
				luexp(read_char_and_report_eof(b));
				v[1] = b;
				luexp(read_char_and_report_eof(b));
				v[2] = b;
				luexp(read_char_and_report_eof(b));
				v[3] = b;
				luexp(read_char_and_report_eof(b));
				v[4] = b;
				v[5] = '\0';
				if (!strcmp(v, "false"))
				{
					bit_reset(var.to_boolean_buf(), index);
				}
				else
				{
					return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Invalid character, boolean expected, got \"%s\".", m_line, m_idx, v);
				}
			}
			else
			{
				return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Invalid character, boolean expected, got '%c'.", m_line, m_idx, b);
			}
		}
		lucatchret;
		return RV();
	}

	RV TextDecoder::decode_string_value(Variant& var, usize index)
	{
		lutry
		{
			c8 b;
			luexp(read_char_and_report_eof(b));
			if (b != '\"')
			{
				return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : String beginning '\"' expected, got '%c'.", m_line, m_idx, b);
			}
			auto str = String();
			luexp(decode_string_literal(str));
			if (var.type() == EVariantType::string)
			{
				var.to_str_buf()[index] = str;
			}
			else if(var.type() == EVariantType::name)
			{
				var.to_name_buf()[index] = intern_name(str.data());
			}
			else if (var.type() == EVariantType::path)
			{
				var.to_path_buf()[index] = Path(str.data());
			}
			else
			{
				lupanic();
			}
		}
		lucatchret;
		return RV();
	}

	RV TextDecoder::decode_blob_value(Variant& var, usize index)
	{
		lutry
		{
			c8 b;
			luexp(read_char_and_report_eof(b));
			if (b != '\"')
			{
				return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Blob string expected.", m_line, m_idx);
			}
			// check header.
			c8 header[9];
			luexp(read_char_and_report_eof(header[0]));
			luexp(read_char_and_report_eof(header[1]));
			luexp(read_char_and_report_eof(header[2]));
			luexp(read_char_and_report_eof(header[3]));
			luexp(read_char_and_report_eof(header[4]));
			luexp(read_char_and_report_eof(header[5]));
			luexp(read_char_and_report_eof(header[6]));
			luexp(read_char_and_report_eof(header[7]));
			header[8] = '\0';
			if (strcmp(header, "base64??"))
			{
				return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Blob string does not start with \"base64??\".", m_line, m_idx);
			}
			auto str = String();
			luexp(read_char_and_report_eof(b));
			i64 data_size = -1;
			if (b == '[')
			{
				auto count = String();
				luexp(read_char_and_report_eof(b));
				while (b != ']')
				{
					count.push_back(b);
					luexp(read_char_and_report_eof(b));
				}
				data_size = atoll(count.c_str());
				str.reserve((usize)data_size);
			}
			else
			{
				seek_back_one(b);
			}

			if (data_size == -1)
			{
				while (true)
				{
					luexp(read_char_and_report_eof(b));
					if (b == '\"')
					{
						break;
					}
					else if ((b >= '0' && b <= '9') ||
						(b >= 'a' && b <= 'z') ||
						(b >= 'A' && b <= 'Z') ||
						b == '/' || b == '+' || b == '=')
					{
						str.push_back(b);
					}
					else
					{
						return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : Invalid base64 character detected.", m_line, m_idx);
					}
				}
			}
			else
			{
				luexp(read_chars(str, data_size));
				luexp(read_char(b));
			}
			
			// decode base64.
			usize buff_sz = base64_get_decoded_size(str.size());
			var.to_blob_buf()[index].resize(buff_sz);
			base64_decode(var.to_blob_buf()[index].data(), var.to_blob_buf()[index].size(), str.data());
		}
		lucatchret;
		return RV();
	}

	RV TextDecoder::decode_table_value(Variant& var, usize index)
	{
		lutry
		{
			c8 b;
			luexp(read_char_and_report_eof(b));
			if (b != '{')
			{
				return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : `{` expected to start a table scope, got '%c'.", m_line, m_idx, b);
			}
			luexp(skip_null());
			luexp(read_char_and_report_eof(b));
			if (b != '}')
			{
				seek_back_one(b);
				while (true)
				{
					luexp(read_char_and_report_eof(b));
					if (b == '}')
					{
						break;
					}
					else
					{
						seek_back_one(b);
					}
					luexp(decode_table_entry(var, index));
					luexp(skip_null());
					luexp(read_char_and_report_eof(b));
					if (b == '}')
					{
						break;
					}
					if (b != ',')
					{
						return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : ',' expected at the end of every table entry.", m_line, m_idx);
					}
					luexp(skip_null());
				}
			}
		}
		lucatchret;
		return RV();
	}

	RV TextDecoder::decode_table_entry(Variant& var, usize index)
	{
		lutry
		{
			luexp(decode_identifier());
			auto field = intern_name(m_temp_buf.data());
			luexp(skip_null());
			c8 b;
			luexp(read_char_and_report_eof(b));
			if (b != ':')
			{
				return custom_error(BasicError::syntax_error(), "IDecoder::decode - Line %u Character %u : ':' expected at the end of table entry name.", m_line, m_idx);
			}
			luexp(skip_null());
			lulet(var2, decode_variant_object());
			var.set_field(index, field, move(var2));
		}
		lucatchret;
		return RV();
	}

	RV TextDecoder::decode_variant_value(Variant& var, usize index)
	{
		lutry
		{
			lulet(var2,decode_variant_object());
			var.to_var_buf()[index] = move(var2);
		}
		lucatchret;
		return RV();
	}

	LUNA_CORE_API P<IDecoder> new_text_decoder()
	{
		return newobj<TextDecoder>();
	}
}