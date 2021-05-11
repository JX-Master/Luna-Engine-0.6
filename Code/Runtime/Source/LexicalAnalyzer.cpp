// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file LexicalAnalyzer.cpp
* @author JXMaster
* @date 2021/4/20
* @brief The lexical analyzer API for the Luna Programming Language.
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_RUNTIME_API LUNA_EXPORT
#include <Runtime/LexicalAnalyzer.hpp>
#include <Runtime/Unicode.hpp>
#include <Runtime/Assert.hpp>

namespace Luna
{
	struct LineCounter
	{
		usize cursor;
		Vector<u32> chars;
		u32 line;
		u32 pos;

		void reset()
		{
			chars.clear();
			cursor = 0;
			line = 1;
			pos = 1;
		}
		LineCounter()
		{
			reset();
		}
		void advance(c32 c)
		{
			++cursor;
			if (c == '\n')
			{
				chars.push_back(pos);
				pos = 1;
				++line;
			}
			else
			{
				++pos;
			}
		}
		void back(c32 c)
		{
			--cursor;
			if (c == '\n')
			{
				pos = chars.back();
				chars.pop_back();
				--line;
			}
			else
			{
				--pos;
			}
		}
	};

	struct LexicalAnalyzer
	{
		const c8* source;
		const c8* cur;
		usize src_length;
		String error_message;
		LineCounter counter;
		bool syntax_error;

		LexicalAnalyzer() :
			source(nullptr),
			src_length(0),
			cur(nullptr),
			syntax_error(false) {}
		c32 extract_source_character();
		void advance_to_next_character(c32 current_char, bool no_line_break = false)
		{
			counter.advance(current_char);
			cur += utf8_charspan(current_char);
		}
		void back_to_last_character(c32 last_char)
		{
			counter.back(last_char);
			cur -= utf8_charspan(last_char);
		}
		void set_error(u32 line, u32 pos, const c8* fmt, ...);

		void reset(const c8* source_string, usize source_size);
		bool is_eof();
		R<LexemeToken> step();
		R<Vector<LexemeToken>> run();
		void skip_single_line_comment();
		void skip_multi_line_comment();
		R<LexemeToken> process_number();
		R<LexemeToken> process_string();
		R<LexemeToken> process_character();
		LexemeToken process_symbol();
		R<LexemeToken> process_identifier();
	};

	inline bool is_whitespace(c32 ch)
	{
		return (ch == 0x20) || (ch == 0xA0) || (ch == 0x0A) || (ch == 0x0D);
	}

	const c8 symbols[] = "+-*/%=()[]{}<>&|!~^?:;,.";

	inline bool is_symbol(c32 ch)
	{
		for (c8 i : symbols)
		{
			if ((c32)i == ch)
			{
				return true;
			}
		}
		return false;
	}

	inline bool is_identifier_head(c32 ch)
	{
		if ((ch >= 0x0041 && ch <= 0x005A) || (ch == 0x005F) ||
			(ch >= 0x0061 && ch <= 0x007A) ||
			(ch >= 0x00A1 && ch <= 0xD7FF) ||
			(ch >= 0xE000 && ch <= 0xFDCF) ||
			(ch >= 0xFDF0 && ch <= 0xFFFD) ||
			(ch >= 0x10000 && ch <= 0x1FFFD) ||
			(ch >= 0x20000 && ch <= 0x2FFFD) ||
			(ch >= 0x30000 && ch <= 0x3FFFD) ||
			(ch >= 0x40000 && ch <= 0x4FFFD) ||
			(ch >= 0x50000 && ch <= 0x5FFFD) ||
			(ch >= 0x60000 && ch <= 0x6FFFD) ||
			(ch >= 0x70000 && ch <= 0x7FFFD) ||
			(ch >= 0x80000 && ch <= 0x8FFFD) ||
			(ch >= 0x90000 && ch <= 0x9FFFD) ||
			(ch >= 0xA0000 && ch <= 0xAFFFD) ||
			(ch >= 0xB0000 && ch <= 0xBFFFD) ||
			(ch >= 0xC0000 && ch <= 0xCFFFD) ||
			(ch >= 0xD0000 && ch <= 0xDFFFD) ||
			(ch >= 0xE0000 && ch <= 0xEFFFD) ||
			(ch >= 0xF0000 && ch <= 0xFFFFD) ||
			(ch >= 0x100000 && ch <= 0x10FFFD))
		{
			return true;
		}
		return false;
	}

