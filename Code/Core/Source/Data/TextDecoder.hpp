// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextDecoder.hpp
* @author JXMaster
* @date 2020/1/30
*/
#pragma once
#include "../CoreHeader.hpp"
#include <Base/TSAssert.hpp>

namespace luna
{
	class TextDecoder : public IDecoder
	{
	public:
		lucid("{5dc4e490-80bc-4c6f-a1e6-db4c9e4122d3}");
		luiimpl(TextDecoder, IDecoder, IObject);
		lutsassert_lock();

		P<IStream> m_stream;
		P<IStringBuffer> m_temp_buf;

		// For debug purpose.
		uint32 m_line;
		uint32 m_idx;
		int16 m_tmp_char;	// buffer at most one char to present seek back operation.

		TextDecoder() :
			luibind(get_global_heap()) {}

		//! using this to read one char from stream.
		result_t read_char(char& ch)
		{
			if (m_tmp_char >= 0)
			{
				ch = (char)m_tmp_char;
				m_tmp_char = -1;
				return s_ok;
			}
			size_t bytes;
			result_t r = m_stream->read(&ch, sizeof(char), &bytes);
			if (failed(r))
			{
				return r;
			}
			if (bytes != sizeof(char))
			{
				return e_eof;
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
			return s_ok;
		}

		result_t read_chars(IStringBuffer* buf, size_t num_chars_to_read)
		{
			// Must be in one line.
			if (m_tmp_char >= 0)
			{
				char ch = (char)m_tmp_char;
				m_tmp_char = -1;
				buf->push_back(ch);
				--num_chars_to_read;
			}
			size_t bytes;
			size_t begin_pos = buf->size();
			buf->resize(buf->size() + num_chars_to_read);
			result_t r = m_stream->read(buf->data() + begin_pos, sizeof(char) * num_chars_to_read, &bytes);
			if (failed(r))
			{
				return r;
			}
			if (bytes != sizeof(char) * num_chars_to_read)
			{
				return e_eof;
			}
			m_idx += (uint32)num_chars_to_read;
			return s_ok;
		}

		result_t read_char_and_report_eof(char& ch)
		{
			result_t r = read_char(ch);
			if (r == e_eof)
			{
				auto _ = log_error("IDecoder::decode - Line %u Character %u : Unexpected EOF got.", m_line, m_idx);
			}
			return r;
		}
		void seek_back_one(char ch)
		{
			m_tmp_char = ch;
		}

		//! Skips any null characters, including space character, tab character, return character
		//! and comments. The cursor points to the next non-null character after this operation.
		result_t skip_null();

		//bool is_first_identifier_char(char ch)
		//{
		//	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_');
		//}

		bool is_identifier_char(char ch)
		{
			return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_');
		}

		result_t decode_string_literal(IStringBuffer* to);

		//! Decodes a sequence as a identifier, which contains 0-9, a-z, A-Z and '_', started with
		//! non-number.
		result_t decode_identifier();

		virtual RP<IVariant> decode(IStream* source_stream) override;

		RP<IVariant> decode_variant_object();
		RP<IVariant> decode_demension_and_create_var(EVariantType t, bool& no_branket);
		result_t decode_variant_object_value(IVariant* var, bool no_branket);
		result_t decode_single_data_value(IVariant* var, size_t index);
		result_t decode_integer_value(IVariant* var, size_t index);
		result_t decode_float_value(IVariant* var, size_t index);
		result_t decode_boolean_value(IVariant* var, size_t index);
		result_t decode_string_value(IVariant* var, size_t index);
		result_t decode_blob_value(IVariant* var, size_t index);
		result_t decode_table_value(IVariant* var, size_t index);
		result_t decode_table_entry(IVariant* var, size_t index);
		result_t decode_variant_value(IVariant* var, size_t index);
	};
}