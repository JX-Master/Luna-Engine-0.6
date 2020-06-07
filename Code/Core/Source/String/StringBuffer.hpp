// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file StringBuffer.hpp
* @author JXMaster
* @date 2019/12/5
 */
#pragma once
#include "../CoreHeader.hpp"
#include <Base/TSAssert.hpp>
#include <string>
namespace luna
{
	class StringBuffer : public IStringBuffer
	{
	public:
		lucid("{fbc7a522-2422-4aec-b3e6-50f868b52ee6}");
		luqbegin();
		luqitem(this, StringBuffer, IStringBuffer, ICloneable);
		luqitem((IString*)this, IString, IObject);
		luqend();
		lurc();
		lutsassert_lock();

		std::string m_str;

		StringBuffer(IAllocator* alloc) :
			luibind(alloc),
			m_str() {}

		virtual P<IObject> clone() override
		{
			lutsassert();
			StringBuffer* buf = m_rc.m_alloc->new_obj<StringBuffer>(m_rc.m_alloc);
			P<IStringBuffer> obj;
			obj.attach(buf);
			obj->assign(m_str.data());
			return obj;
		}
		virtual void assign(const char* s, size_t len = size_t_max_v) override
		{
			lutsassert();
			if (len != size_t_max_v)
			{
				m_str.assign(s, len);
			}
			else
			{
				m_str.assign(s, strlen(s));
			}
		}

		virtual char* data() override
		{
			lutsassert();
			return const_cast<char*>(m_str.data());
		}

		virtual const char* c_str() override
		{
			lutsassert();
			return (m_str.c_str());
		}

		virtual size_t size() override
		{
			lutsassert();
			return m_str.size();
		}

		virtual bool empty() override
		{
			lutsassert();
			return m_str.empty();
		}
		virtual void reserve(size_t new_cap) override
		{
			lutsassert();
			m_str.reserve(new_cap);
		}
		virtual size_t capacity() override
		{
			lutsassert();
			return m_str.capacity();
		}
		virtual void shrink_to_fit() override
		{
			lutsassert();
			m_str.shrink_to_fit();
		}
		virtual void clear() override
		{
			lutsassert();
			m_str.clear();
		}
		virtual void insert(size_t pos, const char* s, size_t len = size_t_max_v) override
		{
			lutsassert();
			luassert_usr(pos <= size());
			if (len == size_t_max_v)
			{
				m_str.insert(pos, s, strlen(s));
			}
			else
			{
				m_str.insert(pos, s, len);
			}
		}
		virtual void insert_char(size_t pos, size_t count, char ch) override
		{
			lutsassert();
			luassert_usr(pos <= size());
			m_str.insert(pos, count, ch);
		}
		virtual void erase(size_t pos, size_t count) override
		{
			lutsassert();
			luassert_msg_usr(pos < size(), "Failed to erase: `pos` is greater than or equal to `size()`.");
			if (pos + count >= size())
			{
				m_str.erase(pos, std::string::npos);
			}
			else
			{
				m_str.erase(pos, count);
			}
		}
		virtual void push_back(char ch) override
		{
			lutsassert();
			m_str.push_back(ch);
		}
		virtual void pop_back() override
		{
			lutsassert();
			luassert_msg_usr(!empty(), "Trying to call `pop_back()` on an empty string buffer.");
			m_str.pop_back();
		}
		virtual void append(const char* s, size_t len = size_t_max_v) override
		{
			lutsassert();
			if (len == size_t_max_v)
			{
				m_str.append(s);
			}
			else
			{
				m_str.append(s, len);
			}
		}
		virtual void append_char(size_t count, char ch) override
		{
			lutsassert();
			m_str.append(count, ch);
		}
		virtual void replace(size_t pos, size_t count, const char* s, size_t len = size_t_max_v) override
		{
			lutsassert();
			luassert_usr((pos + count) < size());

			if (len == size_t_max_v)
			{
				m_str.replace(pos, count, s);
			}
			else
			{
				m_str.replace(pos, count, s, len);
			}
		}
		virtual void replace_char(size_t pos, size_t count, size_t count2, char ch) override
		{
			lutsassert();
			luassert_usr((pos + count) < size());
			m_str.replace(pos, count, count2, ch);
		}
		virtual P<IStringBuffer> substr(size_t pos, size_t count = size_t_max_v) override
		{
			lutsassert();
			StringBuffer* obj = m_rc.m_alloc->new_obj<StringBuffer>(m_rc.m_alloc);
			if (count == size_t_max_v)
			{
				obj->m_str = m_str.substr(pos);
			}
			else
			{
				obj->m_str = m_str.substr(pos, count);
			}
			return box_ptr<IStringBuffer>(obj);
		}

		virtual void resize(size_t count, char ch)
		{
			lutsassert();
			m_str.resize(count, ch);
		}

		virtual void swap(IStringBuffer* rhs) override
		{
			lutsassert();
			luassert_usr(rhs);
			StringBuffer* s = (StringBuffer*)rhs;
			auto str = luna::move(m_str);
			m_str = luna::move(s->m_str);
			s->m_str = luna::move(str);
		}
	};
}