	inline bool is_identifier_character(c32 ch)
	{
		return is_identifier_head(ch) || (ch >= 0x0030 && ch <= 0x0039);
	}

	c32 LexicalAnalyzer::extract_source_character()
	{
		if (!cur || counter.cursor >= src_length)
		{
			return 0;
		}
		c32 ch = utf8_decode_char(cur);
		if (ch == 0x09 || ch == 0x0A || ch == 0x0D ||
			(ch >= 0x0020 && ch <= 0x007E) ||
			(ch >= 0x00A0 && ch <= 0xD7FF) ||
			(ch >= 0xE000 && ch <= 0xFDCF) ||
			(ch >= 0xFDF0 && ch <= 0xFFFD) ||
			(ch >= 0x10000 && ch <= 0x1FFFD) ||
			(ch >= 0x20000 && ch <= 0x2FFFD) ||
			(ch >= 0x30000 && ch <= 0x3FFFD) ||
			(ch >= 0x40000 && ch <= 0x4FFFD) ||
			(ch >= 0x50000 && ch <= 0x5FFFD) ||
			(ch >= 0x60000 && ch <= 0x6FFFD) ||
			(ch >= 0x70000 && ch <= 0x7FFFD) ||
			(ch >= 0x80000 && ch <= 0x8FFFD) ||
			(ch >= 0x90000 && ch <= 0x9FFFD) ||
			(ch >= 0xA0000 && ch <= 0xAFFFD) ||
			(ch >= 0xB0000 && ch <= 0xBFFFD) ||
			(ch >= 0xC0000 && ch <= 0xCFFFD) ||
			(ch >= 0xD0000 && ch <= 0xDFFFD) ||
			(ch >= 0xE0000 && ch <= 0xEFFFD) ||
			(ch >= 0xF0000 && ch <= 0xFFFFD) ||
			(ch >= 0x100000 && ch <= 0x10FFFD))
		{
			return ch;
		}
		else
		{
			return 0;
		}
	}

	void LexicalAnalyzer::set_error(u32 line, u32 pos, const c8* fmt, ...)
	{
		c8 buf[1024];
		i32 c = sprintf_s(buf, "Line: %d, Pos: %d - ", line, pos);
		VarList args;
		va_start(args, fmt);
		vsnprintf(&buf[c], 1024 - c, fmt, args);
		va_end(args);
		error_message = buf;
	}

	void LexicalAnalyzer::reset(const c8* source_string, usize source_size)
	{
		lucheck(source_string);
		source = source_string;
		cur = source;
		counter.reset();
		src_length = source_size;
		syntax_error = false;
	}

	bool LexicalAnalyzer::is_eof()
	{
		return extract_source_character() == 0;
	}

