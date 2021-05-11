// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Error.hpp
* @author JXMaster
* @date 2021/4/19
*/
#pragma once

#include "Base.hpp"
#include "String.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	//! The error code type represents one single error. In Luna Runtime, every
	//! error is represented by one error code, the code value will be determined 
	//! when the error is firstly accessed, and will never be changed.
	//! 
	//! Any valid error code number will not be 0.
	using errcode_t = usize;

	//! The error category type represents one container that can hold multiple error
	//! codes and sub-categories. Like the error code, the category value will be 
	//! determined when the error category is firstly accessed, and will never be 
	//! changed.
	//! 
	//! Any valid error type number will not be 0.
	using errtype_t = usize;

	//! Gets the error code represented by the error name.
	//! @param[in] errcode_name The full name of the error code. For example, to represent
	//! the error "bad_system_call" defined in "BasicError", using "BasicError::bad_system_call".
	//! @return Returns the error code of the corresponding error name. The return value will never be 
	//! 0, if the error code name is required for the first time, the system creates one new registry 
	//! for the error code, and returns one valid error code number that represents to the error.
	LUNA_RUNTIME_API errcode_t get_error_code_by_name(const c8* errcode_name);

	//! Gets the error category represented by the error name.
	//! @param[in] errtype_name The full name of the error category. For example, to represent
	//! the error category "IOError" defined in "NetworkError", using "NetworkError::IOError".
	//! @return Returns the error category of the corresponding error name. The return value will never be 
	//! 0, if the error category name is required for the first time, the system creates one new registry 
	//! for the error category, and returns one valid error category number that represents to the error category.
	LUNA_RUNTIME_API errtype_t get_error_type_by_name(const c8* errtype_name);

	//! Fetches the name of the error code.
	//! @param[in] err_code The error code value.
	//! @return Returns the name of the error code. This string is valid until the 
	//! runtime is closed. Returns empty string ("") if the error code does not exist.
	LUNA_RUNTIME_API const c8* get_errcode_name(errcode_t err_code);

	//! Fetches the name of the error category.
	//! @param[in] err_category The error category value.
	//! @return Returns the name of the error category. This string is valid until the 
	//! runtime is closed. Returns empty string ("") if the error category does not exist.
	LUNA_RUNTIME_API const c8* get_errtype_name(errtype_t err_category);

	//! Errors defined by Luna Runtime itself. These errors can also be used by user module.
	namespace BasicError
	{
		//! Gets the error category object of `BasicError`.
		LUNA_RUNTIME_API errtype_t errtype();
		//! General failure. 
		//! @remark The user should avoid returning generic failure if it can determine what is going wrong and can report that
		//! since returning generic failure does not provide any information for the caller to handle the error.
		LUNA_RUNTIME_API errcode_t failure();
		//! The error is recorded in the error object of the current thread. The user should call `get_error()` to fetch the error
		//! object to check the real error.
		LUNA_RUNTIME_API errcode_t error_object();
		//! The specified item does not exist.
		LUNA_RUNTIME_API errcode_t not_found();
		//! The specified item already exists.
		LUNA_RUNTIME_API errcode_t already_exists();
		//! Invalid arguments are specified.
		LUNA_RUNTIME_API errcode_t bad_arguments();
		//! The operation is invoked in improper time, like trying to reset the resource when the resource is still using.
		LUNA_RUNTIME_API errcode_t bad_calling_time();
		//! Memory allocation failed.
		LUNA_RUNTIME_API errcode_t bad_memory_alloc();
		//! The required operation is not implemented by the instance/platform/build.
		LUNA_RUNTIME_API errcode_t not_supported();
		//! A call to the underlying operating system is failed and the reason cannot be represented by any other
		//! error code.
		LUNA_RUNTIME_API errcode_t bad_system_call();
		//! The access to the file or resource is denied.
		LUNA_RUNTIME_API errcode_t access_denied();
		//! The specified path is not a directory, or one component of the path prefix of the specified path is not a directory.
		LUNA_RUNTIME_API errcode_t not_directory();
		//! The time limit is reached before the operation finishes.
		LUNA_RUNTIME_API errcode_t timeout();
		//! The provided data or string is too long.
		LUNA_RUNTIME_API errcode_t data_too_long();
		//! The memory buffer provided by the user is not large enough to contain all returned data.
		LUNA_RUNTIME_API errcode_t insufficient_buffer();
		//! The service provider is too busy to handle the request.
		LUNA_RUNTIME_API errcode_t busy();
		//! The value specified by the user is out of the valid range from the value.
		LUNA_RUNTIME_API errcode_t out_of_range();
		//! The container, buffer or number overflows.
		LUNA_RUNTIME_API errcode_t overflow();
		//! Syntax error detected when parsing the scripts or strings.
		LUNA_RUNTIME_API errcode_t syntax_error();
		//! The operation is canceled by user.
		LUNA_RUNTIME_API errcode_t user_canceled();
		//! The end of the file or data stream is reached.
		LUNA_RUNTIME_API errcode_t end_of_file();
		//! One instance is expected in the context, but the value in one nullable type is absent.
		LUNA_RUNTIME_API errcode_t null_value();
		//! The instance provided to the as expression does not conforms to the type provided.
		LUNA_RUNTIME_API errcode_t bad_cast();
	}
	
	//! The error object encapsulates one error code along with one string that describes the error. 
