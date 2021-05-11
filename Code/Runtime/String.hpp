// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file String.hpp
* @author JXMaster
* @date 2020/7/31
*/
#pragma once
#include "Memory.hpp"
#include "Algorithm.hpp"
#include "Iterator.hpp"

namespace Luna
{
	namespace Impl
	{
		template <typename _Char>
		struct StringTraits
		{

		};

		template <>
		struct StringTraits<c8>
		{
			static constexpr const c8* null_string = u8"";
		};

		template <>
		struct StringTraits<c16>
		{
			static constexpr const c16* null_string = u"";
		};

		template <>
		struct StringTraits<c32>
		{
			static constexpr const c32* null_string = U"";
		};
	}

	//! The string template used by String, String16 and String32.
	template <typename _Char>
	class BasicString
	{
	private:
		// -------------------- Begin of ABI compatible part --------------------
		_Char* m_buffer;		// The memory buffer.
		usize m_size;			// Number of elements in the vector.
		usize m_capacity;		// Number of elements that can be included in the buffer before a reallocation is needed.
		// --------------------  End of ABI compatible part  --------------------

		// Frees all dynamic memory.
		void free_buffer()
		{
			if (m_buffer)
			{
				memfree(m_buffer);
				m_buffer = nullptr;
			}
			m_size = 0;
			m_capacity = 0;
		}

		usize strlength(const _Char* s)
		{
			usize count = 0;
			while (s[count] != (_Char)0) ++count;
			return count;
		}

	public:
		using value_type = _Char;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using iterator = pointer;
		using const_iterator = const_pointer;
		using reverse_iterator = ReverseIterator<iterator>;
		using const_reverse_iterator = ReverseIterator<const_iterator>;

		static constexpr usize npos = (usize)-1;

		BasicString() :
			m_size(0),
			m_capacity(0),
			m_buffer(nullptr) {}
		BasicString(usize count, value_type ch) :
			m_size(count),
			m_capacity(count)
		{
			if (count)
			{
				m_buffer = (value_type*)memalloc(sizeof(value_type) * (count + 1));
				for (value_type* i = m_buffer; i < m_buffer + m_size; ++i)
				{
					*i = ch;
				}
				m_buffer[m_size] = (value_type)0;
			}
			else
			{
				m_buffer = nullptr;
			}
		}
		BasicString(const BasicString& rhs, usize pos, usize count = npos)
		{
			count = (count == npos) ? rhs.size() - pos : count;
			m_size = count;
			m_capacity = count;
			if (count)
			{
				m_buffer = (value_type*)memalloc(sizeof(value_type) * (count + 1));
				memcpy(m_buffer, rhs.c_str() + pos, sizeof(value_type) * count);
				m_buffer[count] = (value_type)0;
			}
			else
			{
				m_buffer = nullptr;
			}
		}
		BasicString(const value_type* s, usize count) :
			m_size(count),
			m_capacity(count)
		{
			if (count)
			{
				m_buffer = (value_type*)memalloc(sizeof(value_type) * (count + 1));
				memcpy(m_buffer, s, sizeof(value_type) * count);
				m_buffer[count] = (value_type)0;
			}
			else
			{
				m_buffer = nullptr;
			}
		}
		BasicString(const value_type* s)
		{
			usize count = strlength(s);
			m_size = count;
			m_capacity = count;
			if (count)
			{
				m_buffer = (value_type*)memalloc(sizeof(value_type) * (count + 1));
				memcpy(m_buffer, s, sizeof(value_type) * count);
				m_buffer[count] = (value_type)0;
			}
			else
			{
				m_buffer = nullptr;
			}
		}
		BasicString(const BasicString& rhs)
		{
			if (!rhs.empty())
			{
				m_size = rhs.m_size;
				m_capacity = rhs.m_size;
				m_buffer = (value_type*)memalloc(sizeof(value_type) * (m_size + 1));
				memcpy(m_buffer, rhs.m_buffer, sizeof(value_type) * (m_size + 1));
			}
			else
			{
				m_size = 0;
				m_capacity = 0;
				m_buffer = nullptr;
			}
		}
		BasicString(BasicString&& rhs) :
			m_buffer(rhs.m_buffer),
			m_size(rhs.m_size),
			m_capacity(rhs.m_capacity)
		{
			rhs.m_buffer = nullptr;
			rhs.m_size = 0;
			rhs.m_capacity = 0;
		}
		BasicString(InitializerList<value_type> ilist) :
			m_size(ilist.size()),
			m_capacity(ilist.size())
		{
			if (m_size)
			{
				m_buffer = (value_type*)memalloc(sizeof(value_type) * (ilist.size() + 1));
				value_type* i = m_buffer;
				for (auto iter = ilist.begin(); iter != ilist.end(); ++iter)
				{
					*i = *iter;
					++i;
				}
				m_buffer[m_size] = 0;
			}
			else
			{
				m_buffer = nullptr;
			}

		}
		BasicString& operator=(const BasicString& rhs)
		{
			free_buffer();
			if (!rhs.empty())
			{
				reserve(rhs.m_size);
				memcpy(m_buffer, rhs.c_str(), (rhs.size() + 1) * sizeof(value_type));
				m_size = rhs.size();
			}
			return *this;
		}
		BasicString& operator=(BasicString&& rhs)
		{
			free_buffer();
			Luna::swap(m_buffer, rhs.m_buffer);
			Luna::swap(m_size, rhs.m_size);
			Luna::swap(m_capacity, rhs.m_capacity);
			return *this;
		}
		BasicString& operator=(const value_type* s)
		{
			clear();
			usize count = strlen(s);
			reserve(count);
			if (count)
			{
				memcpy(m_buffer, s, sizeof(value_type) * count);
				
			}
			m_size = count;
			if (m_buffer)
			{
				m_buffer[m_size] = (value_type)0;
			}
			return *this;
		}
		BasicString& operator=(value_type ch)
		{
			clear();
			reserve(1);
			m_buffer[0] = ch;
			m_size = 1;
			m_buffer[m_size] = (value_type)0;
			return *this;
		}
		BasicString& operator=(InitializerList<value_type> ilist)
		{
			clear();
			reserve(ilist.size());
			if (ilist.size())
			{
				value_type* i = m_buffer;
				for (auto iter = ilist.begin(); iter != ilist.end(); ++iter)
				{
					*i = *iter;
					++i;
				}
			}
			m_size = ilist.size();
			if (m_buffer)
			{
				m_buffer[m_size] = (value_type)0;
			}
			return *this;
		}
		~BasicString()
		{
			free_buffer();
		}

