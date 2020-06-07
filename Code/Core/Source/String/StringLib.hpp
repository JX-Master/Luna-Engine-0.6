// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file StringManager.hpp
* @author JXMaster
* @date 2019/12/5
 */
#pragma once
#include "../CoreHeader.hpp"
#include <Base/HashMap.hpp>
#include "Name.hpp"
namespace luna
{
	class ImmutableString : public IString
	{
	public:
		lucid("{4fb3aa43-48a6-4a52-8b1e-9ae1b4264ade}");
		luiimpl(ImmutableString, IString, IObject);
		

		ImmutableString(IAllocator* alloc) :
			luibind(alloc) {}

		char* m_str;
		size_t m_size;

		virtual const char* c_str() override
		{
			return m_str;
		}

		virtual size_t size() override
		{
			return m_size;
		}
	};

	class ImmutableBlob : public IBlob
	{
	public:
		lucid("{d988cd6c-9e1c-4a06-97fa-3dc5a1aeb759}");
		luiimpl(ImmutableBlob, IBlob, IObject);
		

		ImmutableBlob(IAllocator* alloc) :
			luibind(alloc) {}

		void* m_data;
		size_t m_size;

		virtual const void* data() override
		{
			return m_data;
		}

		virtual size_t size() override
		{
			return m_size;
		}
	};

	namespace strlib
	{
		extern Unconstructed<HashMap<uint64, WP<Name>>> m_names;
		extern P<IReadWriteLock> m_lock;

		inline void init(IAllocator* alloc)
		{
			m_names.construct(alloc);
			m_lock = new_read_write_lock();
		}
		inline void deinit()
		{
			m_lock = nullptr;
			m_names.destruct();
		}

		P<IName> intern_name(const char* name, size_t l = size_t_max_v);
		RP<IName> check_name(const char* name, size_t len = size_t_max_v);
		P<IStringBuffer> new_string_buffer();
		P<IString> new_string(const char* str, size_t len);
		P<IBlob> new_blob(const void* data, size_t len, IAllocator* alloc = nullptr);
		P<IBuffer> new_buffer(size_t buffer_size, size_t alignment, IAllocator* alloc = nullptr);
		P<IBuffer> box_buffer(void* buffer, size_t buffer_size, IAllocator* alloc, size_t alignment = 0);
		P<IMemoryStream> new_memory_stream(size_t initial_buffer_size = 0, size_t alignment = 0, IAllocator* alloc = nullptr);
		P<IMemoryStream> new_memory_stream_from_buffer(IBuffer* buffer);
		P<IMutPath> new_path(const char* path_string, size_t len);
	}
}