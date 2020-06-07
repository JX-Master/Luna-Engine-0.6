// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IError.hpp
* @author JXMaster
* @date 2020/1/8
*/
#pragma once
#include "IString.hpp"
#include "IVariant.hpp"

namespace luna
{
	//! @interface IError
	//! Represents an error object that can be saved or passed to another thread.
	struct IError : public IObject
	{
		luiid("{e6504706-2c6a-4090-9456-9afabb47ee74}");

		//! Clears the error contained in the error object. The state will be reset to:
		//! 1. Domain: err_domain_base.
		//! 2. Result: s_ok.
		//! 3. Msg: `nullptr`.
		//! 4. Var: `nullptr`.
		virtual void clear() = 0;

		virtual IString* err_msg() = 0;
		virtual void set_err_msg(IString* msg) = 0;
		virtual const Guid& err_domain() = 0;
		virtual void set_err_domain(const Guid& domain) = 0;
		virtual result_t err_result() = 0;
		virtual void set_err_result(result_t result) = 0;
		virtual IVariant* err_var() = 0;
		virtual void set_err_var(IVariant* variant) = 0;
	};
}