		//! May returns `nullptr` if the string is empty.
		pointer data()
		{
			return m_buffer;
		}

		const_pointer data() const
		{
			return m_buffer;
		}

		//! Unlike `data`, this call always returns a valid string, not `nullptr`, even if the string is empty.
		const_pointer c_str() const
		{
			return m_buffer ? m_buffer : Impl::StringTraits<value_type>::null_string;
		}

		// Returns a pointer to the first element. Can only be `nullptr` if `size` and `capacity` is both 0.
		iterator begin()
		{
			return m_buffer;
		}
		// Returns a pointer to the element next to the last element. Can only be `nullptr` if `size` and `capacity` is both 0.
		iterator end()
		{
			return m_buffer + m_size;
		}
		const_iterator begin() const
		{
			return m_buffer;
		}
		const_iterator end() const
		{
			return m_buffer + m_size;
		}
		const_iterator cbegin() const
		{
			return m_buffer;
		}
		const_iterator cend() const
		{
			return m_buffer + m_size;
		}
		reverse_iterator rbegin()
		{
			return reverse_iterator(end());
		}
		reverse_iterator rend()
		{
			return reverse_iterator(begin());
		}
		const_reverse_iterator rbegin() const
		{
			return const_reverse_iterator(end());
		}
		const_reverse_iterator rend() const
		{
			return const_reverse_iterator(begin());
		}
		const_reverse_iterator crbegin() const
		{
			return const_reverse_iterator(cend());
		}
		const_reverse_iterator crend() const
		{
			return const_reverse_iterator(cbegin());
		}
		usize size() const
		{
			return m_size;
		}
		usize length() const
		{
			return m_size;
		}
		usize capacity() const
		{
			return m_capacity;
		}
		bool empty() const
		{
			return (m_size == 0);
		}
		void reserve(usize new_cap)
		{
			if (new_cap > m_capacity)
			{
				value_type* new_buf = (value_type*)memalloc((new_cap + 1) * sizeof(value_type));
				if (m_buffer)
				{
					copy_relocate_range(begin(), end() + 1, new_buf);

					memfree(m_buffer);
				}
				m_buffer = new_buf;
				m_capacity = new_cap;
			}
		}
	private:
		void internal_expand_reserve(usize new_least_cap)
		{
			if (new_least_cap > m_capacity)
			{
				reserve(max(max(new_least_cap, m_capacity * 2), (usize)4));	// Double the size by default.
			}
		}
	public:
		void resize(usize n, value_type v)
		{
			reserve(n);
			if (n > m_size)
			{
				fill_construct_range(m_buffer + m_size, m_buffer + n, v);
			}
			m_size = n;
			m_buffer[n] = (value_type)0;
		}
		void shrink_to_fit()
		{
			if (m_capacity != m_size)
			{
				if (!m_size)
				{
					free_buffer();
				}
				else
				{
					value_type* new_buf = (value_type*)memalloc(sizeof(value_type) * (m_size + 1), alignof(value_type));
					if (m_buffer)
					{
						memcpy(new_buf, m_buffer, sizeof(value_type) * (m_size + 1));
						memfree(m_buffer);
					}
					m_buffer = new_buf;
					m_capacity = m_size;
				}
			}
		}

