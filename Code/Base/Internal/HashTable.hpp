// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file HashTable.hpp
* @author JXMaster
* @date 2020/2/15
* @brief Defines common types used by both hash set and hash map.
*/
#pragma once
#include "../BaseDefines.hpp"
#include "../Functional.hpp"

namespace luna
{
	namespace hash_table
	{
		template <typename _Ty>
		struct Node
		{
			using value_type = _Ty;

			value_type m_value;
			Node* m_next;

			Node() = default;
			explicit Node(const value_type& value) :
				m_value(value) {}
			explicit Node(value_type&& value) :
				m_value(move(value)) {}

			Node(const Node&) = default;
			Node(Node&& rhs) :
				m_next(rhs.m_next),
				m_value(move(rhs.m_value)) {}
			Node& operator=(const Node&) = default;
			Node& operator=(Node&& rhs)
			{
				m_next = rhs.m_next;
				m_value = move(rhs.m_value);
				return *this;
			}
		};

		template <typename _Ty>
		struct BucketIteratorBase
		{
			Node<_Ty>* m_current_node;

			void increment()
			{
				m_current_node = m_current_node->m_next;
			}

			bool operator==(const BucketIteratorBase& rhs)
			{
				return (m_current_node == rhs.m_current_node);
			}
			bool operator!=(const BucketIteratorBase& rhs)
			{
				return !(*this == rhs);
			}
		};

		template <typename _Ty, bool _Const>
		struct BucketIterator
		{
			BucketIteratorBase<_Ty> m_base;

			using node_type = Node<_Ty>;
			using value_type = typename Node<_Ty>::value_type;
			using pointer = conditional_t<_Const, const value_type*, value_type*>;
			using reference = conditional_t<_Const, const value_type&, value_type&>;

			explicit BucketIterator(Node<_Ty>* cur)
			{
				m_base.m_current_node = cur;
			}

			BucketIterator(const BucketIterator<_Ty, false>& rhs)
			{
				m_base = rhs.m_base;
			}

			reference operator*() const
			{
				return m_base.m_current_node->m_value;
			}

			pointer operator->() const
			{
				return &(m_base.m_current_node->m_value);
			}

			BucketIterator& operator++()
			{
				m_base.increment();
				return *this;
			}

			BucketIterator operator++(int)
			{
				BucketIterator temp(*this);
				m_base.increment();
				return temp;
			}

			bool operator==(const BucketIterator& rhs)
			{
				return m_base == rhs.m_base;
			}

			bool operator!=(const BucketIterator& rhs)
			{
				return m_base != rhs.m_base;
			}
		};

		template <typename _Ty>
		struct IteratorBase
		{
			Node<_Ty>* m_current_node;
			Node<_Ty>** m_current_bucket;

			void increment()
			{
				m_current_node = m_current_node->m_next;
				while (m_current_node == nullptr)
				{
					++m_current_bucket;
					m_current_node = *m_current_bucket;
				}
			}

			void increment_bucket()
			{
				++m_current_bucket;
				while (*m_current_bucket == nullptr)
				{
					++m_current_bucket;
				}
				m_current_node = *m_current_bucket;	// The last bucket is used as the end placeholder, which stores size_t_max_v.
			}

			bool operator==(const IteratorBase& rhs)
			{
				return (m_current_node == rhs.m_current_node) && (m_current_bucket == rhs.m_current_bucket);
			}
			bool operator!=(const IteratorBase& rhs)
			{
				return !(*this == rhs);
			}
		};

		template <typename _Ty, bool _Const>
		struct Iterator
		{
			IteratorBase<_Ty> m_base;

			using node_type = Node<_Ty>;
			using value_type = typename Node<_Ty>::value_type;
			using pointer = conditional_t<_Const, const value_type*, value_type*>;
			using reference = conditional_t<_Const, const value_type&, value_type&>;

			Iterator(node_type* node, node_type** bucket)
			{
				m_base.m_current_node = node;
				m_base.m_current_bucket = bucket;
			}

			Iterator(node_type** bucket)
			{
				m_base.m_current_bucket = bucket;
				m_base.m_current_node = *bucket;
			}

			Iterator(const Iterator<_Ty, false>& rhs)
			{
				m_base = rhs.m_base;
			}

			reference operator*() const
			{
				return m_base.m_current_node->m_value;
			}

			pointer operator->() const
			{
				return &(m_base.m_current_node->m_value);
			}

