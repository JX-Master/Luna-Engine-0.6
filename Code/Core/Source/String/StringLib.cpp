// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file NameSystem.cpp
* @author JXMaster
* @date 2019/11/13
*/
#pragma once
#include "StringLib.hpp"
#include "Name.hpp"
#include "StringBuffer.hpp"
#include "Buffer.hpp"
#include "MemoryStream.hpp"
#include "Path.hpp"
namespace luna
{
	namespace strlib
	{
		Unconstructed<HashMap<uint64, WP<Name>>> m_names;
		P<IReadWriteLock> m_lock;

		P<IName> intern_name(const char* name, size_t len)
		{
			luassert_usr(name);
			WriteGuard lock(m_lock);

			// Hashing string.
			if (len == size_t_max_v)
			{
				len = strlen(name);
			}
			uint64 h = memhash64(name, len * sizeof(char));

			// Check for alive objects.
			auto iter = m_names.get().find(h);
			if (iter != m_names.get().end())
			{
				P<IName> n = iter->second.lock();
				if (n)
				{
					return n;
				}
			}

			// cache miss, add a new object.
			size_t strl = (len + 1) * sizeof(char);
			size_t s = strl + sizeof(Name);
			Name* header = (Name*)get_global_heap()->allocate(s);
			new (header) Name(get_global_heap());
			m_names.get().insert_or_assign(h, header);
			// append the string after the object.
			char* straddr = (char*)(((size_t)header) + sizeof(Name));
			memcpy(straddr, name, len * sizeof(char));
			straddr[len] = 0;
			header->m_len = strl - 1;
			header->m_str = straddr;
			header->m_h = h;
			P<IName> name_ptr;
			name_ptr.attach(header);
			return name_ptr;
		}
		RP<IName> check_name(const char* name, size_t len)
		{
			luassert_usr(name);
			ReadGuard lock(m_lock);

			// Hashing string.
			if (len == size_t_max_v)
			{
				len = strlen(name);
			}
			uint64 h = memhash64(name, len * sizeof(char));
			auto iter = m_names.get().find(h);
			if (iter != m_names.get().end())
			{
				P<IName> n = iter->second.lock();
				if (n)
				{
					return n;
				}
			}
			return e_item_not_exist;
		}

		P<IStringBuffer> new_string_buffer()
		{
			P<IStringBuffer> obj;
			obj.attach(get_global_heap()->new_obj<StringBuffer>(get_global_heap()));
			return obj;
		}
		P<IString> new_string(const char* data, size_t len)
		{
			luassert_usr(data);
			if (len == size_t_max_v)
			{
				len = strlen(data);
			}
			size_t obj_size = sizeof(char) * (len + 1) + sizeof(ImmutableString);
			ImmutableString* obj = (ImmutableString*)get_global_heap()->allocate(obj_size);
			new (obj) ImmutableString(get_global_heap());
			obj->m_str = (char*)((size_t)obj + sizeof(ImmutableString));
			obj->m_size = len;
			memcpy(obj->m_str, data, (len + 1) * sizeof(char));
			return box_ptr<IString>(obj);
		}
		P<IBlob> new_blob(const void* data, size_t len, IAllocator* alloc)
		{
			luassert_usr(data);
			if (!alloc)
			{
				alloc = get_global_heap();
			}
			size_t obj_size = len + sizeof(ImmutableString);
			ImmutableBlob* obj = (ImmutableBlob*)alloc->allocate(obj_size);
			new (obj) ImmutableBlob(alloc);
			obj->m_data = (void*)((size_t)obj + sizeof(ImmutableBlob));
			obj->m_size = len;
			memcpy(obj->m_data, data, len);
			return box_ptr<IBlob>(obj);
		}
		P<IBuffer> new_buffer(size_t buffer_size, size_t alignment, IAllocator* alloc)
		{
			luassert_usr(buffer_size);
			if (!alloc)
			{
				alloc = get_global_heap();
			}
			void* buf;
			if (alignment)
			{
				buf = alloc->allocate_aligned(buffer_size, alignment);
			}
			else
			{
				buf = alloc->allocate(buffer_size);
			}
			Buffer* b = alloc->new_obj<Buffer>(alloc);
			b->m_buffer = buf;
			b->m_size = buffer_size;
			b->m_alignment = alignment;
			b->m_data_size = buffer_size;
			b->m_data_offset = 0;
			return box_ptr<IBuffer>(b);
		}
		P<IBuffer> box_buffer(void* buffer, size_t buffer_size, IAllocator* alloc, size_t alignment)
		{
			luassert_usr(buffer && buffer_size && alloc);
			Buffer* b = alloc->new_obj<Buffer>(alloc);
			b->m_buffer = buffer;
			b->m_size = buffer_size;
			b->m_alignment = alignment;
			b->m_data_offset = 0;
			b->m_data_size = buffer_size;
			return box_ptr<IBuffer>(b);
		}
		P<IMemoryStream> new_memory_stream(size_t initial_buffer_size, size_t alignment, IAllocator* alloc)
		{
			if (!alloc)
			{
				alloc = get_global_heap();
			}
			if (!initial_buffer_size)
			{
				initial_buffer_size = 1024;
			}
			auto buf = strlib::new_buffer(initial_buffer_size, alignment, alloc);
			MemoryStream* stream = new_obj<MemoryStream>(get_global_heap());
			stream->m_buf = buf;
			return box_ptr<IMemoryStream>(stream);
		}
		P<IMemoryStream> new_memory_stream_from_buffer(IBuffer* buffer)
		{
			luassert_usr(buffer);
			MemoryStream* stream = new_obj<MemoryStream>(get_global_heap());
			stream->m_buf = buffer;
			return box_ptr<IMemoryStream>(stream);
		}
		P<IMutPath> new_path(const char* path_string, size_t len)
		{
			Path* newp = new_obj<Path>(get_global_heap());
			if (!path_string)
			{
				newp->set_flags(EPathFlag::none);
			}
			else
			{
				newp->parse(path_string, len);
			}
			return box_ptr<IPath>(newp);
		}
	}
}