		reference operator[] (usize n)
		{
			luassert(n < m_size);
			return m_buffer[n];
		}
		const_reference operator[] (usize n) const
		{
			luassert(n < m_size);
			return m_buffer[n];
		}
		reference at(usize n)
		{
			luassert(n < m_size);
			return m_buffer[n];
		}
		const_reference at(usize n) const
		{
			luassert(n < m_size);
			return m_buffer[n];
		}
		reference front()
		{
			luassert(!empty());
			return m_buffer[0];
		}
		const_reference front() const
		{
			luassert(!empty());
			return m_buffer[0];
		}
		reference back()
		{
			luassert(!empty());
			return m_buffer[m_size - 1];
		}
		const_reference back() const
		{
			luassert(!empty());
			return m_buffer[m_size - 1];
		}
		void clear()
		{
			if (m_buffer)
			{
				m_buffer[0] = (value_type)0;
			}
			m_size = 0;
		}
		void push_back(value_type ch)
		{
			internal_expand_reserve(size() + 1);
			m_buffer[m_size] = ch;
			++m_size;
			m_buffer[m_size] = (value_type)0;
		}
		void pop_back()
		{
			luassert(!empty());
			--m_size;
			m_buffer[m_size] = (value_type)0;
		}
		void assign(usize count, value_type ch)
		{
			clear();
			reserve(count);
			if (count)
			{
				fill_construct_range(m_buffer, m_buffer + count, ch);
				m_buffer[count] = (value_type)0;
			}
			m_size = count;
		}
		void assign(const BasicString& str)
		{
			*this = str;
		}
		void assign(const BasicString& str, usize pos, usize count = npos)
		{
			clear();
			count = (count == npos) ? str.size() - pos : count;
			reserve(count);
			if (count)
			{
				memcpy(m_buffer, str.c_str() + pos, count * sizeof(value_type));
				m_buffer[count] = (value_type)0;
			}
			m_size = count;
		}
		void assign(BasicString&& str)
		{
			free_buffer();
			m_buffer = str.m_buffer;
			m_capacity = str.m_capacity;
			m_size = str.m_size;
			str.m_buffer = nullptr;
			str.m_capacity = 0;
			str.m_size = 0;
		}
		void assign(const value_type* s, usize count)
		{
			clear();
			reserve(count);
			if (count)
			{
				memcpy(m_buffer, s, count * sizeof(value_type));
				m_buffer[count] = (value_type)0;
				m_size = count;
			}
		}
		void assign(const value_type* s)
		{
			usize count = strlength(s);
			clear();
			reserve(count);
			if (count)
			{
				memcpy(m_buffer, s, count * sizeof(value_type));
				m_buffer[count] = (value_type)0;
				m_size = count;
			}
		}
		template <typename _InputIt>
		void assign(_InputIt first, _InputIt last)
		{
			clear();
			for (auto iter = first; iter != last; ++iter)
			{
				push_back(*iter);
			}
		}
		void assign(InitializerList<value_type> il)
		{
			assign(il.begin(), il.end());
		}
		void insert(usize index, usize count, value_type ch)
		{
			luassert(index <= m_size);
			internal_expand_reserve(m_size + count);
			if (index != m_size)
			{
				memmove(m_buffer + index + count, m_buffer + index, sizeof(value_type) * (m_size - index));
			}
			fill_construct_range(m_buffer + index, m_buffer + index + count, ch);
			m_size += count;
			m_buffer[m_size] = (value_type)0;
		}
		void insert(usize index, const value_type* s)
		{
			luassert(index <= m_size);
			usize count = strlength(s);
			internal_expand_reserve(m_size + count);
			if (index != m_size)
			{
				memmove(m_buffer + index + count, m_buffer + index, sizeof(value_type) * (m_size - index));
			}
			memcpy(m_buffer + index, s, sizeof(value_type) * count);
			m_size += count;
			m_buffer[m_size] = (value_type)0;
		}
		void insert(usize index, const value_type* s, usize count)
		{
			luassert(index <= m_size);
			internal_expand_reserve(m_size + count);
			if (index != m_size)
			{
				memmove(m_buffer + index + count, m_buffer + index, sizeof(value_type) * (m_size - index));
			}
			memcpy(m_buffer + index, s, sizeof(value_type) * count);
			m_size += count;
			m_buffer[m_size] = (value_type)0;
		}
		void insert(usize index, const BasicString& str)
		{
			luassert(index <= m_size);
			usize count = str.size();
			internal_expand_reserve(m_size + count);
			if (index != m_size)
			{
				memmove(m_buffer + index + count, m_buffer + index, sizeof(value_type) * (m_size - index));
			}
			memcpy(m_buffer + index, str.c_str(), sizeof(value_type) * count);
			m_size += count;
			m_buffer[m_size] = (value_type)0;
		}
		void insert(usize index, const BasicString& str, usize index_str, usize count)
		{
			luassert(index <= m_size);
			luassert(count <= (str.size() - index_str));
			internal_expand_reserve(m_size + count);
			if (index != m_size)
			{
				memmove(m_buffer + index + count, m_buffer + index, sizeof(value_type) * (m_size - index));
			}
			memcpy(m_buffer + index, str.c_str() + index_str, sizeof(value_type) * count);
			m_size += count;
			m_buffer[m_size] = (value_type)0;
		}
		iterator insert(const_iterator pos, value_type ch)
		{
			luassert(((usize)pos >= (usize)m_buffer) && ((usize)pos <= (usize)(m_buffer + m_size)));
			usize index = pos - cbegin();
			internal_expand_reserve(m_size + 1);
			auto mpos = begin() + index;
			if (mpos != end())
			{
				memmove(m_buffer + index + 1, m_buffer + index, sizeof(value_type) * (m_size - index));
			}
			m_buffer[index] = ch;
			++m_size;
			m_buffer[m_size] = (value_type)0;
			return mpos;
		}
		iterator insert(const_iterator pos, usize count, value_type ch)
		{
			luassert(((usize)pos >= (usize)m_buffer) && ((usize)pos <= (usize)(m_buffer + m_size)));
			usize index = pos - cbegin();
			internal_expand_reserve(m_size + count);
			auto mpos = begin() + index;
			if (mpos != end())
			{
				memmove(m_buffer + index + count, m_buffer + index, sizeof(value_type) * (m_size - index));
			}
			fill_construct_range(m_buffer + index, m_buffer + index + count, ch);
			m_size += count;
			m_buffer[m_size] = (value_type)0;
			return mpos;
		}
		template <typename _InputIt>
		iterator insert(const_iterator pos, _InputIt first, _InputIt last)
		{
			luassert(((usize)pos >= (usize)m_buffer) && ((usize)pos <= (usize)(m_buffer + m_size)));
			usize index = pos - cbegin();
			for (auto iter = first; iter != last; ++iter)
			{
				pos = insert(pos, *iter);
				++pos;
			}
			return begin() + index;
		}
		iterator insert(const_iterator pos, InitializerList<value_type> ilist)
		{
			return insert(pos, ilist.begin(), ilist.end());
		}
		void erase(usize index = 0, usize count = npos)
		{
			count = count == npos ? m_size - index : count;
			luassert(index + count <= m_size);
			if ((index + count) != m_size)
			{
				memmove(m_buffer + index, m_buffer + index + count, sizeof(value_type) * (m_size - index - count));
			}
			m_size -= count;
			m_buffer[m_size] = (value_type)0;
		}
		iterator erase(const_iterator pos)
		{
			luassert(((usize)pos >= (usize)m_buffer) && ((usize)pos < (usize)(m_buffer + m_size)));
			if (pos != (end() - 1))
			{
				move_relocate_range((value_type*)pos + 1, (value_type*)end(), (value_type*)pos);
			}
			--m_size;
			m_buffer[m_size] = (value_type)0;
			return const_cast<iterator>(pos);
		}
		iterator erase(const_iterator first, const_iterator last)
		{
			luassert(((usize)first >= (usize)m_buffer) && ((usize)first < (usize)(m_buffer + m_size)));
			luassert(((usize)last >= (usize)m_buffer) && ((usize)last <= (usize)(m_buffer + m_size)));
			if (last != end())
			{
				move_relocate_range((value_type*)last, (value_type*)end(), (value_type*)first);
			}
			m_size -= (last - first);
			m_buffer[m_size] = (value_type)0;
			return const_cast<iterator>(first);
		}

