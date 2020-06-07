// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file HashMap.hpp
* @author JXMaster
* @date 2020/2/14
*/
#pragma once
#include "Internal/HashTable.hpp"
#include "IAllocator.hpp"

namespace luna
{
	inline IAllocator* get_module_allocator();

	template <
		typename _Kty,
		typename _Ty,
		typename _Hash = Hash<_Kty>,		// Used to hash the key value.
		typename _KeyEqual = EqualTo<_Kty>>	// Used to compare the element.
	class HashMap
	{
	public:
		using key_type = _Kty;
		using mapped_type = _Ty;
		using value_type = Pair<const _Kty, _Ty>;
		using hasher = _Hash;
		using key_equal = _KeyEqual;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using iterator = hash_table::Iterator<value_type, false>;
		using const_iterator = hash_table::Iterator<value_type, true>;
		using local_iterator = hash_table::BucketIterator<value_type, false>;
		using const_local_iterator = hash_table::BucketIterator<value_type, true>;
		using node_type = hash_table::Node<value_type>;
		using insert_return_type = iterator;

	private:

		using table_type = hash_table::HashTable<key_type, value_type, hash_table::MapExtractKey<key_type, value_type>, hasher, key_equal>;

		table_type m_base;

		HashMap(table_type&& base) :
			m_base(move(base)) {}

	public:

		HashMap(IAllocator* alloc = get_module_allocator()) :
			m_base(alloc) {}
		HashMap(const HashMap& rhs) :
			m_base(rhs.m_base) {}
		HashMap(HashMap&& rhs) :
			m_base(move(rhs.m_base)) {}
		HashMap& operator=(const HashMap&)
		{
			m_base = rhs.m_base;
			return *this;
		}
		HashMap& operator=(HashMap&& rhs)
		{
			m_base = move(rhs.m_base);
			return *this;
		}
	public:

		iterator begin()
		{
			return m_base.begin();
		}
		const_iterator begin() const
		{
			return m_base.begin();
		}
		const_iterator cbegin() const
		{
			return m_base.cbegin();
		}
		iterator end()
		{
			return m_base.end();
		}
		const_iterator end() const
		{
			return m_base.end();
		}
		const_iterator cend() const
		{
			return m_base.cend();
		}

		local_iterator begin(size_t n)
		{
			return m_base.begin(n);
		}
		const_local_iterator begin(size_t n) const
		{
			return m_base.begin(n);
		}
		const_local_iterator cbegin(size_t n) const
		{
			return m_base.cbegin(n);
		}
		local_iterator end(size_t n)
		{
			return m_base.end(n);
		}
		const_local_iterator end(size_t n) const
		{
			return m_base.end(n);
		}
		const_local_iterator cend(size_t n) const
		{
			return m_base.cend(n);
		}
		bool empty() const
		{
			return m_base.empty();
		}
		size_t size() const
		{
			return m_base.size();
		}
		size_t bucket_count() const
		{
			return m_base.bucket_count();
		}
		size_t bucket_size(size_t n) const
		{
			return m_base.bucket_size(n);
		}
		size_t bucket(const key_type& key) const
		{
			return m_base.bucket(key);
		}
		float32 load_factor() const
		{
			return m_base.load_factor();
		}
		float32 max_load_factor() const
		{
			return m_base.max_load_factor();
		}
		void clear()
		{
			m_base.clear();
		}
		hasher hash_function() const
		{
			return m_base.hash_function();
		}
		key_equal key_eq() const
		{
			return m_base.key_eq();
		}

		void rehash(size_t new_buckets_count)
		{
			m_base.rehash(new_buckets_count);
		}
		void reserve(size_t new_cap)
		{
			m_base.reserve(new_cap);
		}
		void max_load_factor(float32 ml)
		{
			m_base.max_load_factor(ml);
		}

		iterator find(const key_type& key)
		{
			return m_base.find(key);
		}

		const_iterator find(const key_type& key) const
		{
			return m_base.find(key);
		}

		bool contains(const key_type& key) const
		{
			return m_base.contains(key);
		}

		Pair<iterator, bool> insert(const value_type& value)
		{
			return m_base.insert(value);
		}

		Pair<iterator, bool> insert(value_type&& value)
		{
			return m_base.insert(move(value));
		}

		insert_return_type insert(node_type&& node)
		{
			return m_base.insert(move(node));
		}

		template <typename _M>
		Pair<iterator, bool> insert_or_assign(const key_type& key, _M&& value)
		{
			return m_base.insert_or_assign<_M>(key, forward<_M>(value));
		}

		template <typename _M>
		Pair<iterator, bool> insert_or_assign(key_type&& key, _M&& value)
		{
			return m_base.insert_or_assign<_M>(move(key), forward<_M>(value));
		}

		template <typename... _Args>
		iterator emplace(_Args&&... args)
		{
			return m_base.emplace(forward<_Args>(args)...);
		}

		iterator erase(const_iterator pos)
		{
			return m_base.erase(pos);
		}

		size_t erase(const key_type& key)
		{
			return m_base.erase(key);
		}

		void swap(HashMap& rhs)
		{
			HashMap tmp(move(rhs));
			rhs = move(*this);
			*this = move(tmp);
		}

		node_type extract(const_iterator pos)
		{
			return m_base.extract(pos);
		}
	};
}