			Iterator& operator++()
			{
				m_base.increment();
				return *this;
			}

			Iterator operator++(int)
			{
				Iterator temp(*this);
				m_base.increment();
				return temp;
			}

			bool operator==(const Iterator& rhs)
			{
				return m_base == rhs.m_base;
			}

			bool operator!=(const Iterator& rhs)
			{
				return m_base != rhs.m_base;
			}
		};

		// Extract key reference from value reference.

		template <typename _Kty, typename _Vty>
		struct MapExtractKey
		{
			const _Kty& operator()(const _Vty& p) const
			{
				return p.first;
			}
		};

		template <typename _Kty, typename _Vty>
		struct SetExtractKey
		{
			const _Kty& operator()(const _Vty& p) const
			{
				return p;
			}
		};

		template <typename _Kty, 
			typename _Vty,
			typename _ExtractKey,				// MapExtractKey for HashMap, SetExtractKey for HashSet.
			typename _Hash = Hash<_Kty>,		// Used to hash the key value.
			typename _KeyEqual = EqualTo<_Kty>>	// Used to compare the element.
		class HashTable
		{
		public:
			using key_type = _Kty;
			using value_type = _Vty;
			using hasher = _Hash;
			using key_equal = _KeyEqual;
			using reference = value_type&;
			using const_reference = const value_type&;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using iterator = Iterator<value_type, false>;
			using const_iterator = Iterator<value_type, true>;
			using local_iterator = BucketIterator<value_type, false>;
			using const_local_iterator = BucketIterator<value_type, true>;
			using node_type = Node<value_type>;
			using insert_return_type = iterator;

			using extract_key = _ExtractKey;

			// -------------------- Begin of ABI compatible part --------------------

			//! The allocator that is used to allocate memory for hash table and elements.
			P<IAllocator> m_alloc;
			//! A pointer to the hash table, which is an array of linked lists.
			node_type** m_buckets;
			//! The number of buckets in total. There is one 
			size_t m_bucket_count;
			//! The number of elements in the hash table.
			size_t m_size;
			//! The maximum load factor of the table, which determines how often the rehashing 
			//! will occur. The load factor is how many elements are placed in one bucket in 
			//! average.
			float32 m_max_load_factor;

			// --------------------  End of ABI compatible part  --------------------

			static constexpr size_t initial_bucket_v = 16;

			void internal_clear_bucket(size_t i)
			{
				auto cur = m_buckets[i];
				while (cur)
				{
					auto next = cur->m_next;
					m_alloc->delete_obj(cur);
					cur = next;
				}
				m_buckets[i] = nullptr;
			}

			void internal_free_table()
			{
				if (m_buckets)
				{
					m_alloc->free(m_buckets);
					m_buckets = nullptr;
				}
			}

			void internal_clear()
			{
				for (size_t i = 0; i < m_bucket_count; ++i)
				{
					internal_clear_bucket(i);
				}
				internal_free_table();
				m_bucket_count = 0;
				m_size = 0;
			}

			node_type** internal_alloc_table(size_t cap)
			{
				node_type** buf = (node_type**)(m_alloc->allocate(sizeof(node_type*) * (cap + 1)));
				if (!buf)
				{
					return nullptr;
				}
				memzero(buf, sizeof(node_type*) * cap);
				buf[cap] = (node_type*)(size_t_max_v);
				return buf;
			}

