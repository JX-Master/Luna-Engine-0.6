// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Error.hpp
* @author JXMaster
* @date 2020/2/7
*/
#pragma once
#include "../CoreHeader.hpp"
#include <Base/TSAssert.hpp>

namespace luna
{
	// Insert this to system's thread context.
	struct ThreadErrorContext
	{
		Guid m_domain;
		P<IVariant> m_var;
		result_t m_code;
		char m_msg[1024];

		ThreadErrorContext()
		{
			clear_err();
		}

		void clear_err()
		{
			m_domain = { 0, 0 };
			m_var = nullptr;
			m_code = s_ok;
			m_msg[0] = 0;
		}
		void set_verr(result_t result_code, const char* msgfmt, va_list args)
		{
			luassert_msg_usr((result_code < s_user_success) && (result_code > e_user_failure), \
				"The non-user version of ser_err function only accepts engine error.");
			vsnprintf(m_msg, 1024, msgfmt, args);
			m_domain = err_domain_base;
			m_code = result_code;
			m_var = nullptr;
		}
		void set_verr_var(result_t result_code, IVariant* variant, const char* msgfmt, va_list args)
		{
			luassert_msg_usr((result_code < s_user_success) && (result_code > e_user_failure), \
				"The non-user version of ser_err function only accepts engine error.");
			vsnprintf(m_msg, 1024, msgfmt, args);
			m_domain = err_domain_base;
			m_code = result_code;
			m_var = variant;
		}
		void set_verr_user(const Guid& result_domain, result_t result_code, const char* msgfmt, va_list args)
		{
			vsnprintf(m_msg, 1024, msgfmt, args);
			m_domain = result_domain;
			m_code = result_code;
			m_var = nullptr;
		}
		void set_verr_var_user(const Guid& result_domain, result_t result_code, IVariant* variant, const char* msgfmt, va_list args)
		{
			vsnprintf(m_msg, 1024, msgfmt, args);
			m_domain = result_domain;
			m_code = result_code;
			m_var = variant;
		}
		const char* err_msg()
		{
			return m_msg;
		}
		const Guid& err_domain()
		{
			return m_domain;
		}
		result_t err_result()
		{
			return m_code;
		}
		IVariant* err_var()
		{
			return m_var.get();
		}
	};

	class Error : public IError
	{
	public:
		lucid("{22a540c3-febd-4f84-b6fb-b3b35bf5994c}");
		luiimpl(Error, IError, IObject);
		lutsassert_lock();

		Guid m_domain;
		P<IString> m_msg;
		P<IVariant> m_var;
		result_t m_code;

		Error(IAllocator* alloc) :
			luibind(alloc) {}

		virtual void clear() override
		{
			m_domain = err_domain_base;
			m_msg = nullptr;
			m_code = s_ok;
			m_var = nullptr;
		}

		virtual IString* err_msg() override
		{
			lutsassert();
			return m_msg.get();
		}
		virtual void set_err_msg(IString* msg) override
		{
			lutsassert();
			m_msg = msg;
		}
		virtual const Guid& err_domain() override
		{
			lutsassert();
			return m_domain;
		}
		virtual void set_err_domain(const Guid& domain)
		{
			lutsassert();
			m_domain = domain;
		}
		virtual result_t err_result() override
		{
			lutsassert();
			return m_code;
		}
		virtual void set_err_result(result_t result) override
		{
			lutsassert();
			m_code = result;
		}
		virtual IVariant* err_var() override
		{
			lutsassert();
			return m_var.get();
		}
		virtual void set_err_var(IVariant* variant) override
		{
			lutsassert();
			m_var = variant;
		}
	};

	namespace error
	{
		P<IError> new_err();
		P<IError> promote_err();
		const char* explain(result_t result_code);
		P<IError> err_from_code(result_t result_code);
	}
}