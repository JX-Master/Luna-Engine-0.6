// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file HashTest.cpp
* @author JXMaster
* @date 2020/2/19
*/
#include "TestCommon.hpp"
#include <Base/HashMap.hpp>
#include <Base/HashSet.hpp>

namespace luna
{
	void hash_test()
	{
		TestObject::reset();

		{
			HashSet<int> h(get_global_heap());
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
			// insert_return_type insert(const value_type& value);
			// insert_return_type insert(const value_type& value, hash_code_t c, node_type* pNodeNew = NULL);
			// iterator           insert(const_iterator, const value_type& value);
			// iterator           find(const key_type& k);
			// const_iterator     find(const key_type& k) const;

			HashSet<int> h(get_global_heap());
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

			int* const intarr = allocate_array<int>(get_global_heap(), count);
			memset(intarr, 0, count * sizeof(int));
			int n_count = 0;

			for (auto iter = h.begin(); iter != h.end(); ++iter, ++n_count)
			{
				int i = *iter;
				lutest((i >= 0) && (i < count) && (intarr[i] == 0));
				intarr[i] = 1;
			}

			lutest(n_count == count);
			deallocate_array(get_global_heap(), intarr);
		}

		{
			// size_type bucket_count() const
			// size_type bucket_size(size_type n) const
			// float load_factor() const
			// float get_max_load_factor() const;
			// void  set_max_load_factor(float fMaxLoadFactor);
			// void rehash(size_type n);

			HashSet<int> h(get_global_heap());
			float32 lf = h.load_factor();
			lutest(lf == 0.0f);
			h.max_load_factor(65536.0f * 512.0f);
			float32 max_lf = h.max_load_factor();
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
				lutest((Hash<int>()(v) % h.bucket_count()) == b);	// This is how we hash value into bucket.
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
			HashSet<int> h1(get_global_heap());
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

			HashSet<int> h1(get_global_heap());
			HashSet<int> h2(get_global_heap());
			h1.insert(1);
			lutest(h1.size() == 1);
			auto node = h1.extract(h1.begin());
			lutest(h1.size() == 0);
			h2.insert(move(node));
			lutest(h2.size() == 1);
			lutest(h2.find(1) != h2.end());
		}
	}
}