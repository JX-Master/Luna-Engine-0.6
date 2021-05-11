// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file LexicalAnalyzer.hpp
* @author JXMaster
* @date 2021/4/20
* @brief The lexical analyzer API for the Luna Programming Language.
*/
#pragma once
#include "Base.hpp"
#include "Vector.hpp"
#include "String.hpp"
#include "Assert.hpp"
#include "Result.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	enum class LexemeTokenType : u32
	{
		// Keywords are recognized as identifiers. It is the parser's duty to separate  
		// keywords from normal identifiers.
		identifier = 1,
		// Only the following characters will be recognized as symbols: 
		// + - * / % = ( ) [ ] { } < > & | ! ~ ^ ? : ; , .
		// Only single character symbol will be recognized. It is the parser's duty to compose 
		// multiple character operators (like "&&" or ">>").
		symbol = 2,
		integer_literal = 3,
		floating_point_literal = 4,
		character_literal = 5,
		string_literal = 6,
		boolean_literal = 7,
		null_literal = 8,
	};

	//! Represents one single lexeme token.
	struct LexemeToken
	{
		LexemeTokenType type;
		// The beginning line of this token (start from 1).
		u32 line;
		// The position of the beginning of this token in the line (start from 1).
		u32 position;
		union
		{
			String identifier;
			c8 symbol;
			i64 integer_literal;
			f64 floating_point_literal;
			c32 character_literal;
			String string_literal;
			bool boolean_literal;
		};
		//! It the type is `identifier` and this flag is `true`, then the parser shall not 
		//! interpret this identifier as one keyword. This is set when back-quotes are 
		//! used to enclose the identifier in source code.
		bool not_keyword = false;
		//! Identifier or string.
		LexemeToken(LexemeTokenType type, const String& str) : type(type)
		{
			lucheck(type == LexemeTokenType::identifier || type == LexemeTokenType::string_literal);
			new (&identifier) String(str);
		}
		//! Symbol.
		LexemeToken(c8 symbol) : 
			type(LexemeTokenType::symbol),
			symbol(symbol) {}
		//! Integer literal.
		LexemeToken(i64 integer_literal) :
			type(LexemeTokenType::integer_literal),
			integer_literal(integer_literal) {}
		//! Floating point literal.
		LexemeToken(f64 floating_point_literal) :
			type(LexemeTokenType::floating_point_literal),
			floating_point_literal(floating_point_literal) {}
		//! Character literal.
		LexemeToken(c32 character_literal) :
			type(LexemeTokenType::character_literal),
			character_literal(character_literal) {}
		//! Boolean literal.
		LexemeToken(bool boolean_literal) :
			type(LexemeTokenType::boolean_literal),
			boolean_literal(boolean_literal) {}
		//! Null literal.
		LexemeToken() :
			type(LexemeTokenType::null_literal) {}
		~LexemeToken()
		{
			if (type == LexemeTokenType::string_literal || type == LexemeTokenType::identifier)
			{
				identifier.~String();
			}
		}
		LexemeToken(const LexemeToken& rhs) :
			type(rhs.type)
		{
			memcpy(this, &rhs, sizeof(LexemeToken));
			if (type == LexemeTokenType::string_literal || type == LexemeTokenType::identifier)
			{
				new (&identifier) String(rhs.identifier);
			}
		}
		LexemeToken(LexemeToken&& rhs) :
			type(rhs.type)
		{
			memcpy(this, &rhs, sizeof(LexemeToken));
			if (type == LexemeTokenType::string_literal || type == LexemeTokenType::identifier)
			{
				new (&identifier) String(move(rhs.identifier));
			}
		}
		LexemeToken& operator=(const LexemeToken& rhs)
		{
			if (type == LexemeTokenType::string_literal || type == LexemeTokenType::identifier)
			{
				identifier.~String();
			}
			type = rhs.type;
			memcpy(this, &rhs, sizeof(LexemeToken));
			if (type == LexemeTokenType::string_literal || type == LexemeTokenType::identifier)
			{
				new (&identifier) String(rhs.identifier);
			}
			return *this;
		}
		LexemeToken& operator=(LexemeToken&& rhs)
		{
			if (type == LexemeTokenType::string_literal || type == LexemeTokenType::identifier)
			{
				identifier.~String();
			}
			type = rhs.type;
			memcpy(this, &rhs, sizeof(LexemeToken));
			if (type == LexemeTokenType::string_literal || type == LexemeTokenType::identifier)
			{
				new (&identifier) String(move(rhs.identifier));
			}
			return *this;
		}
	};

	struct LexicalAnalyzer;

	//! Creates a new lexical analyzer.
	//! @return Returns the new created lexical analyzer.
	LUNA_RUNTIME_API LexicalAnalyzer* new_lexical_analyzer();

	//! Destroys the lexical analyzer.
	//! @param[in] analyzer The lexical analyzer to destroy.
	LUNA_RUNTIME_API void delete_lexical_analyzer(LexicalAnalyzer* analyzer);

	//! Sets the source string for the analyzer. This call always resets the analyzer cursor to the first character
	//! of the source string.
	//! @param[in] analyzer The lexical analyzer to operate on.
	//! @param[in] source The source string to set. The analyzing stops when it found
	//! the first non-source character in the string (usually null-terminator), or when the source size
	//! limit is reached. The user should keep the source string valid and unchanged so long as the analyzer 
	//! is working on it.
	//! @param[in] source_size The length of the source string. Specify a big enough value to disable the 
	//! source size limit.
	LUNA_RUNTIME_API void lexical_analyzer_set_source_string(LexicalAnalyzer* analyzer, const c8* source_string, usize source_size = usize_max);

	//! Checks if the analyzer cursor has reached the end of the source string, so that no more token can be fetched.
	//! @param[in] analyzer The lexical analyzer to operate on.
	//! @return Returns `true` if the analyzer cursor has reaches the end of the source string. This call always return
	//! `true` if no source string is set by `lexical_analyzer_set_source_string`.
	LUNA_RUNTIME_API bool lexical_analyzer_is_eof(LexicalAnalyzer* analyzer);

	//! Performs one single-step lexical analyzing using the analyzer and returns the token extracted from the source string.
	//! This call advances the analyzer cursor to the next unanalyzed source character in the source string.
	//! @param[in] analyzer The lexical analyzer to operate on.
	//! @return Returns the next token analyzed from the source string, or one error code on error.
	//! The error code can be:
	//! * BasicError::end_of_file If all tokens in the source string is fetched (`lexical_analyzer_is_eof` returns `true`).
	//! * BasicError::syntax_error If syntax errors are detected in the source string. To see extended description for the syntax error,
	//! use `lexical_analyzer_get_error_message`.
	//! @remark The syntax error is unrecoverable. If this function fails with BasicError::syntax_error, the user must reset the analyzer before any further 
	//! lexical analyzing can be performed. Before the analyzer is reset, any further lexical analyzing call fails with BasicError::syntax_error.
	LUNA_RUNTIME_API R<LexemeToken> lexical_analyzer_step(LexicalAnalyzer* analyzer);

	//! Performs lexical analyzing using the analyzer all the way down until end of file is reached, and returns all tokens extracted from 
	//! the source string. If no syntax error is detected, `lexical_analyzer_is_eof` returns `true` after this call is finished.
	//! @param[in] analyzer The lexical analyzer to operate on.
	//! @return Returns all tokens analyzed from the source string, or one error code on error.
	//! The error code can be:
	//! * BasicError::end_of_file If all tokens in the source string is fetched when this function is called(`lexical_analyzer_is_eof` returns `true`).
	//! * BasicError::syntax_error If syntax errors are detected in the source string. To see extended description for the syntax error,
	//! use `lexical_analyzer_get_error_message`.
	//! @remark The syntax error is unrecoverable. If this function fails with BasicError::syntax_error, the user must reset the analyzer before any further 
	//! lexical analyzing can be performed. Before the analyzer is reset, any further lexical analyzing call fails with BasicError::syntax_error.
	LUNA_RUNTIME_API R<Vector<LexemeToken>> lexical_analyzer_run(LexicalAnalyzer* analyzer);

	//! Gets the error message stored in the lexical analyzer.
	//! @param[in] analyzer The lexical analyzer to use.
	//! @return Returns the error message stored in the lexical analyzer. Note that the message
	//! is meaningful only if last `lexical_analyze()` call fails.
	LUNA_RUNTIME_API const String& lexical_analyzer_get_error_message(LexicalAnalyzer* analyzer);
}