			HashTable(IAllocator* alloc) :
				m_alloc(alloc),
				m_buckets(nullptr),
				m_bucket_count(0),
				m_size(0),
				m_max_load_factor(16) {}
			HashTable(const HashTable& rhs) :
				m_alloc(rhs.m_alloc),
				m_buckets(nullptr),
				m_bucket_count(0),
				m_size(0),
				m_max_load_factor(16)
			{
				max_load_factor(rhs.m_max_load_factor);
				if (!rhs.empty())
				{
					m_buckets = internal_alloc_table(rhs.m_bucket_count);
					m_bucket_count = rhs.m_bucket_count;
					for (size_t i = 0; i < rhs.m_bucket_count; ++i)
					{
						node_type* iter = rhs.m_buckets[i];
						while (iter)
						{
							node_type* node = m_alloc->new_obj<node_type>(*iter);
							// insert.
							node->m_next = m_buckets[i];
							m_buckets[i] = node;
							++m_size;
							iter = iter->m_next;
						}
					}
				}
			}
			HashTable(HashTable&& rhs) :
				m_alloc(rhs.m_alloc),
				m_buckets(rhs.m_buckets),
				m_bucket_count(rhs.m_bucket_count),
				m_size(rhs.m_size),
				m_max_load_factor(rhs.m_max_load_factor)
			{
				rhs.m_buckets = nullptr;
				rhs.m_bucket_count = 0;
				rhs.m_size = 0;
			}
			HashTable& operator=(const HashTable& rhs)
			{
				internal_clear();
				m_alloc = rhs.m_alloc;
				max_load_factor(rhs.m_max_load_factor);
				if (!rhs.empty())
				{
					m_buckets = internal_alloc_table(rhs.m_bucket_count);
					m_bucket_count = rhs.m_bucket_count;
					for (size_t i = 0; i < rhs.m_bucket_count; ++i)
					{
						node_type* iter = rhs.m_buckets[i];
						while (iter)
						{
							node_type* node = m_alloc->new_obj<node_type>(*iter);
							// insert.
							node->m_next = m_buckets[i];
							m_buckets[i] = node;
							++m_size;
							iter = iter->m_next;
						}
					}
				}
				return *this;
			}
			HashTable& operator=(HashTable&& rhs)
			{
				internal_clear();
				m_alloc = rhs.m_alloc;
				m_buckets = rhs.m_buckets;
				m_bucket_count = rhs.m_bucket_count;
				m_size = rhs.m_size;
				m_max_load_factor = rhs.m_max_load_factor;
				rhs.m_buckets = nullptr;
				rhs.m_bucket_count = 0;
				rhs.m_size = 0;
				return *this;
			}
			~HashTable()
			{
				internal_clear();
			}

			iterator begin()
			{
				if (!m_buckets)
				{
					return iterator(nullptr, nullptr);	// The end() also returns this.
				}
				iterator i(m_buckets);
				if (!(i.m_base.m_current_node))
				{
					i.m_base.increment_bucket();
				}
				return i;
			}
			const_iterator begin() const
			{
				if (!m_buckets)
				{
					return const_iterator(nullptr, nullptr);	// The end() also returns this.
				}
				const_iterator i(m_buckets);
				if (!(i.m_base.m_current_node))
				{
					i.m_base.increment_bucket();
				}
				return i;
			}
			const_iterator cbegin() const
			{
				return begin();
			}
			iterator end()
			{
				if (!m_buckets)
				{
					return iterator(nullptr, nullptr);
				}
				// Returns the bucket after the last valid bucket, whose stored address is always `size_t_max`.
				return iterator(m_buckets + m_bucket_count);	
			}
			const_iterator end() const
			{
				if (!m_buckets)
				{
					return const_iterator(nullptr, nullptr);
				}
				return const_iterator(m_buckets + m_bucket_count);
			}
			const_iterator cend() const
			{
				return end();
			}