		void swap(BasicString& rhs)
		{
			BasicString tmp(move(*this));
			(*this) = move(rhs);
			rhs = move(tmp);
		}

		void append(usize count, value_type ch)
		{
			if (count)
			{
				internal_expand_reserve(m_size + count);
				fill_construct_range(m_buffer + m_size, m_buffer + m_size + count, ch);
				m_size += count;
				m_buffer[m_size] = (value_type)0;
			}
		}
		void append(const BasicString& str)
		{
			if (!str.empty())
			{
				internal_expand_reserve(m_size + str.size());
				memcpy(m_buffer + m_size, str.c_str(), str.size() * sizeof(value_type));
				m_size += str.size();
				m_buffer[m_size] = (value_type)0;
			}
		}
		void append(const BasicString& str, usize pos, usize count = npos)
		{
			count = count == npos ? str.size() : count;
			if (count)
			{
				internal_expand_reserve(m_size + count);
				memcpy(m_buffer + m_size, str.c_str() + pos, count * sizeof(value_type));
				m_size += count;
				m_buffer[m_size] = (value_type)0;
			}
		}
		void append(const value_type* s, usize count)
		{
			if (count)
			{
				internal_expand_reserve(m_size + count);
				memcpy(m_buffer + m_size, s, count * sizeof(value_type));
				m_size += count;
				m_buffer[m_size] = (value_type)0;
			}
		}
		void append(const value_type* s)
		{
			usize count = strlength(s);
			if (count)
			{
				internal_expand_reserve(m_size + count);
				memcpy(m_buffer + m_size, s, count * sizeof(value_type));
				m_size += count;
				m_buffer[m_size] = (value_type)0;
			}
		}
		template <typename _InputIt>
		void append(_InputIt first, _InputIt last)
		{
			for (auto iter = first; iter != last; ++iter)
			{
				push_back(*iter);
			}
		}
		void append(InitializerList<value_type> ilist)
		{
			append(ilist.begin(), ilist.end());
		}