//! This is totally optional, most errors can be fully described using error codes only, but in case 
//! that extended context-related error information is needed, this object can be used to pass the error.
//! 
//! This object is mostly used with `get_error_object` to set the error object for the current thread.
	struct Error
	{
		errcode_t code;
		String message;
		Error() = default;
		Error(errcode_t code) :
			code(code) {}
		Error(errcode_t code, const String& message) :
			code(code),
			message(message) {}
		Error(errcode_t code, const c8* fmt, VarList args) :
			code(code)
		{
			c8 buf[1024];
			vsnprintf(buf, 1024, fmt, args);
			message = buf;
		}
		Error(errcode_t code, const c8* fmt, ...) :
			code(code)
		{
			VarList args;
			va_start(args, fmt);
			c8 buf[1024];
			vsnprintf(buf, 1024, fmt, args);
			message = buf;
			va_end(args);
		}
		Error(const Error& rhs) = default;
		Error(Error&& rhs) = default;
		Error& operator=(const Error& rhs) = default;
		Error& operator=(Error&& rhs) = default;
		void clear()
		{
			code = 0;
			message = "";
		}
	};

	//! Gets the error object of this thread. Every thread will be assigned with one independent error object.
	//! @return Returns the error object of this thread.
	//! @remark Every function that sets the error object must return `BasicError::error_object` to indicate that
	//! the error code is passed through the error object. Every function that receives the error code should also
	//! check the error code to see if it is `BasicError::error_object`, and should check the error object for the 
	//! real error code if the received error code is `BasicError::error_object`. You may use `get_error_code` 
	//! helper function to receive the real error code in any situation.
	LUNA_RUNTIME_API Error& get_error_object();

	//! A helper function to set the current thread's error object, and returns `BasicError::error_object`.
	//! @param[in] code The real error code to return.
	//! @param[in] fmt The formatting rules for the error message.
	//! @return Returns `BasicError::error_object` always.
	//! @remark To use this function, replaces the original `return {errcode};` statement with `return custom_error({errcode}, "Message.")`,
	//! then the system will pass the error using error object and let the user to pass one string message along with the error.
	inline errcode_t custom_error(errcode_t code, const c8* fmt, ...)
	{
		VarList args;
		va_start(args, fmt);
		get_error_object() = Error(code, fmt, args);
		va_end(args);
		return BasicError::error_object();
	}

	//! A helper function to fetch the error code of the current function.
	//! @param[in] code The original error code received from the called function. If this is `BasicError::error_object`, this call
	//! returns the error code stored in the error object of the current thread. If this is not `BasicError::error_object`, this call
	//! returns the error code as is.
	inline errcode_t get_errcode(errcode_t code)
	{
		return (code == BasicError::error_object()) ? get_error_object().code : code;
	}

	//! A helper function to fetch the error message of the current function.
	inline const c8* get_errmsg(errcode_t code)
	{
		return (code == BasicError::error_object()) ? get_error_object().message.c_str() : get_errcode_name(code);
	}
}