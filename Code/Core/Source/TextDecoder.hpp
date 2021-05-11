// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextDecoder.hpp
* @author JXMaster
* @date 2020/1/30
*/
#pragma once
#include "../IDecoder.hpp"
#include "../Interface.hpp"

namespace Luna
{
	class TextDecoder : public IDecoder
	{
	public:
		lucid("{5dc4e490-80bc-4c6f-a1e6-db4c9e4122d3}");
		luiimpl(TextDecoder, IDecoder, IObject);
		lutsassert_lock();

		P<IStream> m_stream;
		String m_temp_buf;

		// For debug purpose.
		u32 m_line;
		u32 m_idx;
		i16 m_tmp_char;	// buffer at most one char to present seek back operation.

		TextDecoder() {}

		//! using this to read one char from stream.
		RV read_char(c8& ch)
		{
			if (m_tmp_char >= 0)
			{
				ch = (c8)m_tmp_char;
				m_tmp_char = -1;
				return RV();
			}
			usize bytes;
			RV r = m_stream->read(&ch, sizeof(c8), &bytes);
			if (failed(r))
			{
				return r;
			}
			if (bytes != sizeof(c8))
			{
				return BasicError::end_of_file();
			}
			if (ch == '\n')
			{
				++m_line;
				m_idx = 1;
			}
			else
			{
				++m_idx;
			}
			return RV();
		}

		RV read_chars(String& buf, usize num_chars_to_read)
		{
			// Must be in one line.
			if (m_tmp_char >= 0)
			{
				c8 ch = (c8)m_tmp_char;
				m_tmp_char = -1;
				buf.push_back(ch);
				--num_chars_to_read;
			}
			usize bytes;
			usize begin_pos = buf.size();
			buf.resize(buf.size() + num_chars_to_read, '\0');
			RV r = m_stream->read(buf.data() + begin_pos, sizeof(c8) * num_chars_to_read, &bytes);
			if (failed(r))
			{
				return r;
			}
			if (bytes != sizeof(c8) * num_chars_to_read)
			{
				return BasicError::end_of_file();
			}
			m_idx += (u32)num_chars_to_read;
			return RV();
		}

		RV read_char_and_report_eof(c8& ch)
		{
			RV r = read_char(ch);
			if (r.errcode() == BasicError::end_of_file())
			{
				get_error_object() = Error(BasicError::end_of_file(), "IDecoder::decode - Line %u Character %u : Unexpected EOF got.", m_line, m_idx);
				return BasicError::error_object();
			}
			return r;
		}
		void seek_back_one(c8 ch)
		{
			m_tmp_char = ch;
		}

		//! Skips any null characters, including space character, tab character, return character
		//! and comments. The cursor points to the next non-null character after this operation.
		RV skip_null();

		//bool is_first_identifier_char(char ch)
		//{
		//	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_');
		//}

		bool is_identifier_char(c8 ch)
		{
			return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_');
		}

		RV decode_string_literal(String& to);

		//! Decodes a sequence as a identifier, which contains 0-9, a-z, A-Z and '_', started with
		//! non-number.
		RV decode_identifier();

		virtual R<Variant> decode(IStream* source_stream) override;

		R<Variant> decode_variant_object();
		R<Variant> decode_demension_and_create_var(EVariantType t, bool& no_branket);
		RV decode_variant_object_value(Variant& var, bool no_branket);
		RV decode_single_data_value(Variant& var, usize index);
		RV decode_integer_value(Variant& var, usize index);
		RV decode_float_value(Variant& var, usize index);
		RV decode_boolean_value(Variant& var, usize index);
		RV decode_string_value(Variant& var, usize index);
		RV decode_blob_value(Variant& var, usize index);
		RV decode_table_value(Variant& var, usize index);
		RV decode_table_entry(Variant& var, usize index);
		RV decode_variant_value(Variant& var, usize index);
	};
}