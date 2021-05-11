// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Error.cpp
* @author JXMaster
* @date 2020/11/24
*/
#include <Runtime/PlatformDefines.hpp>

#define LUNA_RUNTIME_API LUNA_EXPORT
#include <Runtime/Error.hpp>
#include <Runtime/HashMap.hpp>
#include "OS.hpp"
namespace Luna
{
	handle_t g_error_mtx;
	handle_t g_error_tls;
	Unconstructed<HashMap<errcode_t, c8*>> g_errcode_registry;
	Unconstructed<HashMap<errtype_t, c8*>> g_errtype_registry;

	void error_destructor(void* cookie)
	{
		Error* err = (Error*)cookie;
		memdelete(err);
	}

	void error_init()
	{
		g_error_mtx = OS::new_mutex();
		g_errtype_registry.construct();
		g_errcode_registry.construct();
		g_error_tls = OS::tls_alloc(error_destructor).get();
	}

	void error_close()
	{
		OS::tls_free(g_error_tls);
		for (auto& i : g_errcode_registry.get())
		{
			memfree(i.second);
		}
		for (auto& i : g_errtype_registry.get())
		{
			memfree(i.second);
		}
		g_errcode_registry.destruct();
		g_errcode_registry.destruct();
		OS::delete_mutex(g_error_mtx);
		g_error_mtx = nullptr;
	}

	LUNA_RUNTIME_API errcode_t get_error_code_by_name(const c8* errcode_name)
	{
		lucheck(errcode_name);
		usize l = strlen(errcode_name);
		usize h = strhash(errcode_name);
		auto& r = g_errcode_registry.get();
		OS::lock_mutex(g_error_mtx);
		auto iter = r.find(h);
		if (iter == r.end())
		{
			c8* str = (c8*)memalloc((l + 1) * sizeof(c8));
			memcpy(str, errcode_name, (l + 1) * sizeof(c8));
			r.insert(make_pair(h, str));
		}
		OS::unlock_mutex(g_error_mtx);
		return h;
	}

	LUNA_RUNTIME_API errcode_t get_error_type_by_name(const c8* errtype_name)
	{
		lucheck(errtype_name);
		usize l = strlen(errtype_name);
		usize h = strhash(errtype_name);
		auto& r = g_errtype_registry.get();
		OS::lock_mutex(g_error_mtx);
		auto iter = r.find(h);
		if (iter == r.end())
		{
			c8* str = (c8*)memalloc((l + 1) * sizeof(c8));
			memcpy(str, errtype_name, (l + 1) * sizeof(c8));
			r.insert(make_pair(h, str));
		}
		OS::unlock_mutex(g_error_mtx);
		return h;
	}

	LUNA_RUNTIME_API const c8* get_errcode_name(errcode_t err_code)
	{
		OS::lock_mutex(g_error_mtx);
		auto iter = g_errcode_registry.get().find(err_code);
		if (iter == g_errcode_registry.get().end())
		{
			OS::unlock_mutex(g_error_mtx);
			return u8"";
		}
		OS::unlock_mutex(g_error_mtx);
		return iter->second;
	}

	LUNA_RUNTIME_API const c8* get_errtype_name(errtype_t err_category)
	{
		OS::lock_mutex(g_error_mtx);
		auto iter = g_errtype_registry.get().find(err_category);
		if (iter == g_errtype_registry.get().end())
		{
			OS::unlock_mutex(g_error_mtx);
			return u8"";
		}
		OS::unlock_mutex(g_error_mtx);
		return iter->second;
	}

	LUNA_RUNTIME_API Error& get_error_object()
	{
		Error* err = (Error*)OS::tls_get(g_error_tls);
		if (!err)
		{
			err = memnew<Error>();
			luassert_always(succeeded(OS::tls_set(g_error_tls, err)));
		}
		return *err;
	}

	namespace BasicError
	{
		LUNA_RUNTIME_API errtype_t errtype()
		{
			static errtype_t e = get_error_type_by_name(u8"BasicError");
			return e;
		}
		LUNA_RUNTIME_API errcode_t failure()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::failure");
			return e;
		}
		LUNA_RUNTIME_API errcode_t error_object()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::error_object");
			return e;
		}
		LUNA_RUNTIME_API errcode_t not_found()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::not_found");
			return e;
		}
		LUNA_RUNTIME_API errcode_t already_exists()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::already_exists");
			return e;
		}
		LUNA_RUNTIME_API errcode_t bad_arguments()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::bad_arguments");
			return e;
		}
		LUNA_RUNTIME_API errcode_t bad_calling_time()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::bad_calling_time");
			return e;
		}
		LUNA_RUNTIME_API errcode_t bad_memory_alloc()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::bad_memory_alloc");
			return e;
		}
		LUNA_RUNTIME_API errcode_t not_supported()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::not_supported");
			return e;
		}
		LUNA_RUNTIME_API errcode_t bad_system_call()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::bad_system_call");
			return e;
		}
		LUNA_RUNTIME_API errcode_t access_denied()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::access_denied");
			return e;
		}
		LUNA_RUNTIME_API errcode_t not_directory()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::not_directory");
			return e;
		}
		LUNA_RUNTIME_API errcode_t timeout()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::timeout");
			return e;
		}
		LUNA_RUNTIME_API errcode_t data_too_long()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::data_too_long");
			return e;
		}
		LUNA_RUNTIME_API errcode_t insufficient_buffer()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::insufficient_buffer");
			return e;
		}
		LUNA_RUNTIME_API errcode_t busy()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::busy");
			return e;
		}
		LUNA_RUNTIME_API errcode_t out_of_range()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::out_of_range");
			return e;
		}
		LUNA_RUNTIME_API errcode_t overflow()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::overflow");
			return e;
		}
		LUNA_RUNTIME_API errcode_t syntax_error()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::syntax_error");
			return e;
		}
		LUNA_RUNTIME_API errcode_t user_canceled()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::user_canceled");
			return e;
		}
		LUNA_RUNTIME_API errcode_t end_of_file()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::end_of_file");
			return e;
		}
		LUNA_RUNTIME_API errcode_t null_value()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::null_value");
			return e;
		}
		LUNA_RUNTIME_API errcode_t bad_cast()
		{
			static errcode_t e = get_error_code_by_name(u8"BasicError::bad_cast");
			return e;
		}
	}
}
