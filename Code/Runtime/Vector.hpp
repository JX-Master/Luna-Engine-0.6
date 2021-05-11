// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Vector.hpp
* @author JXMaster
* @date 2018/12/9
*/
#pragma once
#include "Memory.hpp"
#include "Algorithm.hpp"
#include "Iterator.hpp"
#include "Assert.hpp"

namespace Luna
{			
	//! @class Vector
	//! Vector represents a value type that stores a continuous array of elements.
	template <typename _Ty>
	class Vector
	{
	private:
		// -------------------- Begin of ABI compatible part --------------------
		_Ty* m_buffer;			// The memory buffer.
		usize m_size;			// Number of elements in the vector.
		usize m_capacity;		// Number of elements that can be included in the buffer before a reallocation is needed.
		// --------------------  End of ABI compatible part  --------------------

		// Frees all dynamic memory.
		void free_buffer()
		{
			destruct_range(begin(), end());
			if (m_buffer)
			{
				memfree(m_buffer, alignof(_Ty));
				m_buffer = nullptr;
			}
			m_size = 0;
			m_capacity = 0;
		}

	public:

		using value_type = _Ty;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using iterator = pointer;
		using const_iterator = const_pointer;
		using reverse_iterator = ReverseIterator<iterator>;
		using const_reverse_iterator = ReverseIterator<const_iterator>;