	R<LexemeToken> LexicalAnalyzer::step()
	{
		if (syntax_error)
		{
			return BasicError::syntax_error();
		}
		c32 ch = extract_source_character();
		while (ch)
		{
			// Match whitespace.
			if (is_whitespace(ch))
			{
				// Skip whitespace.
				advance_to_next_character(ch);
				ch = extract_source_character();
				continue;
			}
			// Match comments.
			if (ch == '/')
			{
				advance_to_next_character(ch);
				ch = extract_source_character();
				if (ch == '/')
				{
					// Single-line comment.
					advance_to_next_character(ch);
					skip_single_line_comment();
					ch = extract_source_character();
					continue;
				}
				else if (ch == '*')
				{
					// Multi-line comment.
					advance_to_next_character(ch);
					skip_multi_line_comment();
					ch = extract_source_character();
					continue;
				}
				else
				{
					// Failed to match.
					back_to_last_character('/');
					ch = '/';
				}
			}
			// Match numbers.
			if (ch >= '0' && ch <= '9')
			{
				R<LexemeToken> r = process_number();
				if (!r.valid())
				{
					if (r.errcode() == BasicError::syntax_error())
					{
						syntax_error = true;
					}
				}
				return r;
			}
			// Match string.
			if (ch == '"')
			{
				R<LexemeToken> r = process_string();
				if (!r.valid())
				{
					if (r.errcode() == BasicError::syntax_error())
					{
						syntax_error = true;
					}
				}
				return r;
			}
			// Match character.
			if (ch == '\'')
			{
				R<LexemeToken> r = process_character();
				if (!r.valid())
				{
					if (r.errcode() == BasicError::syntax_error())
					{
						syntax_error = true;
					}
				}
				return r;
			}
			// Match symbols.
			if (is_symbol(ch))
			{
				return process_symbol();
			}
			// Match identifiers.
			if (ch == '`' || is_identifier_head(ch))
			{
				R<LexemeToken> r = process_identifier();
				if (!r.valid())
				{
					if (r.errcode() == BasicError::syntax_error())
					{
						syntax_error = true;
					}
				}
				return r;
			}
			// No match is a syntax error.
			set_error(counter.line, counter.pos, "Unrecognized token: %c(0x%0x).", (c8)ch, (u32)ch);
			syntax_error = true;
			return BasicError::syntax_error();
		}
		return BasicError::end_of_file();
	}

	R<Vector<LexemeToken>> LexicalAnalyzer::run()
	{
		Vector<LexemeToken> tokens;
		while (!is_eof())
		{
			auto r = step();
			if (!r.valid())
			{
				return r.errcode();
			}
			tokens.push_back(move(r.get()));
		}
		return tokens;
	}

	void LexicalAnalyzer::skip_single_line_comment()
	{
		c32 ch = extract_source_character();
		if (!ch) return;
		while (ch != '\n')
		{
			advance_to_next_character(ch);
			ch = extract_source_character();
			if (!ch) return;
		}
		advance_to_next_character(ch);
	}

	void LexicalAnalyzer::skip_multi_line_comment()
	{
		c32 ch = extract_source_character();
		if (!ch) return;
	entry:
		while (ch != '*')
		{
			advance_to_next_character(ch);
			ch = extract_source_character();
			if (!ch) return;
		}
		advance_to_next_character(ch);
		ch = extract_source_character();
		if (!ch) return;
		if (ch == '/')
		{
			advance_to_next_character(ch);
			return;
		}
		else
		{
			goto entry;
		}
	}

	enum class NumberFormat
	{
		binary = 1,
		octal = 2,
		decimal = 3,
		hexadecimal = 4
	};

