// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Error.cpp
* @author JXMaster
* @date 2020/2/7
*/
#include "Error.hpp"

namespace luna
{
	namespace error
	{

		P<IError> new_err()
		{
			Error* err = new_obj<Error>(get_global_heap());
			return box_ptr<IError>(err);
		}
		P<IError> promote_err()
		{
			Error* err = new_obj<Error>(get_global_heap());
			err->m_code = err_result();
			auto msg = new_string(err_msg());
			err->m_msg = msg.get();
			err->m_var = err_var();
			return box_ptr<IError>(err);
		}
		const char* explain(result_t result_code)
		{
			if (result_code == e_user_failure || result_code == s_user_success)
			{
				return err_msg();
			}
			switch (result_code)
			{
			case s_ok:
				return "The operation succeeded.";
			case s_already_done:
				return "The operation is already done and it does not need to be done a second time.";
			case s_lua_yield:
				return "The lua coroutine is yielded.";
			case e_failure:
				return "The operation failed with no additional information.";
			case e_item_not_exist:
				return "The specified item does not exist.";
			case e_item_already_exists:
				return "The specified item already exists.";
			case e_bad_arguments:
				return "Invalid arguments are specified.";
			case e_bad_calling_time:
				return "The function is called in improper time.";
			case e_bad_memory_alloc:
				return "Memory allocation failed.";
			case e_not_available:
				return "The object does not support the specified operation.";
			case e_bad_system_call:
				return " A call to the underlying operating system is failed and no additional information is provided.";
			case e_file_not_exist:
				return "The specified file or directory does not exist.";
			case e_file_already_exists:
				return "The specified file or directory already exists.";
			case e_access_denied:
				return "The access to the file, directory or resource is denied.";
			case e_dir_not_exist:
				return "The specified directory does not exist.";
			case e_dir_already_exists:
				return "The specified directory already exists.";
			case e_insufficient_buffer:
				return "The memory buffer provided by the user is not large enough to contain all returned elements.";
			case e_pending:
				return "The operation is pending in another thread and is not finished yet.";
			case e_type_dismatch:
				return "The type required to finish this call does not match the actual type the object holds.";
			case e_overflow:
				return "The number or container overflows.";
			case e_interface_not_supported:
				return "The query_interface or cast_interface operation failed because the interface queried is not supported.";
			case e_expired:
				return "The object is expired.";
			case e_lua_errrun:
				return "The operation fails because a runtime error is triggered in Lua state.";
			case e_lua_errerr:
				return "The operation fails because a runtime error is triggered while running the message handler.";
			case e_lua_errgcmm:
				return "The operation fails because a runtime error is triggered while running a lua __gc metamethod.";
			case e_syntax_error:
				return "Syntax error detected when parsing scripts.";
			case e_user_canceled:
				return "The operation is canceled by user.";
			default:
				if (failed(result_code))
				{
					return "Unknown error.";
				}
				break;
			}
			return "Unknown success";
		}

		P<IError> err_from_code(result_t result_code)
		{
			if (result_code == e_user_failure || result_code == s_user_success)
			{
				return promote_err();
			}
			P<IError> err = new_err();
			err->set_err_domain(err_domain_base);
			err->set_err_msg(new_string(explain(result_code)));
			err->set_err_result(result_code);
			return err;
		}

	}
}