		BasicString& operator+=(const BasicString& str)
		{
			append(str);
			return *this;
		}
		BasicString& operator+=(value_type ch)
		{
			push_back(ch);
			return *this;
		}
		BasicString& operator+=(const value_type* s)
		{
			append(s);
			return *this;
		}
		BasicString& operator+=(InitializerList<value_type> ilist)
		{
			append(ilist);
			return *this;
		}

		i32 compare(const BasicString& rhs) const
		{
			return strcmp(c_str(), rhs.c_str());
		}
		i32 compare(usize pos1, usize count1, const BasicString& rhs) const
		{
			count1 = min(count1, size() - pos1);
			return memcmp(c_str() + pos1, rhs.c_str(), count1 * sizeof(value_type));
		}
		i32 compare(usize pos1, usize count1, const BasicString& rhs, usize pos2, usize count2 = npos) const
		{
			count1 = min(count1, size() - pos1);
			count2 = min(count2, size() - pos2);
			return memcmp(c_str() + pos1, rhs.c_str() + pos2, min(count1, count2) * sizeof(value_type));
		}
		i32 compare(const value_type* s) const
		{
			return strcmp(c_str(), s);
		}
		i32 compare(usize pos1, usize count1, const value_type* s) const
		{
			count1 = min(count1, size() - pos1);
			return memcmp(c_str() + pos1, s, count1 * sizeof(value_type));
		}
		i32 compare(usize pos1, usize count1, const value_type* s, usize count2) const
		{
			count1 = min(count1, size() - pos1);
			return memcmp(c_str() + pos1, s, min(count1, count2) * sizeof(value_type));
		}