	R<LexemeToken> LexicalAnalyzer::process_number()
	{
		u32 line = counter.line;
		u32 position = counter.pos;
		String32 integral;
		String32 decimal;
		String32 exponent;
		bool is_exponent_positive = true;
		bool is_floating_point = false;
		NumberFormat format = NumberFormat::decimal;
		c32 ch = extract_source_character();
		// Check binary, octal or hexadecimal literal.
		if (ch == '0')
		{
			advance_to_next_character(ch);
			ch = extract_source_character();
			if (ch == 'b')
			{
				// Binary.
				format = NumberFormat::binary;
				advance_to_next_character(ch);
			}
			else if (ch == 'o')
			{
				// Octal.
				format = NumberFormat::octal;
				advance_to_next_character(ch);
			}
			else if (ch == 'x')
			{
				// Hexadecimal
				format = NumberFormat::hexadecimal;
				advance_to_next_character(ch);
			}
			else
			{
				back_to_last_character(ch);
			}
			ch = extract_source_character();
		}
		// Process integral part.
		while (ch)
		{
			// Skip "_"
			if (ch == '_')
			{
				advance_to_next_character(ch);
				ch = extract_source_character();
				continue;
			}
			if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
			{
				integral.push_back(ch);
				advance_to_next_character(ch);
				ch = extract_source_character();
				continue;
			}
			else
			{
				break;
			}
		}
		// Process decimal part.
		if (ch == '.')
		{
			is_floating_point = true;
			advance_to_next_character(ch);
			ch = extract_source_character();
			while (ch)
			{
				// Skip "_"
				if (ch == '_')
				{
					advance_to_next_character(ch);
					ch = extract_source_character();
					continue;
				}
				if ((format == NumberFormat::decimal && (ch >= '0' && ch <= '9')) || 
					(format == NumberFormat::hexadecimal && ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))))
				{
					decimal.push_back(ch);
					advance_to_next_character(ch);
					ch = extract_source_character();
					continue;
				}
				else
				{
					break;
				}
			}
			// Exponent part.
			if ((format == NumberFormat::decimal && (ch == 'e' || ch == 'E')) ||
				(format == NumberFormat::hexadecimal && (ch == 'p' || ch == 'P')))
			{
				advance_to_next_character(ch);
				ch = extract_source_character();
				if (ch == '+' || ch == '-')
				{
					is_exponent_positive = (ch == '+');
					advance_to_next_character(ch);
					ch = extract_source_character();
				}
				while (ch)
				{
					// Skip "_"
					if (ch == '_')
					{
						advance_to_next_character(ch);
						ch = extract_source_character();
						continue;
					}
					if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
					{
						exponent.push_back(ch);
						advance_to_next_character(ch);
						ch = extract_source_character();
						continue;
					}
					else
					{
						break;
					}
				}
			}
		}
		// Parse.
		if (is_floating_point)
		{
			f64 value = 0.0;
			if (format == NumberFormat::decimal)
			{
				for (auto& i : integral)
				{
					value *= 10;
					value += i - '0';
				}
				f64 decimal_base = 0.1;
				for (auto& i : decimal)
				{
					value += decimal_base * (i - '0');
					decimal_base /= 10;
				}
				i64 exp = 0;
				for (auto& i : exponent)
				{
					exp *= 10;
					exp += i - '0';
				}
				exp = is_exponent_positive ? exp : -exp;
				while (exp > 0)
				{
					value *= 10.0;
					exp--;
				}
				while (exp < 0)
				{
					value *= 0.1;
					exp++;
				}
			}
			else if (format == NumberFormat::hexadecimal)
			{
				for (auto& i : integral)
				{
					value *= 16.0;
					if (i >= '0' && i <= '9')
					{
						value += i - '0';
					}
					else if (i >= 'a' && i <= 'f')
					{
						value += i - 'a' + 10;
					}
					else
					{
						value += i - 'A' + 10;
					}
				}
				f64 decimal_base = 0.0625;
				for (auto& i : decimal)
				{
					if (i >= '0' && i <= '9')
					{
						value += decimal_base * (i - '0');

					}
					else if (i >= 'a' && i <= 'f')
					{
						value += decimal_base * (i - 'a' + 10);
					}
					else
					{
						value += decimal_base * (i - 'A' + 10);
					}
					decimal_base *= 0.0625;
				}
				i64 exp = 0;
				for (auto& i : exponent)
				{
					exp <<= 4;
					if (i >= '0' && i <= '9')
					{
						exp += i - '0';
					}
					else if (i >= 'a' && i <= 'f')
					{
						exp += i - 'a' + 10;
					}
					else
					{
						exp += i - 'A' + 10;
					}
				}
				exp = is_exponent_positive ? exp : -exp;
				while (exp > 0)
				{
					value *= 2.0;
					exp--;
				}
				while (exp < 0)
				{
					value *= 0.5;
					exp++;
				}
			}
			else
			{
				set_error(line, position, "Floating-point literals cannot begin with \"0b\" and \"0o\".");
				return BasicError::syntax_error();
			}
			LexemeToken t(value);
			t.line = line;
			t.position = position;
			return t;
		}
		else
		{
			i64 value = 0;
			if (format == NumberFormat::decimal)
			{
				for (auto i : integral)
				{
					value *= 10;
					value += i - '0';
				}
			}
			else if (format == NumberFormat::hexadecimal)
			{
				for (auto i : integral)
				{
					value <<= 4;
					if (i >= '0' && i <= '9')
					{
						value += i - '0';
					}
					else if (i >= 'a' && i <= 'f')
					{
						value += i - 'a' + 10;
					}
					else
					{
						value += i - 'A' + 10;
					}
				}
			}
			else if (format == NumberFormat::binary)
			{
				for (auto i : integral)
				{
					value <<= 1;
					value += i - '0';
				}
			}
			else
			{
				for (auto i : integral)
				{
					value <<= 3;
					value += i - '0';
				}
			}
			LexemeToken t(value);
			t.line = line;
			t.position = position;
			return t;
		}
	}

	R<LexemeToken> LexicalAnalyzer::process_string()
	{
		String s;
		u32 line = counter.line;
		u32 pos = counter.pos;
		advance_to_next_character('"');
		c32 ch = extract_source_character();
		while (ch)
		{
			if (ch == '\\')
			{
				advance_to_next_character(ch);
				ch = extract_source_character();
				c32 ch2;
				switch (ch)
				{
				case '0':
					ch2 = '\0'; break;
				case '\\':
					ch2 = '\\'; break;
				case 't':
					ch2 = '\t'; break;
				case 'n':
					ch2 = '\n'; break;
				case 'r':
					ch2 = '\r'; break;
				case '"':
					ch2 = '\"'; break;
				case '\'':
					ch2 = '\''; break;
				case 'u':
				{
					advance_to_next_character(ch);
					ch = extract_source_character();
					set_error(counter.line, counter.pos, "\"{\" expected to enclose one Unicode number.");
					if (ch != '{') return BasicError::syntax_error();
					advance_to_next_character(ch);
					ch = extract_source_character();
					u32 unicode_i = 0;
					while (ch != '}')
					{
						if (!(ch >= '0' && ch <= '9'))
						{
							set_error(counter.line, counter.pos, "Invalid Unicode number.");
							return BasicError::syntax_error();
						}
						unicode_i += ch - '0';
						unicode_i *= 10;
						advance_to_next_character(ch);
						ch = extract_source_character();
					}
					ch2 = (c32)unicode_i;
				}
					break;
				default:
					set_error(counter.line, counter.pos, "Invalid character appeared after \"\\\".");
					return BasicError::syntax_error();
				}
				c8 buf[6];
				usize buf_count = utf8_encode_char(buf, ch2);
				for (usize i = 0; i < buf_count; ++i)
				{
					s.push_back(buf[i]);
				}
				advance_to_next_character(ch);
				ch = extract_source_character();
				continue;
			}
			if (ch == '"')
			{
				advance_to_next_character(ch);
				break;
			}
			c8 buf[6];
			usize buf_count = utf8_encode_char(buf, ch);
			for (usize i = 0; i < buf_count; ++i)
			{
				s.push_back(buf[i]);
			}
			advance_to_next_character(ch);
			ch = extract_source_character();
		}
		LexemeToken t(LexemeTokenType::string_literal, s);
		t.line = line;
		t.position = pos;
		return t;
	}

	R<LexemeToken> LexicalAnalyzer::process_character()
	{
		u32 line = counter.line;
		u32 pos = counter.pos;
		advance_to_next_character('\'');
		c32 ch = extract_source_character();
		c32 final_ch;
		if (ch == '\\')
		{
			advance_to_next_character(ch);
			ch = extract_source_character();
			switch (ch)
			{
			case '0':
				final_ch = '\0'; break;
			case '\\':
				final_ch = '\\'; break;
			case 't':
				final_ch = '\t'; break;
			case 'n':
				final_ch = '\n'; break;
			case 'r':
				final_ch = '\r'; break;
			case '"':
				final_ch = '\"'; break;
			case '\'':
				final_ch = '\''; break;
			case 'u':
			{
				advance_to_next_character(ch);
				ch = extract_source_character();
				set_error(counter.line, counter.pos, "\"{\" expected to enclose one Unicode number.");
				if (ch != '{') return BasicError::syntax_error();
				advance_to_next_character(ch);
				ch = extract_source_character();
				u32 unicode_i = 0;
				while (ch != '}')
				{
					if (!((ch >= '0' && ch <= '9') ||
						(ch >= 'a' && ch <= 'f') ||
						(ch >= 'A' && ch <= 'F')))
					{
						set_error(counter.line, counter.pos, "Invalid Unicode number.");
						return BasicError::syntax_error();
					}
					unicode_i <<= 4;
					if (ch >= '0' && ch <= '9')
					{
						unicode_i += ch - '0';
					}
					else if (ch >= 'a' && ch <= 'f')
					{
						unicode_i += ch - 'a' + 10;
					}
					else
					{
						unicode_i += ch - 'A' + 10;
					}
					advance_to_next_character(ch);
					ch = extract_source_character();
				}
				final_ch = (c32)unicode_i;
			}
			break;
			default:
				set_error(counter.line, counter.pos, "Invalid character appeared after \"\\\".");
				return BasicError::syntax_error();
			}
			advance_to_next_character(ch);
			ch = extract_source_character();
		}
		else
		{
			final_ch = ch;
			advance_to_next_character(ch);
			ch = extract_source_character();
		}
		if (ch != '\'')
		{
			return BasicError::syntax_error();
		}
		advance_to_next_character(ch);
		LexemeToken t(final_ch);
		t.line = line;
		t.position = pos;
		return t;
	}

	LexemeToken LexicalAnalyzer::process_symbol()
	{
		c32 ch = extract_source_character();
		LexemeToken t((c8)ch);
		t.line = counter.line;
		t.position = counter.pos;
		advance_to_next_character(ch);
		return t;
	}

	R<LexemeToken> LexicalAnalyzer::process_identifier()
	{
		String identifier;
		u32 line = counter.line;
		u32 pos = counter.pos;
		bool not_keyword = false;
		c32 ch = extract_source_character();
		if (ch == '`')
		{
			not_keyword = true;
			advance_to_next_character(ch);
			ch = extract_source_character();
		}
		while (is_identifier_character(ch))
		{
			c8 buf[6];
			usize buf_count = utf8_encode_char(buf, ch);
			for (usize i = 0; i < buf_count; ++i)
			{
				identifier.push_back(buf[i]);
			}
			advance_to_next_character(ch);
			ch = extract_source_character();
		}
		if (not_keyword)
		{
			if (ch != '`')
			{
				set_error(counter.line, counter.pos, "\"`\" must be properly closed by another \"`\".");
				return BasicError::syntax_error();
			}
			advance_to_next_character(ch);
		}

		if (!identifier.compare(u8"true"))
		{
			LexemeToken t(true);
			t.line = line;
			t.position = pos;
			t.not_keyword = not_keyword;
			return t;
		}
		else if (!identifier.compare("false"))
		{
			LexemeToken t(false);
			t.line = line;
			t.position = pos;
			t.not_keyword = not_keyword;
			return t;
		}
		else if (!identifier.compare("null"))
		{
			LexemeToken t;
			t.line = line;
			t.position = pos;
			t.not_keyword = not_keyword;
			return t;
		}
		else
		{
			LexemeToken t(LexemeTokenType::identifier, identifier);
			t.line = line;
			t.position = pos;
			t.not_keyword = not_keyword;
			return t;
		}
	}

	LUNA_RUNTIME_API LexicalAnalyzer* new_lexical_analyzer()
	{
		return memnew<LexicalAnalyzer>();
	}

	LUNA_RUNTIME_API void delete_lexical_analyzer(LexicalAnalyzer* analyzer)
	{
		memdelete(analyzer);
	}

	LUNA_RUNTIME_API void lexical_analyzer_set_source_string(LexicalAnalyzer* analyzer, const c8* source_string, usize source_size)
	{
		analyzer->reset(source_string, source_size);
	}

	LUNA_RUNTIME_API bool lexical_analyzer_is_eof(LexicalAnalyzer* analyzer)
	{
		return analyzer->is_eof();
	}

	LUNA_RUNTIME_API R<LexemeToken> lexical_analyzer_step(LexicalAnalyzer* analyzer)
	{
		return analyzer->step();
	}

	LUNA_RUNTIME_API R<Vector<LexemeToken>> lexical_analyzer_run(LexicalAnalyzer* analyzer)
	{
		return analyzer->run();
	}

	LUNA_RUNTIME_API const String& lexical_analyzer_get_error_message(LexicalAnalyzer* analyzer)
	{
		return analyzer->error_message;
	}
}