			local_iterator begin(size_t n)
			{
				luassert(n < m_bucket_count);
				return local_iterator(m_buckets[n]);
			}
			const_local_iterator begin(size_t n) const
			{
				luassert(n < m_bucket_count);
				return const_local_iterator(m_buckets[n]);
			}
			const_local_iterator cbegin(size_t n) const
			{
				luassert(n < m_bucket_count);
				return const_local_iterator(m_buckets[n]);
			}
			local_iterator end(size_t)
			{
				return local_iterator(nullptr);
			}
			const_local_iterator end(size_t) const
			{
				return const_local_iterator(nullptr);
			}
			const_local_iterator cend(size_t) const
			{
				return const_local_iterator(nullptr);
			}
			bool empty() const
			{
				return m_size == 0;
			}
			size_t size() const
			{
				return m_size;
			}
			size_t bucket_count() const
			{
				return m_bucket_count;
			}
			size_t bucket_size(size_t n) const
			{
				size_t i = 0;
				auto iter = cbegin(n);
				while (iter != cend(n))
				{
					++i;
					++iter;
				}
				return i;
			}
		private:
			size_t hash_code_to_bucket_index(size_t hash_code) const
			{
				if (!m_bucket_count)
				{
					return 0;
				}
				return hash_code % m_bucket_count;
			}
		public:
			size_t bucket(const key_type& key) const
			{
				return hash_code_to_bucket_index(hasher()(key));
			}
			float32 load_factor() const
			{
				if (!m_bucket_count)
				{
					return 0.0f;
				}
				return (float32)m_size / (float32)m_bucket_count;
			}
			float32 max_load_factor() const
			{
				return m_max_load_factor;
			}
			void clear()
			{
				internal_clear();
			}
			hasher hash_function() const
			{
				return hasher();
			}
			key_equal key_eq() const
			{
				return key_equal();
			}
			//! The number of elements this hash table can hold before next rehash.
			size_t capacity() const
			{
				return (size_t)ceilf(m_max_load_factor * m_bucket_count);
			}
			void rehash(size_t new_buckets_count)
			{
				// The new buckets count is at least `initial_bucket_v`(currently 16), if the new bucket count exceeds the maximum load
				// factor, it will be expanded to match the load factor.
				new_buckets_count = max(max(new_buckets_count, (size_t)(ceilf((float32)size() / m_max_load_factor))), initial_bucket_v);
				if (new_buckets_count == m_bucket_count)
				{
					return;
				}
				auto new_bkts = internal_alloc_table(new_buckets_count);
				// Do rehash. iterates the old hash buckets and moves all nodes to the new hash table.
				for (size_t i = 0; i < m_bucket_count; ++i)
				{
					node_type* iter = m_buckets[i];	// the iterator for the linked list.
					while (iter)
					{
						// rehash the element.
						size_t bkt_index = (hasher()(extract_key()(iter->m_value))) % new_buckets_count;
						// borrow the node from old bucket to new.
						node_type* node = iter;
						iter = iter->m_next;
						node->m_next = new_bkts[bkt_index];	// insert into front.
						new_bkts[bkt_index] = node;
					}
				}
				internal_free_table();
				m_buckets = new_bkts;
				m_bucket_count = new_buckets_count;
			}
			void reserve(size_t new_cap)
			{
				size_t current_capacity = capacity();
				if (new_cap > current_capacity)
				{
					rehash((size_t)ceilf((float32)new_cap / m_max_load_factor));
				}
			}
		private:
			//! Called in single insertion operations such as insert & emplace, 
			//! this call reserves enough spaces so rehash will not happen often.
			void increment_reserve(size_t new_cap)
			{
				size_t current_capacity = capacity();
				if (new_cap > current_capacity)
				{
					new_cap = max(new_cap, current_capacity * 2);
					rehash((size_t)ceilf((float32)new_cap / m_max_load_factor));
				}
			}
		public:
			void max_load_factor(float32 ml)
			{
				m_max_load_factor = ml;
				if (load_factor() > max_load_factor())
				{
					rehash(0);
				}
			}

			iterator internal_find(const _Kty& key, size_t bucket_index)
			{
				if (!m_buckets)	// early out for empty case.
				{
					return end();
				}
				auto tar_bucket = m_buckets[bucket_index];
				auto cur = tar_bucket;
				while (cur)
				{
					if (key_equal()(key, extract_key()(cur->m_value)))
					{
						return iterator(cur, m_buckets + bucket_index);	// found.
					}
					cur = cur->m_next;
				}
				return end();	// not found.
			}

			const_iterator internal_find(const _Kty& key, size_t bucket_index) const
			{
				if (!m_buckets)	// early out for empty case.
				{
					return end();
				}
				auto tar_bucket = m_buckets[bucket_index];
				auto cur = tar_bucket;
				while (cur)
				{
					if (key_equal()(key, extract_key()(cur->m_value)))
					{
						return const_iterator(cur, m_buckets + bucket_index);	// found.
					}
					cur = cur->m_next;
				}
				return end();	// not found.
			}

			//! Emplace and inserts the elements as the first node in the bucket.
			template <typename... _Args>
			iterator internal_insert_to_first(size_t hash_code, _Args&&... args)
			{
				increment_reserve(m_size + 1);
				// In case that the table is rehashed, we need to defer the mediation until 
				// the table is rehashed. 
				size_t bucket_index = hash_code_to_bucket_index(hash_code);
				node_type* new_node = m_alloc->new_obj<node_type>(forward<_Args>(args)...);
				new_node->m_next = m_buckets[bucket_index];
				m_buckets[bucket_index] = new_node;
				++m_size;
				return iterator(m_buckets + bucket_index);
			}