		void replace(usize pos, usize count, const BasicString& str)
		{
			isize delta = str.size() - count;
			if (delta > 0)
			{
				internal_expand_reserve(m_size + delta);
			}
			memmove(m_buffer + pos + str.size(), m_buffer + pos + count, sizeof(value_type) * (m_size - pos - count));
			memcpy(m_buffer + pos, str.c_str(), sizeof(value_type) * str.size());
			m_size += delta;
			m_buffer[m_size] = (value_type)0;
		}
		void replace(const_iterator first, const_iterator last, const BasicString& str)
		{
			usize pos = first - m_buffer;
			usize count = last - first;
			replace(pos, count, str);
		}
		void replace(usize pos, usize count, const BasicString& str, usize pos2, usize count2 = npos)
		{
			count2 = (pos2 + count2 > str.size()) ? str.size() - pos2 : count2;
			isize delta = count2 - count;
			if (delta > 0)
			{
				internal_expand_reserve(m_size + delta);
			}
			memmove(m_buffer + pos + count2, m_buffer + pos + count, sizeof(value_type) * (m_size - pos - count));
			memcpy(m_buffer + pos, str.c_str() + pos2, sizeof(value_type)* count2);
			m_size += delta;
			m_buffer[m_size] = (value_type)0;
		}
		template <typename _InputIt>
		void replace(const_iterator first, const_iterator last, _InputIt first2, _InputIt last2)
		{
			usize pos = first - m_buffer;
			erase(first, last);
			insert(begin() + pos, first2, last2);
		}
		void replace(usize pos, usize count, const value_type* cstr, usize count2)
		{
			isize delta = count2 - count;
			if (delta > 0)
			{
				internal_expand_reserve(m_size + delta);
			}
			memmove(m_buffer + pos + count2, m_buffer + pos + count, sizeof(value_type) * (m_size - pos - count));
			memcpy(m_buffer + pos, cstr, sizeof(value_type) * count2);
			m_size += delta;
			m_buffer[m_size] = (value_type)0;
		}
		void replace(const_iterator first, const_iterator last, const value_type* cstr, usize count2)
		{
			usize pos = first - m_buffer;
			usize count = last - first;
			replace(pos, count, cstr, count2);
		}
		void replace(usize pos, usize count, const value_type* cstr)
		{
			usize count2 = strlength(cstr);
			replace(pos, count, cstr, count2);
		}
		void replace(const_iterator first, const_iterator last, const value_type* cstr)
		{
			usize pos = first - m_buffer;
			usize count = last - first;
			replace(pos, count, cstr);
		}
		void replace(usize pos, usize count, usize count2, value_type ch)
		{
			isize delta = count2 - count;
			if (delta > 0)
			{
				internal_expand_reserve(m_size + delta);
			}
			memmove(m_buffer + pos + count2, m_buffer + pos + count, sizeof(value_type) * (m_size - pos - count));
			fill_construct_range(m_buffer + pos, m_buffer + pos + count2, ch);
			m_size += delta;
			m_buffer[m_size] = (value_type)0;
		}
		void replace(const_iterator first, const_iterator last, usize count2, value_type ch)
		{
			usize pos = first - m_buffer;
			usize count = last - first;
			replace(pos, count, count2, ch);
		}
		void replace(const_iterator first, const_iterator last, InitializerList<value_type> ilist)
		{
			usize pos = first - m_buffer;
			usize count = last - first;
			isize delta = ilist.size() - count;
			if (delta > 0)
			{
				internal_expand_reserve(m_size + delta);
			}
			memmove(m_buffer + pos + ilist.size(), m_buffer + pos + count, sizeof(value_type) * (m_size - pos - count));
			auto iter = const_cast<iterator>(first);
			for (auto& i : ilist)
			{
				*iter = i;
				++iter;
			}
			m_size += delta;
			m_buffer[m_size] = (value_type)0;
		}
		BasicString substr(usize pos = 0, usize count = npos) const
		{
			luassert(pos <= m_size);
			count = min(count, m_size - pos);
			return BasicString(m_buffer + pos, count, m_alloc);
		}
		usize copy(value_type* dest, usize count, usize pos = 0) const
		{
			luassert(pos <= m_size);
			count = min(count, m_size - pos);
			memcpy(dest, m_buffer + pos, sizeof(value_type) * count);
			return count;
		}
	};

	using String = BasicString<c8>;
	using String16 = BasicString<c16>;
	using String32 = BasicString<c32>;
}