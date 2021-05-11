// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file HashMultiTable.hpp
* @author JXMaster
* @date 2020/12/26
*/
#pragma once
#include "HashTable.hpp"

namespace Luna
{
	namespace HashTableImpl
	{
		template <typename _Kty,
			typename _Vty,
			typename _ExtractKey,				// MapExtractKey for HashMap, SetExtractKey for HashSet.
			typename _Hash = hash<_Kty>,		// Used to hash the key value.
			typename _KeyEqual = equal_to<_Kty>>	// Used to compare the element.
		class HashMultiTable
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

			//! A pointer to the hash table, which is an array of linked lists.
			node_type** m_buckets;
			//! The number of buckets in total. There is one 
			usize m_bucket_count;
			//! The number of elements in the hash table.
			usize m_size;
			//! The maximum load factor of the table, which determines how often the rehashing 
			//! will occur. The load factor is how many elements are placed in one bucket in 
			//! average.
			f32 m_max_load_factor;

			// --------------------  End of ABI compatible part  --------------------

			static constexpr usize initial_bucket_v = HashTable::initial_bucket_v;


		};
	}
}