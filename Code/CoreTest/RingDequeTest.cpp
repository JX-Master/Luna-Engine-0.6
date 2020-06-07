// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RingDequeTest.cpp
* @author JXMaster
* @date 2020/2/23
*/
#include "TestCommon.hpp"
#include <Base/RingDeque.hpp>

namespace luna
{
	void ring_deque_test()
	{
		TestObject::reset();

		{
			// Default ctor.
			RingDeque<TestObject> rd1(get_global_heap());

			lutest(rd1.empty());
			lutest(rd1.size() == 0);

			// Copy ctor is disabled.

			// Move ctor.
			rd1.push_back(TestObject(7));
			RingDeque<TestObject> rd2(move(rd1));

			lutest(rd1.empty());
			lutest(rd1.size() == 0);
			lutest(rd2.size() == 1);
			lutest(rd2[0] == TestObject(7));

			// Clone.
			RingDeque<TestObject> rd3 = rd2;

			lutest(rd3.size() == 1);
			lutest(rd3[0] == TestObject(7));

			// Copy assignment is disabled.

			// Move assignment. The move from ring deque is cleared.
			rd3[0] = TestObject(9);
			rd3 = move(rd2);

			lutest(rd3.size() == 1);
			lutest(rd3[0] == TestObject(7));
			lutest(rd2.size() == 0);
		}

		lutest(TestObject::is_clear());
		TestObject::reset();

		{
			// reference       front();
			// const_reference front() const;
			// reference       back();
			// const_reference back() const;

			RingDeque<int> rd(get_global_heap());
			rd.resize(10, 7);
			rd[0] = 10;
			rd[1] = 11;
			rd[2] = 12;

			lutest(rd.front() == 10);
			lutest(rd.back() == 7);

			RingDeque<TestObject> rd2(get_global_heap());
			rd2.resize(10, TestObject(7));

			const RingDeque<TestObject>& rd3 = rd2;

			lutest(rd3.front() == TestObject(7));
			lutest(rd3.back() == TestObject(7));
		}

		lutest(TestObject::is_clear());
		TestObject::reset();

		{
			// iterator               begin();
			// const_iterator         begin() const;
			// iterator               end();
			// const_iterator         end() const;
			// reverse_iterator       rbegin();
			// const_reverse_iterator rbegin() const;
			// reverse_iterator       rend();
			// const_reverse_iterator rend() const;

			RingDeque<int> rd(get_global_heap());
			rd.resize(20);
			int i;
			for (i = 0; i < 20; ++i)
			{
				rd[i] = i;
			}
			i = 0;
			for (auto iter = rd.begin(); iter != rd.end(); ++iter)
			{
				lutest(*iter == i);
				++i;
			}
			i = rd.size() - 1;
			for (auto iter = rd.rbegin(); iter != rd.rend(); ++iter)
			{
				lutest(*iter == i);
				--i;
			}
		}

		lutest(TestObject::is_clear());
		TestObject::reset();

		{
			// void    swap(rdtor& x);
			// void    assign(size_type n, const value_type& value);
			// void    assign(InputIterator first, InputIterator last);

			const int A[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
			const int B[] = { 99, 99, 99, 99, 99 };
			const size_t N = sizeof(A) / sizeof(int);
			const size_t M = sizeof(B) / sizeof(int);

			// assign from pointer range
			RingDeque<int> rd(get_global_heap());
			rd.assign(A, A + N);
			lutest(equal(rd.begin(), rd.end(), A));
			lutest(rd.size() == N);

			// assign from iterator range
			RingDeque<int> rd2(get_global_heap());
			rd2.assign(rd.begin(), rd.end());
			lutest(equal(A, A + N, rd2.begin()));
			lutest(equal(rd2.begin(), rd2.end(), A));

			// assign from initializer range with resize
			rd2.assign(M, 99);
			lutest(equal(rd2.begin(), rd2.end(), B));
			lutest(equal(B, B + M, rd2.begin()));
			lutest((rd2.size() == M) && (M != N));

			// void assign(initializer_list<T> ilist);
			rd2.assign({ 0, 1, 2, 3 });
			lutest(rd2.size() == 4);
			lutest((rd2[0] == 0) && (rd2[3] == 3));
		}

		lutest(TestObject::is_clear());
		TestObject::reset();

		{
			// reference       operator[](size_type n);
			// const_reference operator[](size_type n) const;
			// reference       at(size_type n);
			// const_reference at(size_type n) const;

			RingDeque<int> rd(get_global_heap());
			rd.resize(5, 0);
			lutest(rd[3] == 0);
			lutest(rd.at(3) == 0);

			RingDeque<TestObject> rd2(get_global_heap());
			rd2.resize(5);
			lutest(rd2[3] == TestObject(0));
			lutest(rd2.at(3) == TestObject(0));
		}

		lutest(TestObject::is_clear());
		TestObject::reset();

		{
			// void push_back(const value_type& value);
			// void pop_back();
			// void push_back(T&& value);

			// void push_front(const value_type& value);
			// void pop_front();
			// void push_front(T&& value);

			RingDeque<int> rd(get_global_heap());
			rd.resize(6);
			int i;
			for (i = 0; i < 6; ++i)
			{
				rd[i] = i;
			}
			lutest(rd.size() == 6);
			lutest(rd[5] == 5);
			for (i = 0; i < 40; ++i)
			{
				rd.push_back(98);
			}
			lutest(rd.size() == 46);
			lutest(rd[45] = 98);
			for (i = 0; i < 30; ++i)
			{
				rd.pop_back();
			}
			lutest(rd.size() == 16);
			lutest(rd[5] = 5);

			rd.push_front(45);
			lutest(rd.size() == 17);
			lutest(rd[0] = 45);
			lutest(rd[6] = 5);

			for (i = 0; i < 40; ++i)
			{
				rd.push_front(75);
			}
			lutest(rd.size() == 57);
			lutest(rd[0] == 75);
			for (i = 0; i < 40; ++i)
			{
				rd.pop_front();
			}
			lutest(rd.size() == 17);
			lutest(rd[0] = 45);
			lutest(rd[6] = 5);
		}

		{
			// template<class... Args>
			// iterator emplace(const_iterator position, Args&&... args);

			// template<class... Args>
			// void emplace_back(Args&&... args);

			// template<class... Args>
			// void emplace_front(Args&&... args);

			// iterator insert(const_iterator position, value_type&& value);
			// void push_back(value_type&& value);
			// void push_front(value_type&& value);

			TestObject::reset();

			RingDeque<TestObject> rd(get_global_heap());

			rd.emplace_back(2, 3, 4);
			lutest(rd.size() == 1);
			lutest(rd.back().m_value == (2 + 3 + 4));
			lutest(TestObject::g_ctor_count == 1);

			rd.emplace_front(3, 4, 5);
			lutest(rd.size() == 2);
			lutest(rd.front().m_value == (3 + 4 + 5));
			lutest(TestObject::g_ctor_count == 2);

			rd.emplace(rd.begin(), 4, 5, 6);
			lutest(rd.size() == 3);
			lutest(rd.back().m_value == (2 + 3 + 4));
			lutest(rd.front().m_value == (4 + 5 + 6));
			lutest(TestObject::g_ctor_count == 3); // Because TestObject is trivially relocatable, the relocation will not invoke move constructor.

			TestObject::reset();

			RingDeque<TestObject> rd2(get_global_heap());
			rd2.push_back(TestObject(2, 3, 4));
			lutest(rd2.size() == 1);
			lutest(rd2.back().m_value == (2 + 3 + 4));
			lutest(TestObject::g_move_ctor_count == 1);

			rd2.push_front(TestObject(3, 4, 5));
			lutest(rd2.size() == 2);
			lutest(rd2.front().m_value == (3 + 4 + 5));
			lutest(TestObject::g_move_ctor_count == 2);

			rd2.insert(rd2.begin(), TestObject(4, 5, 6));
			lutest(rd2.size() == 3);
			lutest(rd2.back().m_value == (2 + 3 + 4));
			lutest(rd2.front().m_value == (4 + 5 + 6));
			lutest(TestObject::g_move_ctor_count == 3); // Because TestObject is trivially relocatable, the relocation will not invoke move constructor.
		}

		TestObject::reset();

		{
			// iterator erase(iterator position);
			// iterator erase(iterator first, iterator last);
			// void     clear();

			RingDeque<int> rd(get_global_heap());
			rd.resize(20);
			int i;
			for (i = 0; i < 20; ++i)
			{
				rd[i] = i;
			}
			rd.erase(rd.begin() + 10);// Becomes: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19
			lutest(rd.size() == 19);
			lutest(rd[0] == 0);
			lutest(rd[10] == 11);
			lutest(rd[18] == 19);

			rd.erase(rd.begin() + 10, rd.begin() + 15);// Becomes: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 16, 17, 18, 19
			lutest(rd.size() == 14);
			lutest(rd[9] == 9);
			lutest(rd[13] == 19);

			rd.erase(rd.begin() + 1, rd.begin() + 5); // Becomes: 0, 5, 6, 7, 8, 9, 16, 17, 18, 19
			lutest(rd.size() == 10);
			lutest(rd[0] == 0);
			lutest(rd[1] == 5);
			lutest(rd[9] == 19);

			rd.erase(rd.begin() + 7, rd.begin() + 10);	 // Becomes: 0, 5, 6, 7, 8, 9, 16
			lutest(rd.size() == 7);
			lutest(rd[0] == 0);
			lutest(rd[1] == 5);
			lutest(rd[6] == 16);

			rd.erase(rd.begin() + 3, rd.begin() + 7);	 // Becomes: 0, 5, 6.
			lutest(rd.size() == 3);
			lutest(rd[0] == 0);
			lutest(rd[1] == 5);
			lutest(rd[2] == 6);

			// test erase across array bound.
			rd.push_front(9);
			rd.push_front(10);
			rd.push_front(11);
			rd.push_front(12);		// Becomes: 12, 11, 10, 9, | 0, 5, 6.
			lutest(rd.size() == 7);
			lutest(rd[0] == 12);
			lutest(rd[2] == 10);
			lutest(rd[5] == 5);
			rd.erase(rd.begin() + 1, rd.begin() + 5);	// Becomes: 12, 5, 6.
			lutest(rd.size() == 3);
			lutest(rd[0] == 12);
			lutest(rd[1] == 5);
			lutest(rd[2] == 6);

			rd.clear();
			lutest(rd.empty());
			lutest(rd.size() == 0);
		}

		{
			// iterator insert(iterator position, const value_type& value);
			// iterator insert(iterator position, size_type n, const value_type& value);
			// iterator insert(iterator position, InputIterator first, InputIterator last);
			// iterator insert(const_iterator position, std::initializer_list<T> ilist);
			RingDeque<int> rd(get_global_heap());
			rd.resize(7, 13);
			int seq1[] = { 13, 13, 13, 13, 13, 13, 13 };
			lutest(equal(rd.begin(), rd.end(), seq1));

			// insert at end of size and capacity.
			rd.insert(rd.end(), 99);
			int seq2[] = { 13, 13, 13, 13, 13, 13, 13, 99 };
			lutest(equal(rd.begin(), rd.end(), seq2));

			// insert at end of size.
			rd.reserve(30);
			rd.insert(rd.end(), 999);
			int seq3[] = { 13, 13, 13, 13, 13, 13, 13, 99, 999 };
			lutest(equal(rd.begin(), rd.end(), seq3));

			// Insert in middle.
			auto iter = rd.insert(rd.begin() + 7, 49);
			int seq4[] = { 13, 13, 13, 13, 13, 13, 13, 49, 99, 999 };
			lutest(equal(rd.begin(), rd.end(), seq4));
			lutest(iter == (rd.begin() + 7));

			// Insert multiple copies
			iter = rd.insert(rd.begin() + 5, 3, 42);
			int seq5[] = { 13, 13, 13, 13, 13, 42, 42, 42, 13, 13, 49, 99, 999 };
			lutest(equal(rd.begin(), rd.end(), seq5));
			lutest(iter == (rd.begin() + 5));

			// Insert multiple copies with count == 0
			auto at = rd.end();
			iter = rd.insert(at, 0, 666);
			lutest(equal(rd.begin(), rd.end(), seq5));
			lutest(iter == at);

			// Insert iterator range
			const int data[] = { 2, 3, 4, 5 };
			at = rd.begin() + 1;
			iter = rd.insert(at, data, data + 4);
			lutest(iter == at);
			int seq6[] = { 13, 2, 3, 4, 5, 13, 13, 13, 13, 42, 42,
				42, 13, 13, 49, 99, 999 };
			lutest(equal(rd.begin(), rd.end(), seq6));

			// Insert empty iterator range
			at = rd.begin() + 1;
			iter = rd.insert(at, data + 4, data + 4);
			lutest(iter == at);
			lutest(equal(rd.begin(), rd.end(), seq6));

			// Insert with reallocation
			iter = rd.insert(rd.end() - 3, 6, 17);
			lutest(iter == rd.end() - (3 + 6));
			int seq7[] = { 13, 2, 3, 4, 5, 13, 13, 13, 13, 42, 42,
				42, 13, 13, 17, 17, 17, 17, 17, 17, 49, 99, 999 };
			lutest(equal(rd.begin(), rd.end(), seq7));

			// Single insert with reallocation
			RingDeque<int> rd2(get_global_heap());
			rd2.reserve(100);
			rd2.insert(rd2.begin(), 100, 17);
			lutest(rd2.size() == 100);
			lutest(rd2[0] == 17);
			rd2.insert(rd2.begin() + 50, 42);
			lutest(rd2.size() == 101);
			lutest(rd2[50] == 42);
		}

		lutest(TestObject::is_clear());
		TestObject::reset();

		{
			// reserve / resize / capacity / clear
			RingDeque<int> rd(get_global_heap());
			rd.assign(10, 17);
			rd.reserve(20);
			lutest(rd.size() == 10);
			lutest(rd.capacity() == 20);

			rd.resize(7);	// Shrink
			lutest(rd.capacity() == 20);

			rd.resize(17); // Grow without reallocation
			lutest(rd.capacity() == 20);

			rd.resize(42);	// Grow with reallocation
			lutest(rd.capacity() >= 42);

			rd.resize(44, 19);	// Grow with reallocation
			lutest(rd[43] == 19);

			auto c = rd.capacity();
			rd.clear();
			lutest(rd.empty());
			lutest(rd.capacity() == c);

			rd.shrink_to_fit();
			lutest(rd.size() == rd.capacity());

			RingDeque<int> rd2(get_global_heap());
			rd2.swap(rd);
			lutest(rd.empty());
			lutest(rd.capacity() == 0);
		}

		{
			// Use deque to simulates a FIFO queue to test in "real world".
			TestObject::reset();

			RingDeque<TestObject> rd(get_global_heap());

			// push 1000.
			for (int32 i = 0; i < 1000; ++i)
			{
				rd.push_back(TestObject(i));
			}

			// consumes 750.
			for (int32 i = 0; i < 750; ++i)
			{
				luassert(rd.front() == TestObject(i));
				rd.pop_front();
			}

			// then push another 1500.
			for (int32 i = 0; i < 1500; ++i)
			{
				rd.push_back(TestObject(1000 + i));
			}

			// then consumes 250.
			for (int32 i = 0; i < 250; ++i)
			{
				rd.pop_front();
			}

			// then the last row of 2000.
			for (int32 i = 0; i < 2000; ++i)
			{
				rd.push_back(TestObject(2500 + i));
			}

			// all objects are queued, consumes all.
			while (!rd.empty())
			{
				rd.pop_front();
			}

			luassert(rd.empty());
		}
		lutest(TestObject::is_clear());
		TestObject::reset();
	}
}