// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Vector.hpp
* @author JXMaster
* @date 2018/12/9
*/
#pragma once
#include "PlatformDefines.hpp"
#include "IAllocator.hpp"
#include "Memory.hpp"
#include "Algorithm.hpp"

namespace luna
{
	inline IAllocator* get_module_allocator();
			
	//! @class Vector
	//! ABI-compatible vector class to present a non-reference-counted dynamic array storage. This class can be passed
	//! by interface, but the std::vector cannot.
	template <typename _Ty>
	class Vector
	{
	private:
		// -------------------- Begin of ABI compatible part --------------------
		P<IAllocator> m_alloc;	// The allocator that manages the memory buffer for this vector.
		_Ty* m_buffer;			// The memory buffer.
		size_t m_size;			// Number of elements in the vector.
		size_t m_capacity;		// Number of elements that can be included in the buffer before a reallocation is needed.
		// --------------------  End of ABI compatible part  --------------------

		// Frees all dynamic memory.
		void free_buffer()
		{
			destruct_range(begin(), end());
			if (m_buffer)
			{
				deallocate_array<_Ty>(m_alloc, m_buffer);
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
		Vector(IAllocator* alloc = get_module_allocator()) :
			m_alloc(alloc),
			m_buffer(nullptr),
			m_size(0),
			m_capacity(0) {}
		Vector(const Vector& rhs) :
			m_alloc(rhs.m_alloc),
			m_buffer(nullptr),
			m_size(0),
			m_capacity(0)
		{
			reserve(rhs.m_size);
			// Copy the elements directly.
			copy_construct_range(rhs.begin(), rhs.end(), m_buffer);
			m_size = rhs.size();
		}
		Vector(Vector&& rhs) :
			m_alloc(rhs.m_alloc),
			m_buffer(rhs.m_buffer),
			m_size(rhs.m_size),
			m_capacity(rhs.m_capacity)
		{
			rhs.m_buffer = nullptr;
			rhs.m_size = 0;
			rhs.m_capacity = 0;
		}
		Vector& operator=(const Vector& rhs)
		{
			free_buffer();
			m_alloc = rhs.m_alloc;
			reserve(rhs.m_size);
			// Copy the elements directly.
			copy_construct_range(rhs.begin(), rhs.end(), m_buffer);
			m_size = rhs.size();
			return *this;
		}
		Vector& operator=(Vector&& rhs)
		{
			free_buffer();
			m_alloc = rhs.m_alloc;
			luna::swap(m_buffer, rhs.m_buffer);
			luna::swap(m_size, rhs.m_size);
			luna::swap(m_capacity, rhs.m_capacity);
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
			return reverse_iterator(end());
		}
		const_reverse_iterator rend() const
		{
			return reverse_iterator(begin());
		}
		const_reverse_iterator crbegin() const
		{
			return reverse_iterator(cend());
		}
		const_reverse_iterator crend() const
		{
			return reverse_iterator(cbegin());
		}
		size_t size() const
		{
			return m_size;
		}
		size_t capacity() const
		{
			return m_capacity;
		}
		bool empty() const
		{
			return (m_size == 0);
		}
		void reserve(size_t new_cap)
		{
			if (new_cap > m_capacity)
			{
				value_type* new_buf = allocate_array<value_type>(m_alloc, new_cap);
				if (m_buffer)
				{
					copy_relocate_range(begin(), end(), new_buf);
					deallocate_array<value_type>(m_alloc, m_buffer);
				}
				m_buffer = new_buf;
				m_capacity = new_cap;
			}
		}

	private:
		void internal_expand_reserve(size_t new_least_cap)
		{
			if (new_least_cap > m_capacity)
			{
				reserve(max(max(new_least_cap, m_capacity * 2), (size_t)4));	// Double the size by default.
			}
		}
	public:

		void resize(size_t n)
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
		void resize(size_t n, const value_type& v)
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
					value_type* new_buf = allocate_array<value_type>(m_alloc, m_size);
					if (m_buffer)
					{
						copy_relocate_range(m_buffer, m_buffer + m_size, new_buf);
						deallocate_array<value_type>(m_alloc, m_buffer);
					}
					m_buffer = new_buf;
					m_capacity = m_size;
				}
			}
		}

		reference operator[] (size_t n)
		{
			luassert(n < m_size);
			return m_buffer[n];
		}
		const_reference operator[] (size_t n) const
		{
			luassert(n < m_size);
			return m_buffer[n];
		}
		reference at(size_t n)
		{
			luassert(n < m_size);
			return m_buffer[n];
		}
		const_reference at(size_t n) const
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
		void assign(size_t count, const value_type& value)
		{
			clear();
			reserve(count);
			fill_construct_range(m_buffer, m_buffer + count, value);
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
		void assign(initializer_list<value_type> il)
		{
			assign(il.begin(), il.end());
		}
		template <typename _InputIt>
		void assign_n(_InputIt first, size_t count)
		{
			clear();
			reserve(count);
			copy_construct_range(first, first + count, m_buffer);
			m_size = count;
		}
		void assign(const value_type* first, const value_type* last)
		{
			size_t count = last - first;
			assign_n(first, count);
		}
		iterator insert(const_iterator pos, const value_type& val)
		{
			luassert(((size_t)pos >= (size_t)m_buffer) && ((size_t)pos <= (size_t)(m_buffer + m_size)));
			size_t index = pos - cbegin();
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
			luassert(((size_t)pos >= (size_t)m_buffer) && ((size_t)pos <= (size_t)(m_buffer + m_size)));
			size_t index = pos - cbegin();
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

		iterator insert(const_iterator pos, size_t count, const value_type& val)
		{
			luassert(((size_t)pos >= (size_t)m_buffer) && ((size_t)pos <= (size_t)(m_buffer + m_size)));
			size_t index = pos - cbegin();
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
			luassert(((size_t)pos >= (size_t)m_buffer) && ((size_t)pos <= (size_t)(m_buffer + m_size)));
			size_t index = pos - cbegin();
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
			luassert(((size_t)pos >= (size_t)m_buffer) && ((size_t)pos <= (size_t)(m_buffer + m_size)));
			size_t index = pos - cbegin();
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
		iterator insert(const_iterator pos, initializer_list<value_type> il)
		{
			return insert(pos, il.begin(), il.end());
		}
		iterator insert_n(const_iterator pos, const value_type* first, size_t count)
		{
			luassert(((size_t)pos >= (size_t)m_buffer) && ((size_t)pos <= (size_t)(m_buffer + m_size)));
			size_t index = pos - cbegin();
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
			size_t count = (last - first);
			return insert_n(pos, first, count);
		}
		iterator erase(const_iterator pos)
		{
			luassert(((size_t)pos >= (size_t)m_buffer) && ((size_t)pos <= (size_t)(m_buffer + m_size)));
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
			luassert(((size_t)first >= (size_t)m_buffer) && ((size_t)first <= (size_t)(m_buffer + m_size)));
			luassert(((size_t)last >= (size_t)m_buffer) && ((size_t)last <= (size_t)(m_buffer + m_size)));
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
			luassert(((size_t)pos >= (size_t)m_buffer) && ((size_t)pos <= (size_t)(m_buffer + m_size)));
			size_t index = pos - cbegin();
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
		IAllocator* get_allocator() const
		{
			return m_alloc.get();
		}
	};

	// ABI assert.
	static_assert(sizeof(Vector<size_t>) == sizeof(size_t) * 4, "Vector size does not match.");
}