			//! Emplace and inserts the elements as the first node in the bucket. Key is extracted
			//! from parameters.
			//! This is slightly costly than internal_insert_to_first, but it prevents calling the move
			//! constructor for the object.
			template <typename... _Args>
			iterator internal_emplace_to_first(_Args&&... args)
			{
				node_type* new_node = m_alloc->new_obj<node_type>(forward<_Args>(args)...);
				size_t hash_code = hasher()(extract_key()(new_node->m_value));
				auto iter = internal_find(extract_key()(new_node->m_value), hash_code_to_bucket_index(hash_code));
				if (iter != end())
				{
					m_alloc->delete_obj(new_node);
					return iter;
				}
				increment_reserve(m_size + 1);
				// In case that the table is rehashed, we need to defer the mediation until 
				// the table is rehashed. 
				size_t hash_k = hash_code_to_bucket_index(hash_code);
				new_node->m_next = m_buckets[hash_k];
				m_buckets[hash_k] = new_node;
				++m_size;
				return iterator(m_buckets + hash_k);
			}

			iterator find(const key_type& key)
			{
				size_t hash_k = bucket(key);
				return internal_find(key, hash_k);
			}

			const_iterator find(const key_type& key) const
			{
				size_t hash_k = bucket(key);
				return internal_find(key, hash_k);
			}

			bool contains(const key_type& key) const
			{
				size_t hash_k = bucket(key);
				return (internal_find(key, hash_k) == end()) ? false : true;
			}

			Pair<iterator, bool> insert(const value_type& value)
			{
				size_t hash_code = hasher()(extract_key()(value));
				auto iter = internal_find(extract_key()(value), hash_code_to_bucket_index(hash_code));
				if (iter != end())
				{
					return make_pair(iter, false);
				}
				return make_pair(internal_insert_to_first(hash_code, value), true);
			}

			Pair<iterator, bool> insert(value_type&& value)
			{
				size_t hash_code = hasher()(extract_key()(value));
				auto iter = internal_find(extract_key()(value), hash_code_to_bucket_index(hash_code));
				if (iter != end())
				{
					return make_pair(iter, false);
				}
				return make_pair(internal_insert_to_first(hash_code, move(value)), true);
			}

			insert_return_type insert(node_type&& node)
			{
				size_t hash_code = hasher()(extract_key()(node.m_value));
				auto iter = internal_find(extract_key()(node.m_value), hash_code_to_bucket_index(hash_code));
				if (iter != end())
				{
					return iter;
				}
				return internal_insert_to_first(hash_code, move(node));
			}

			template <typename _M>
			Pair<iterator, bool> insert_or_assign(const key_type& key, _M&& value)
			{
				size_t hash_code = hasher()(key);
				auto iter = internal_find(key, hash_code_to_bucket_index(hash_code));
				if (iter != end())
				{
					iter->second = forward<_M>(value);
					return make_pair(iter, false);
				}
				else
				{
					return make_pair(internal_insert_to_first(hash_code, value_type(key, forward<_M>(value))), true);
				}
			}

			template <typename _M>
			Pair<iterator, bool> insert_or_assign(key_type&& key, _M&& value)
			{
				size_t hash_code = hasher()(key);
				auto iter = internal_find(key, hash_code_to_bucket_index(hash_code));
				if (iter != end())
				{
					iter->second = forward<_M>(value);
					return make_pair(iter, false);
				}
				else
				{
					return make_pair(internal_insert_to_first(hash_code, value_type(move(key), forward<_M>(value))), true);
				}
			}

			template <typename... _Args>
			iterator emplace(_Args&&... args)
			{
				return internal_emplace_to_first(forward<_Args>(args)...);
			}

			iterator erase(const_iterator pos)
			{
				iterator inext(pos.m_base.m_current_node, pos.m_base.m_current_bucket);
				++inext;

				node_type* node = pos.m_base.m_current_node;
				node_type* node_cur = *(pos.m_base.m_current_bucket);

				if (node == node_cur)
				{
					*(pos.m_base.m_current_bucket) = node_cur->m_next;
				}
				else
				{
					node_type* node_next = node_cur->m_next;
					while (node_next != node)
					{
						node_cur = node_next;
						node_next = node_cur->m_next;
					}
					node_cur->m_next = node_next->m_next;
				}
				m_alloc->delete_obj(node);
				--m_size;
				return inext;
			}

			size_t erase(const key_type& key)
			{
				auto iter = find(key);
				if (iter != end())
				{
					erase(iter);
					return 1;
				}
				return 0;
			}

			node_type extract(const_iterator pos)
			{
				node_type ret_node(move(*(pos.m_base.m_current_node)));
				erase(pos);
				return ret_node;
			}
		};
	}
}