		// All functions can be modified without breaking the ABI.
		Vector() :
			m_buffer(nullptr),
			m_size(0),
			m_capacity(0) {}
		Vector(usize count, const value_type& value) :
			m_buffer(nullptr),
			m_size(0),
			m_capacity(0)
		{
			if (count)
			{
				reserve(count);
				fill_construct_range(m_buffer, m_buffer + count, value);
				m_size = count;
			}
		}
		Vector(usize count) :
			m_buffer(nullptr),
			m_size(0),
			m_capacity(0)
		{
			if (count)
			{
				reserve(count);
				default_construct_range(m_buffer, m_buffer + count);
				m_size = count;
			}
		}
		Vector(const Vector& rhs) :
			m_buffer(nullptr),
			m_size(0),
			m_capacity(0)
		{
			if (rhs.m_size)
			{
				reserve(rhs.m_size);
				// Copy the elements directly.
				copy_construct_range(rhs.begin(), rhs.end(), m_buffer);
				m_size = rhs.size();
			}
		}
		Vector(Vector&& rhs) :
			m_buffer(rhs.m_buffer),
			m_size(rhs.m_size),
			m_capacity(rhs.m_capacity)
		{
			rhs.m_buffer = nullptr;
			rhs.m_size = 0;
			rhs.m_capacity = 0;
		}
		Vector(InitializerList<value_type> init) :
			m_buffer(nullptr),
			m_size(0),
			m_capacity(0)
		{
			if (init.size())
			{
				reserve(init.size());
				copy_construct_range(init.begin(), init.end(), m_buffer);
				m_size = init.size();
			}
		}
		Vector& operator=(const Vector& rhs)
		{
			free_buffer();
			reserve(rhs.m_size);
			// Copy the elements directly.
			copy_construct_range(rhs.begin(), rhs.end(), m_buffer);
			m_size = rhs.size();
			return *this;
		}
		Vector& operator=(Vector&& rhs)
		{
			free_buffer();
			Luna::swap(m_buffer, rhs.m_buffer);
			Luna::swap(m_size, rhs.m_size);
			Luna::swap(m_capacity, rhs.m_capacity);
			return *this;
		}
		Vector& operator=(InitializerList<value_type> ilist)
		{
			clear();
			reserve(ilist.size());
			copy_construct_range(ilist.begin(), ilist.end(), m_buffer);
			m_size = ilist.size();
			return *this;
		}
		~Vector()
		{
			free_buffer();
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
				value_type* new_buf = (value_type*)memalloc(new_cap * sizeof(_Ty), alignof(_Ty));
				if (m_buffer)
				{
					copy_relocate_range(begin(), end(), new_buf);
					memfree(m_buffer, alignof(_Ty));
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

		void resize(usize n)
		{
			reserve(n);
			if (n > m_size)
			{
				default_construct_range(m_buffer + m_size, m_buffer + n);
			}
			else if (n < m_size)
			{
				destruct_range(m_buffer + n, m_buffer + m_size);
			}
			m_size = n;
		}
		void resize(usize n, const value_type& v)
		{
			reserve(n);
			if (n > m_size)
			{
				fill_construct_range(m_buffer + m_size, m_buffer + n, v);
			}
			else if (n < m_size)
			{
				destruct_range(m_buffer + n, m_buffer + m_size);
			}
			m_size = n;
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
					value_type* new_buf = (value_type*)memalloc(m_size * sizeof(value_type), alignof(value_type));
					if (m_buffer)
					{
						copy_relocate_range(m_buffer, m_buffer + m_size, new_buf);
						memfree(m_buffer, alignof(value_type));
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
		pointer data()
		{
			return m_buffer;
		}
		const_pointer data() const
		{
			return m_buffer;
		}
		void clear()
		{
			destruct_range(begin(), end());
			m_size = 0;
		}
		void push_back(const value_type& val)
		{
			internal_expand_reserve(size() + 1);
			new (m_buffer + m_size) value_type(val);
			++m_size;
		}
		void push_back(value_type&& val)
		{
			internal_expand_reserve(size() + 1);
			new (m_buffer + m_size) value_type(move(val));
			++m_size;
		}
		void pop_back()
		{
			luassert(!empty());
			destruct(m_buffer + m_size - 1);
			--m_size;
		}
		void assign(usize count, const value_type& value)
		{
			clear();
			reserve(count);
			if (count)
			{
				fill_construct_range(m_buffer, m_buffer + count, value);
			}
			m_size = count;
		}
	private:
		template <typename _Integer>
		void internal_assign_iterator(_Integer count, _Integer val, true_type)
		{
			clear();
			reserve(count);
			fill_construct_range(m_buffer, m_buffer + count, val);
			m_size = count;
		}

		template <typename _InputIt>
		void internal_assign_iterator(_InputIt first, _InputIt last, false_type)
		{
			clear();
			for (auto iter = first; iter != last; ++iter)
			{
				push_back(*iter);
			}
		}
	public:
		template <typename _InputIter>
		void assign(_InputIter first, _InputIter last)
		{
			internal_assign_iterator<_InputIter>(first, last, typename is_integral<_InputIter>::type());
		}
		void assign(InitializerList<value_type> il)
		{
			assign(il.begin(), il.end());
		}
		template <typename _InputIt>
		void assign_n(_InputIt first, usize count)
		{
			clear();
			reserve(count);
			copy_construct_range(first, first + count, m_buffer);
			m_size = count;
		}
		void assign(const value_type* first, const value_type* last)
		{
			usize count = last - first;
			assign_n(first, count);
		}
		iterator insert(const_iterator pos, const value_type& val)
		{
			luassert(((usize)pos >= (usize)m_buffer) && ((usize)pos <= (usize)(m_buffer + m_size)));
			usize index = pos - cbegin();
			internal_expand_reserve(m_size + 1);
			pos = begin() + index;
			if (pos != end())
			{
				move_relocate_range_backward((value_type*)pos, (value_type*)end(), (value_type*)end() + 1);
			}
			new ((void*)(m_buffer + index)) value_type(val);
			++m_size;
			return const_cast<iterator>(pos);
		}
		iterator insert(const_iterator pos, value_type&& val)
		{
			luassert(((usize)pos >= (usize)m_buffer) && ((usize)pos <= (usize)(m_buffer + m_size)));
			usize index = pos - cbegin();
			internal_expand_reserve(m_size + 1);
			pos = begin() + index;
			if (pos != end())
			{
				move_relocate_range_backward((value_type*)pos, (value_type*)end(), (value_type*)end() + 1);
			}
			new ((void*)pos) value_type(move(val));
			++m_size;
			return const_cast<iterator>(pos);
		}

		iterator insert(const_iterator pos, usize count, const value_type& val)
		{
			luassert(((usize)pos >= (usize)m_buffer) && ((usize)pos <= (usize)(m_buffer + m_size)));
			usize index = pos - cbegin();
			internal_expand_reserve(m_size + count);
			pos = begin() + index;
			if (pos != end())
			{
				move_relocate_range_backward((value_type*)pos, (value_type*)end(), (value_type*)end() + count);
			}
			fill_construct_range(m_buffer + index, m_buffer + index + count, val);
			m_size += count;
			return const_cast<iterator>(pos);
		}

	private:
		template <typename _Integer>
		iterator internal_insert_iterator(const_iterator pos, _Integer count, _Integer val, true_type)
		{
			luassert(((usize)pos >= (usize)m_buffer) && ((usize)pos <= (usize)(m_buffer + m_size)));
			usize index = pos - cbegin();
			internal_expand_reserve(m_size + count);
			pos = begin() + index;
			if (pos != end())
			{
				move_relocate_range_backward((value_type*)pos, (value_type*)end(), (value_type*)end() + count);
			}
			fill_construct_range(m_buffer + index, m_buffer + index + count, val);
			m_size += count;
			return const_cast<iterator>(pos);
		}

		template <typename _InputIt>
		iterator internal_insert_iterator(const_iterator pos, _InputIt first, _InputIt last, false_type)
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

	public:
		template <typename _InputIt>
		iterator insert(const_iterator pos, _InputIt first, _InputIt last)
		{
			return internal_insert_iterator<_InputIt>(pos, first, last, typename is_integral<_InputIt>::type());
		}
		iterator insert(const_iterator pos, InitializerList<value_type> il)
		{
			return insert(pos, il.begin(), il.end());
		}
		iterator insert_n(const_iterator pos, const value_type* first, usize count)
		{
			luassert(((usize)pos >= (usize)m_buffer) && ((usize)pos <= (usize)(m_buffer + m_size)));
			usize index = pos - cbegin();
			internal_expand_reserve(count + m_size);
			pos = begin() + index;
			if (pos != end())
			{
				move_relocate_range_backward((value_type*)pos, (value_type*)end(), (value_type*)end() + count);
			}
			copy_construct_range(first, first + count, (value_type*)pos);
			m_size += count;
			return const_cast<iterator>(pos);
		}
		iterator insert(const_iterator pos, const value_type* first, const value_type* last)
		{
			usize count = (last - first);
			return insert_n(pos, first, count);
		}
		iterator erase(const_iterator pos)
		{
			luassert(((usize)pos >= (usize)m_buffer) && ((usize)pos < (usize)(m_buffer + m_size)));
			((value_type*)pos)->~value_type();
			if (pos != (end() - 1))
			{
				move_relocate_range((value_type*)pos + 1, (value_type*)end(), (value_type*)pos);
			}
			--m_size;
			return const_cast<iterator>(pos);
		}
		iterator erase(const_iterator first, const_iterator last)
		{
			luassert(((usize)first >= (usize)m_buffer) && ((usize)first < (usize)(m_buffer + m_size)));
			luassert(((usize)last >= (usize)m_buffer) && ((usize)last <= (usize)(m_buffer + m_size)));
			destruct_range((value_type*)first, (value_type*)last);
			if (last != end())
			{
				move_relocate_range((value_type*)last, (value_type*)end(), (value_type*)first);
			}
			m_size -= (last - first);
			return const_cast<iterator>(first);
		}
		void swap(Vector& rhs)
		{
			Vector tmp(move(*this));
			(*this) = move(rhs);
			rhs = move(tmp);
		}
		template <typename... _Args>
		iterator emplace(const_iterator pos, _Args&&... args)
		{
			luassert(((usize)pos >= (usize)m_buffer) && ((usize)pos <= (usize)(m_buffer + m_size)));
			usize index = pos - cbegin();
			internal_expand_reserve(m_size + 1);
			pos = begin() + index;
			if (pos != end())
			{
				move_relocate_range_backward((value_type*)pos, (value_type*)end(), (value_type*)end() + 1);
			}
			new ((void*)pos) value_type(forward<_Args>(args)...);
			++m_size;
			return const_cast<iterator>(pos);
		}
		template <typename... _Args>
		iterator emplace_back(_Args&&... args)
		{
			internal_expand_reserve(size() + 1);
			new (m_buffer + m_size) value_type(forward<_Args>(args)...);
			++m_size;
			return (m_buffer + m_size - 1);
		}
	};

	// ABI assert.
	static_assert(sizeof(Vector<usize>) == sizeof(usize) * 3, "Vector size does not match.");
}