// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextDecoder.cpp
* @author JXMaster
* @date 2020/1/30
*/
#include "TextDecoder.hpp"

namespace luna
{
	result_t TextDecoder::skip_null()
	{
		while (true)
		{
			char b;
			result_t r;
			r = read_char(b);
			if (failed(r))
			{
				return s_ok;
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
					return s_ok;
				}
				if (b == '/')
				{
					// skip until next line.
					while (true)
					{
						r = read_char(b);
						if (failed(r))
						{
							return s_ok;
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
							return s_ok;
						}
						if (b == '*')
						{
							r = read_char(b);
							if (failed(r))
							{
								return s_ok;
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
					return s_ok;
				}
			}
			else
			{
				seek_back_one(b);
				return s_ok;
			}
		}
	}

	inline bool is_hex(char c)
	{
		return (c >= '0' && c <= '9') ||
			(c >= 'a' && c <= 'f') ||
			(c >= 'A' && c <= 'F');
	}

	inline bool is_digit(char c)
	{
		return (c >= 0 && c <= 9);
	}

	result_t TextDecoder::decode_string_literal(IStringBuffer* to)
	{
		lutry
		{
			char b;
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
						to->push_back('\"');
						break;
					case '\'':
						to->push_back('\'');
						break;
					case '\\':
						to->push_back('\\');
						break;
					case 'a':
						to->push_back('\a');
						break;
					case 'b':
						to->push_back('\b');
						break;
					case 'f':
						to->push_back('\f');
						break;
					case 'n':
						to->push_back('\n');
						break;
					case 'r':
						to->push_back('\r');
						break;
					case 't':
						to->push_back('\t');
						break;
					case 'v':
						to->push_back('\v');
						break;
					case 'x':
					{
						luexp(read_char_and_report_eof(b));
						uint8 ch_high = atohex(b);
						luexp(read_char_and_report_eof(b));
						if (is_hex(b))
						{
							uint8 ch_low = atohex(b);
							to->push_back((ch_high << 4) + ch_low);
						}
						else
						{
							to->push_back(ch_high);
							seek_back_one(b);
						}
					}
					break;
					default:
						if (is_digit(b))
						{
							// 3 digits most.
							uint8 ch = b - '0';
							luexp(read_char_and_report_eof(b));
							if (is_digit(b))
							{
								ch = ch * 10 + (b - '0');
								luexp(read_char_and_report_eof(b));
								if (is_digit(b))
								{
									ch = ch * 10 + (b - '0');
									to->push_back(ch);
								}
								else
								{
									to->push_back(ch);
									seek_back_one(b);
								}
							}
							else
							{
								to->push_back(ch);
								seek_back_one(b);
							}
						}
						else
						{
							set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Unknown escape character got: \\%c.", m_line, m_idx, b);
							return e_user_failure;
						}
					}
				}
				else
				{
					to->push_back(b);
				}
			}
		}
		lucatchret;
		return s_ok;
	}

	result_t TextDecoder::decode_identifier()
	{
		m_temp_buf->clear();
		char b;
		lutry
		{
			luexp(read_char_and_report_eof(b));
			
			if (b == '"')
			{
				// Decode special.
				decode_string_literal(m_temp_buf);
			}
			else if(is_identifier_char(b))
			{
				// Decode normal.
				m_temp_buf->push_back(b);
				while (true)
				{
					if (failed(read_char(b)))
					{
						return s_ok;
					}
					if (!is_identifier_char(b))
					{
						seek_back_one(b);
						return s_ok;
					}
					m_temp_buf->push_back(b);
				}
			}
			else
			{
				set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Identifier expected, got '%c'", m_line, m_idx, b);
				// an identifier expected, but not got.
				return e_user_failure;
			}
		}
		lucatchret;
		return s_ok;
	}

	RP<IVariant> TextDecoder::decode(IStream* source_stream)
	{
		luassert_usr(source_stream);
		lutsassert();

		m_stream = source_stream;
		m_line = 1;
		m_idx = 1;
		m_tmp_char = -1;
		auto flags = source_stream->flags();
		if ((flags & EStreamFlag::readable) == EStreamFlag::none)
		{
			return e_not_available;
		}

		lutry
		{
			m_temp_buf = new_string_buffer();
			lulet(var, decode_variant_object());
			m_stream = nullptr;
			return var;
		}
		lucatch
		{
			m_stream = nullptr;
			m_temp_buf = nullptr;
			return lures;
		}
		return e_failure;
	}

	RP<IVariant> TextDecoder::decode_variant_object()
	{
		skip_null();
		// the variant object is expected to start with a string.
		decode_identifier();
		const char* ty = m_temp_buf->data();
		if (!strcmp(ty, "null"))
		{
			return new_var(EVariantType::null);
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
			set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Unknown type indentifier : \"%s\"", m_line, m_idx, ty);
			return e_user_failure;
		}
		lutry
		{
			bool no_branket;
			lulet(var, decode_demension_and_create_var(t, no_branket));
			luexp(decode_variant_object_value(var, no_branket));
			return var;
		}
		lucatchret;
		return s_ok;
	}
	RP<IVariant> TextDecoder::decode_demension_and_create_var(EVariantType t, bool& no_branket)
	{
		lutry
		{
			no_branket = false;
			size_t len[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
			size_t dim = 0;
			while (dim < 8)
			{
				luexp(skip_null());
				char ch;
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
					set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : '[' expected to start a type size field, got '%c'.", m_line, m_idx, ch);
					return e_user_failure;
				}
				luexp(skip_null());
				m_temp_buf->clear();
				luexp(read_char_and_report_eof(ch));
				while (ch != ']')
				{
					m_temp_buf->push_back(ch);
					luexp(read_char_and_report_eof(ch));
				}
				uint64 dim_num = strtoull(m_temp_buf->data(), nullptr, 0);
				if (!dim_num)
				{
					set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Invalid dimension number is specified before.", m_line, m_idx);
					return e_user_failure;
				}
	#ifdef LUNA_PLATFORM_WINDOWS
				if (dim_num > size_t_max_v)
				{
					set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : The dimension number before is too big for this platform: %llu", m_line, m_idx, dim_num);
					return e_user_failure;
				}
	#endif
				len[dim] = (size_t)dim_num;
				++dim;
			}
			if (dim == 1)
			{
				return new_var1(t, len[0]);
			}
			if (dim == 2)
			{
				return new_var2(t, len[1], len[0]);
			}
			if (dim == 3)
			{
				return new_var3(t, len[2], len[1], len[0]);
			}
			if (dim == 4)
			{
				return new_var4(t, len[3], len[2], len[1], len[0]);
			}
			if (dim == 5)
			{
				return new_var5(t, len[4], len[3], len[2], len[1], len[0]);
			}
			if (dim == 6)
			{
				return new_var6(t, len[5], len[4], len[3], len[2], len[1], len[0]);
			}
			if (dim == 7)
			{
				return new_var7(t, len[6], len[5], len[4], len[3], len[2], len[1], len[0]);
			}
			if (dim == 8)
			{
				return new_var8(t, len[7], len[6], len[5], len[4], len[3], len[2], len[1], len[0]);
			}
			lupanic();
			return nullptr;
		}
		lucatchret;
		return s_ok;
	}

	result_t TextDecoder::decode_variant_object_value(IVariant* var, bool no_branket)
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
				char ch;
				luexp(read_char_and_report_eof(ch));
				if (ch == '{')
				{
					luexp(skip_null());
					size_t index = 0;
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
							set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Unknown character detected: '%c'", m_line, m_idx, ch);
							return e_user_failure;
						}
						luexp(skip_null());
						++index;
					}
				}
				else
				{
					set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : '{' expected to start the variant object value array, got '%c'", m_line, m_idx, ch);
					return e_user_failure;
				}
			}
		}
		lucatchret;
		return s_ok;
	}

	result_t TextDecoder::decode_single_data_value(IVariant* var, size_t index)
	{
		lutry
		{
			EVariantType t = var->type();
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
		return s_ok;
	}

	result_t TextDecoder::decode_integer_value(IVariant* var, size_t index)
	{
		m_temp_buf->clear();
		char b;
		result_t r = s_ok;
		while (succeeded(r = read_char(b)))
		{
			if (b == '}' || b == ',')
			{
				seek_back_one(b);
				break;
			}
			m_temp_buf->push_back(b);
		}
		if (failed(r) && r != e_eof)
		{
			return r;
		}
		if (m_temp_buf->empty())
		{
			set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Integer expected, got none.", m_line, m_idx);
			return e_user_failure;
		}
		EVariantType t = var->type();
		if (t == EVariantType::u64)
		{
			char* endptr;
			uint64 v = strtoull(m_temp_buf->data(), &endptr, 0);
			if (endptr == m_temp_buf->data())
			{
				set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Integer expected, got none.", m_line, m_idx);
				return e_user_failure;
			}
			var->u64_buf()[index] = v;
		}
		else
		{
			char* endptr;
			int64 v = strtoll(m_temp_buf->data(), &endptr, 0);
			if (endptr == m_temp_buf->data())
			{
				set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Integer expected, got none.", m_line, m_idx);
				return e_user_failure;
			}
			switch (t)
			{
			case luna::EVariantType::u8:
				var->u8_buf()[index] = (uint8)v;
				break;
			case luna::EVariantType::i8:
				var->i8_buf()[index] = (int8)v;
				break;
			case luna::EVariantType::u16:
				var->u16_buf()[index] = (uint16)v;
				break;
			case luna::EVariantType::i16:
				var->i16_buf()[index] = (int16)v;
				break;
			case luna::EVariantType::u32:
				var->u32_buf()[index] = (uint32)v;
				break;
			case luna::EVariantType::i32:
				var->i32_buf()[index] = (int32)v;
				break;
			case luna::EVariantType::i64:
				var->i64_buf()[index] = (int64)v;
				break;
			default:
				lupanic();
				break;
			}
		}
		return s_ok;
	}

	result_t TextDecoder::decode_float_value(IVariant* var, size_t index)
	{
		m_temp_buf->clear();
		char b;
		result_t r = s_ok;
		while (succeeded(r = read_char(b)))
		{
			if (b == '}' || b == ',')
			{
				seek_back_one(b);
				break;
			}
			m_temp_buf->push_back(b);
		}
		if (failed(r) && r != e_eof)
		{
			return r;
		}
		if (m_temp_buf->empty())
		{
			set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Float expected, got none.", m_line, m_idx);
			return e_user_failure;
		}
		EVariantType t = var->type();
		if (t == EVariantType::f32)
		{
			char* endptr;
			float32 v = strtof(m_temp_buf->data(), &endptr);
			if (endptr == m_temp_buf->data())
			{
				set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Float expected, got none.", m_line, m_idx);
				return e_user_failure;
			}
			var->f32_buf()[index] = v;
		}
		else if (t == EVariantType::f64)
		{
			char* endptr;
			float64 v = strtod(m_temp_buf->data(), &endptr);
			if (endptr == m_temp_buf->data())
			{
				set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Float expected, got none.", m_line, m_idx);
				return e_user_failure;
			}
			var->f64_buf()[index] = v;
		}
		else
		{
			lupanic();
		}
		return s_ok;
	}

	result_t TextDecoder::decode_boolean_value(IVariant* var, size_t index)
	{
		lutry
		{
			char v[6];
			char b;
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
					bit_set(var->boolean_buf(), index);
				}
				else
				{
					set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Invalid character, boolean expected, got \"%s\".", m_line, m_idx, v);
					return e_user_failure;
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
					bit_reset(var->boolean_buf(), index);
				}
				else
				{
					set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Invalid character, boolean expected, got \"%s\".", m_line, m_idx, v);
					return e_user_failure;
				}
			}
			else
			{
				set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Invalid character, boolean expected, got '%c'.", m_line, m_idx, b);
				return e_user_failure;
			}
		}
		lucatchret;
		return s_ok;
	}

	result_t TextDecoder::decode_string_value(IVariant* var, size_t index)
	{
		lutry
		{
			char b;
			luexp(read_char_and_report_eof(b));
			if (b != '\"')
			{
				set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : String beginning '\"' expected, got '%c'.", m_line, m_idx, b);
				return e_user_failure;
			}
			auto str = new_string_buffer();
			luexp(decode_string_literal(str));
			if (var->type() == EVariantType::string)
			{
				var->str_buf()[index] = str;
			}
			else if(var->type() == EVariantType::name)
			{
				var->name_buf()[index] = intern_name(str->data());
			}
			else if (var->type() == EVariantType::path)
			{
				var->path_buf()[index] = new_path(str->data());
			}
			else
			{
				lupanic();
			}
		}
		lucatchret;
		return s_ok;
	}

	result_t TextDecoder::decode_blob_value(IVariant* var, size_t index)
	{
		lutry
		{
			char b;
			luexp(read_char_and_report_eof(b));
			if (b != '\"')
			{
				set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Blob string expected.", m_line, m_idx);
				return e_user_failure;
			}
			// check header.
			char header[9];
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
				set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Blob string does not start with \"base64??\".", m_line, m_idx);
				return e_user_failure;
			}
			auto str = new_string_buffer();
			luexp(read_char_and_report_eof(b));
			int64 data_size = -1;
			if (b == '[')
			{
				auto count = new_string_buffer();
				luexp(read_char_and_report_eof(b));
				while (b != ']')
				{
					count->push_back(b);
					luexp(read_char_and_report_eof(b));
				}
				data_size = atoll(count->c_str());
				str->reserve((size_t)data_size);
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
						str->push_back(b);
					}
					else
					{
						set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : Invalid base64 character detected.", m_line, m_idx);
						return e_user_failure;
					}
				}
			}
			else
			{
				luexp(read_chars(str, data_size));
				luexp(read_char(b));
			}
			
			// decode base64.
			var->blob_buf()[index] = base64_decode_buffered(str->data());
		}
		lucatchret;
		return s_ok;
	}

	result_t TextDecoder::decode_table_value(IVariant* var, size_t index)
	{
		lutry
		{
			char b;
			luexp(read_char_and_report_eof(b));
			if (b != '{')
			{
				set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : `{` expected to start a table scope, got '%c'.", m_line, m_idx, b);
				return e_user_failure;
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
						set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : ',' expected at the end of every table entry.", m_line, m_idx);
						return e_user_failure;
					}
					luexp(skip_null());
				}
			}
		}
		lucatchret;
		return s_ok;
	}

	result_t TextDecoder::decode_table_entry(IVariant* var, size_t index)
	{
		lutry
		{
			decode_identifier();
			auto field = intern_name(m_temp_buf->data());
			luexp(skip_null());
			char b;
			luexp(read_char_and_report_eof(b));
			if (b != ':')
			{
				set_err(e_syntax_error, "IDecoder::decode - Line %u Character %u : ':' expected at the end of table entry name.", m_line, m_idx);
				return e_user_failure;
			}
			luexp(skip_null());
			lulet(var2, decode_variant_object());
			var->set_field(index, field, var2);
		}
		lucatchret;
		return s_ok;
	}

	result_t TextDecoder::decode_variant_value(IVariant* var, size_t index)
	{
		lutry
		{
			lulet(var2,decode_variant_object());
			var->var_buf()[index] = var2;
		}
		lucatchret;
		return s_ok;
	}
}