// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Name.cpp
* @author JXMaster
* @date 2020/8/8
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "Name.hpp"
#include "../Name.hpp"
#include "../HashMap.hpp"
#include "OS.hpp"
namespace Luna
{
	struct NameEntry
	{
		c8* m_str;
		u32 m_ref_count;

		NameEntry(c8* str, u32 ref_count) :
			m_str(str),
			m_ref_count(ref_count) {}
	};

	Unconstructed<HashMap<u64, NameEntry>> g_name_map;
	handle_t g_name_mtx;

	void name_init()
	{
		g_name_mtx = OS::new_mutex();
		g_name_map.construct();
	}
	void name_close()
	{
		OS::delete_mutex(g_name_mtx);
		g_name_mtx = nullptr;
		g_name_map.destruct();
	}
	LUNA_RUNTIME_API const c8* intern_name(const c8* name)
	{
		if (!name || (*name == '0')) return nullptr;
		u64 h = strhash64(name);
		OS::lock_mutex(g_name_mtx);
		auto iter = g_name_map.get().find(h);
		if (iter != g_name_map.get().end())
		{
			++iter->second.m_ref_count;
			OS::unlock_mutex(g_name_mtx);
			return iter->second.m_str;
		}
		else
		{
			// Create new entry.
			usize len = strlen(name);
			u64* h_buf = (u64*)OS::memalloc(sizeof(c8) * (len + 1) + sizeof(u64));
			c8* buf = (c8*)(h_buf + 1);
			*h_buf = h;
			strcpy(buf, name);
			g_name_map.get().insert(make_pair(h, NameEntry(buf, 1)));
			OS::unlock_mutex(g_name_mtx);
			return buf;
		}
	}
	LUNA_RUNTIME_API const c8* intern_name(const c8* name, usize count)
	{
		if (!name || (*name == '0')) return nullptr;
		u64 h = memhash64(name, count);
		OS::lock_mutex(g_name_mtx);
		auto iter = g_name_map.get().find(h);
		if (iter != g_name_map.get().end())
		{
			++iter->second.m_ref_count;
			OS::unlock_mutex(g_name_mtx);
			return iter->second.m_str;
		}
		else
		{
			// Create new entry.
			u64* h_buf = (u64*)OS::memalloc(sizeof(c8) * (count + 1) + sizeof(u64));
			c8* buf = (c8*)(h_buf + 1);
			*h_buf = h;
			memcpy(buf, name, sizeof(c8) * count);
			buf[count] = 0;
			g_name_map.get().insert(make_pair(h, NameEntry(buf, 1)));
			OS::unlock_mutex(g_name_mtx);
			return buf;
		}
	}
	LUNA_RUNTIME_API u64 get_name_id(const c8* name)
	{
		if (!name)
		{
			return 0;
		}
		return *(((u64*)name) - 1);
	}
	LUNA_RUNTIME_API const c8* fetch_name(u64 id)
	{
		if (!id) return nullptr;
		OS::lock_mutex(g_name_mtx);
		auto iter = g_name_map.get().find(id);
		if (iter == g_name_map.get().end())
		{
			OS::unlock_mutex(g_name_mtx);
			return nullptr;
		}
		OS::unlock_mutex(g_name_mtx);
		return iter->second.m_str;
	}
	LUNA_RUNTIME_API void retain_name(const c8* name)
	{
		if (!name) return;
		OS::lock_mutex(g_name_mtx);
		auto iter = g_name_map.get().find(get_name_id(name));
		if (iter == g_name_map.get().end())
		{
			OS::unlock_mutex(g_name_mtx);
			return;
		}
		++iter->second.m_ref_count;
		OS::unlock_mutex(g_name_mtx);
	}
	LUNA_RUNTIME_API void release_name(const c8* name)
	{
		if (!name) return;
		OS::lock_mutex(g_name_mtx);
		auto iter = g_name_map.get().find(get_name_id(name));
		if (iter == g_name_map.get().end())
		{
			OS::unlock_mutex(g_name_mtx);
			return;
		}
		--iter->second.m_ref_count;
		if (!iter->second.m_ref_count)
		{
			// Release entry.
			OS::memfree(((u64*)(iter->second.m_str)) - 1);
			iter->second.m_str = nullptr;
			g_name_map.get().erase(iter);
		}
		OS::unlock_mutex(g_name_mtx);
	}
}