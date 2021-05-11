// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file HashTest.cpp
* @author JXMaster
* @date 2020/2/19
*/
#include "TestCommon.hpp"
#include <Runtime/HashMap.hpp>
#include <Runtime/HashSet.hpp>
#include <Runtime/HashMultiMap.hpp>
#include <Runtime/HashMultiSet.hpp>

namespace Luna
{
	void hash_test()
	{
		TestObject::reset();

		{
			HashSet<int> h;
			lutest(h.empty());
			lutest(h.size() == 0);
			//! When the set is constructed, it does not allocate
			//! any dynamic memory, thus does not have any bucket.
			lutest(h.bucket_count() == 0);

			for (int i = 0; i < 100; ++i)
			{
				h.insert(i);
			}
			lutest(h.size() == 100);

			h.clear();
			lutest(h.empty());
			lutest(h.size() == 0);
			//! When the set gets cleared, it frees all dynamic 
			//! memory, thus does not have any bucket.
			lutest(h.bucket_count() == 0);
		}

		{
			HashMultiSet<int> h;
			lutest(h.empty());
			lutest(h.size() == 0);
			//! When the set is constructed, it does not allocate
			//! any dynamic memory, thus does not have any bucket.
			lutest(h.bucket_count() == 0);

			for (int i = 0; i < 100; ++i)
			{
				h.insert(i);
			}
			lutest(h.size() == 100);

			h.clear();
			lutest(h.empty());
			lutest(h.size() == 0);
			//! When the set gets cleared, it frees all dynamic 
			//! memory, thus does not have any bucket.
			lutest(h.bucket_count() == 0);
		}

		{
			// HashSet
			// size_type          size() const
			// bool               empty() const
			// iterator			  insert(const value_type& value);
			// iterator			  insert(value_type&& value);
			// iterator           find(const key_type& k);
			// const_iterator     find(const key_type& k) const;

			HashSet<int> h;
			constexpr size_t count = 10000;
			lutest(h.empty());
			lutest(h.size() == 0);
			for (int i = 0; i < count; ++i)
			{
				h.insert(i);
			}
			lutest(!h.empty());
			lutest(h.size() == count);
			for (auto iter = h.begin(); iter != h.end(); ++iter)
			{
				int value = *iter;
				lutest(value < count);
			}
			for (int i = 0; i < count * 2; ++i)
			{
				auto iter = h.find(i);
				if (i < count)
				{
					lutest(iter != h.end());
				}
				else
				{
					lutest(iter == h.end());
				}
			}

			// iterator       begin();
			// const_iterator begin() const;
			// iterator       end();
			// const_iterator end() const;
			int* const intarr = (int*)memalloc(count * sizeof(int));
			memset(intarr, 0, count * sizeof(int));
			int n_count = 0;
			for (auto iter = h.begin(); iter != h.end(); ++iter, ++n_count)
			{
				int i = *iter;
				lutest((i >= 0) && (i < count) && (intarr[i] == 0));
				intarr[i] = 1;
			}
			lutest(n_count == count);
			memfree(intarr);
		}

		{
			// HashMultiSet
			// size_type          size() const
			// bool               empty() const
			// iterator           find(const key_type& k);
			// const_iterator     find(const key_type& k) const;

			HashMultiSet<int> h;
			constexpr size_t count = 10000;
			lutest(h.empty());
			lutest(h.size() == 0);
			for (int i = 0; i < count; ++i)
			{
				h.insert(i);
			}
			lutest(!h.empty());
			lutest(h.size() == count);
			for (auto iter = h.begin(); iter != h.end(); ++iter)
			{
				int value = *iter;
				lutest(value < count);
			}
			for (int i = 0; i < count * 2; ++i)
			{
				auto iter = h.find(i);
				if (i < count)
				{
					lutest(iter != h.end());
				}
				else
				{
					lutest(iter == h.end());
				}
			}

			// iterator       begin();
			// const_iterator begin() const;
			// iterator       end();
			// const_iterator end() const;
			int* const intarr = (int*)memalloc(count * sizeof(int));
			memset(intarr, 0, count * sizeof(int));
			int n_count = 0;
			for (auto iter = h.begin(); iter != h.end(); ++iter, ++n_count)
			{
				int i = *iter;
				lutest((i >= 0) && (i < count) && (intarr[i] == 0));
				intarr[i] = 1;
			}
			lutest(n_count == count);
			memfree(intarr);
		}

		{
			// size_type bucket_count() const
			// size_type bucket_size(size_type n) const
			// float load_factor() const
			// float get_max_load_factor() const;
			// void  set_max_load_factor(float fMaxLoadFactor);
			// void rehash(size_type n);

			HashSet<int> h;
			f32 lf = h.load_factor();
			lutest(lf == 0.0f);
			h.max_load_factor(65536.0f * 512.0f);
			f32 max_lf = h.max_load_factor();
			lutest(max_lf == (65536.0f * 512.0f));
			h.rehash(20);
			size_t n = h.bucket_count();
			lutest((n >= 20) && (n < 25));
			for (int i = 0; i < 10000; ++i)
			{
				h.insert(i);	// This also tests for high loading.
			}
			size_t n2 = h.bucket_count();
			lutest(n2 == n);	// Verify no rehashing has occurred, due to our high load factor.
			n = h.bucket_size(0);	// force rehash & shrink.
			lutest(n >= ((h.size() / h.bucket_count()) / 2));// It will be some high value. We divide by 2 to give it some slop.

			// local_iterator       begin(size_type n);
			// local_iterator       end(size_type n);
			// const_local_iterator begin(size_type n) const;
			// const_local_iterator end(size_type n) const;	
			size_t b = h.bucket_count() - 1;
			for (auto iter = h.begin(b); iter != h.end(b); ++iter)
			{
				int v = *iter;
				lutest((hash<int>()(v) % h.bucket_count()) == b);	// This is how we hash value into bucket.
			}
			// clear();
			h.clear();
			lutest(h.empty());
			lutest(h.size() == 0);
			lutest(h.bucket_count() == 0);	// Currently clear operation will free all buckets, this behavior may change later.
		}

		{
			// size_type bucket_count() const
			// size_type bucket_size(size_type n) const
			// float load_factor() const
			// float get_max_load_factor() const;
			// void  set_max_load_factor(float fMaxLoadFactor);
			// void rehash(size_type n);

			HashMultiSet<int> h;
			f32 lf = h.load_factor();
			lutest(lf == 0.0f);
			h.max_load_factor(65536.0f * 512.0f);
			f32 max_lf = h.max_load_factor();
			lutest(max_lf == (65536.0f * 512.0f));
			h.rehash(20);
			size_t n = h.bucket_count();
			lutest((n >= 20) && (n < 25));
			for (int i = 0; i < 10000; ++i)
			{
				h.insert(i);	// This also tests for high loading.
			}
			size_t n2 = h.bucket_count();
			lutest(n2 == n);	// Verify no rehashing has occurred, due to our high load factor.
			n = h.bucket_size(0);	// force rehash & shrink.
			lutest(n >= ((h.size() / h.bucket_count()) / 2));// It will be some high value. We divide by 2 to give it some slop.

			// local_iterator       begin(size_type n);
			// local_iterator       end(size_type n);
			// const_local_iterator begin(size_type n) const;
			// const_local_iterator end(size_type n) const;	
			size_t b = h.bucket_count() - 1;
			for (auto iter = h.begin(b); iter != h.end(b); ++iter)
			{
				int v = *iter;
				lutest((hash<int>()(v) % h.bucket_count()) == b);	// This is how we hash value into bucket.
			}

			// clear();
			h.clear();
			lutest(h.empty());
			lutest(h.size() == 0);
			lutest(h.bucket_count() == 0);	// Currently clear operation will free all buckets, this behavior may change later.
		}

		{
			// clone()
			// swap()
			HashSet<int> h1;
			for (int i = 0; i < 10; ++i)
			{
				h1.insert(i);
			}
			lutest(h1.size() == 10);
			auto h2 = h1;
			lutest(h2.size() == 10);
			h2.insert(12);
			lutest(h1.size() == 10);
			lutest(h2.size() == 11);
			h1.swap(h2);
			lutest(h1.size() == 11);
			lutest(h2.size() == 10);
		}

		{
			// clone()
			// swap()
			HashMultiSet<int> h1;
			for (int i = 0; i < 10; ++i)
			{
				h1.insert(i);
			}
			lutest(h1.size() == 10);
			auto h2 = h1;
			lutest(h2.size() == 10);
			h2.insert(12);
			lutest(h1.size() == 10);
			lutest(h2.size() == 11);
			h1.swap(h2);
			lutest(h1.size() == 11);
			lutest(h2.size() == 10);
		}

		{
			// node_type extract()
			// insert(node_type&&)

			HashSet<int> h1;
			HashSet<int> h2;
			h1.insert(1);
			lutest(h1.size() == 1);
			auto node = h1.extract(h1.begin());
			lutest(h1.size() == 0);
			h2.insert(move(node));
			lutest(h2.size() == 1);
			lutest(h2.find(1) != h2.end());
		}

		{
			// node_type extract()
			// insert(node_type&&)

			HashMultiSet<int> h1;
			HashMultiSet<int> h2;
			h1.insert(1);
			lutest(h1.size() == 1);
			auto node = h1.extract(h1.begin());
			lutest(h1.size() == 0);
			h2.insert(move(node));
			lutest(h2.size() == 1);
			lutest(h2.find(1) != h2.end());
		}

		{
			// HashMap, HashSet
			// Pair<iterator, bool> insert(const value_type& value)
			// Pair<iterator, bool> insert(value_type&& value)
			// HashMultiMap, HashMultiSet
			// iterator insert(const value_type& value)
			// iterator insert(value_type&& value)
			HashMap<int, int> h1;
			HashMultiMap<int, int> h2;
			auto r1 = h1.insert(make_pair(3, 3));
			auto r2 = h1.insert(make_pair(3, 4));
			h2.insert(make_pair(3, 3));
			h2.insert(make_pair(3, 4));
			lutest(r1.second == true);
			lutest(r2.second == false);
			lutest(h1.size() == 1);
			lutest(h2.size() == 2);

			// Erase with repeat key.
			auto r3 = h1.erase(3);
			auto r4 = h2.erase(3);
			lutest(r3 == 1);
			lutest(r4 == 2);
			lutest(h1.size() == 0);
			lutest(h2.size() == 0);
		}

		{
			// Pair<iterator, iterator> equal_range(const key_type& key)
			// Pair<const_iterator, const_iterator> equal_range(const key_type& key) const
			// usize count(const key_type& key) const
			HashMap<int, int> h1;
			HashMultiMap<int, int> h2;
			h1.insert(make_pair(3, 3));
			h1.insert(make_pair(3, 4));
			h2.insert(make_pair(3, 3));
			h2.insert(make_pair(3, 4));

			auto range1 = h1.equal_range(3);
			auto range2 = h2.equal_range(3);
			auto c1 = h1.count(3);
			auto c2 = h2.count(3);

			lutest(range1.first->second == 3);
			++range1.first;
			lutest(range1.first == range1.second);
			lutest(range2.first->second == 3);
			++range2.first;
			lutest(range2.first->second == 4);
			++range2.first;
			lutest(range2.first == range2.second);
			lutest(c1 == 1);
			lutest(c2 == 2);
		}

		{
			// HashMap, HashSet
			// Pair<iterator, bool> emplace(_Args... args)
			// HashMultiMap, HashMultiSet
			// iterator emplace(_Args... args)

			HashMap<int, TestObject> h1;
			HashMultiMap<int, TestObject> h2;
			h1.emplace(3, TestObject(5, true));
			h1.emplace(3, TestObject(7, true));
			h2.emplace(3, TestObject(9, true));
			h2.emplace(3, TestObject(11, true));
			lutest(h1.size() == 1);
			lutest(h2.size() == 2);
		}
	}
}