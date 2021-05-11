// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file VectorTest.cpp
* @author JXMaster
* @date 2020/2/16
*/
#include "TestCommon.hpp"
#include <Runtime/Vector.hpp>
#include <Runtime/String.hpp>

namespace Luna
{
	void vector_test()
	{
		TestObject::reset();
		
		{
			// Default ctor.
			Vector<TestObject> vec1;

			lutest(vec1.empty());
			lutest(vec1.size() == 0);

			// Move ctor.
			vec1.push_back(TestObject(7));
			Vector<TestObject> vec2(move(vec1));

			lutest(vec1.empty());
			lutest(vec1.size() == 0);
			lutest(vec2.size() == 1);
			lutest(vec2[0] == TestObject(7));

			// Clone.
			Vector<TestObject> vec3 = vec2;

			lutest(vec3.size() == 1);
			lutest(vec3[0] == TestObject(7));

			// Move assignment. The move from vector is cleared.
			vec3[0] = TestObject(9);
			vec3 = move(vec2);

			lutest(vec3.size() == 1);
			lutest(vec3[0] == TestObject(7));
			lutest(vec2.size() == 0);

			Vector<i32> vec4(10, 12);
			lutest(vec4.size() == 10);
			lutest(vec4[9] == 12);

			Vector<TestObject> vec5(10);
			lutest(vec5.size() == 10);
			lutest(vec5[4] == TestObject(0));

			// Copy ctor.
			Vector<TestObject> vec6(vec5);
			lutest(vec6.size() == 10);
			lutest(vec6[4] == TestObject(0));

			// Copy assignment.
			Vector<TestObject> vec7;
			vec7 = vec6;
			lutest(vec7.size() == 10);
			lutest(vec7[4] == TestObject(0));
		}

		lutest(TestObject::is_clear());
		TestObject::reset();

		{
			// pointer         data();
			// const_pointer   data() const;
			// reference       front();
			// const_reference front() const;
			// reference       back();
			// const_reference back() const;

			Vector<int> vec;
			vec.resize(10, 7);
			vec[0] = 10;
			vec[1] = 11;
			vec[2] = 12;

			lutest(vec.data() == &vec[0]);
			lutest(*vec.data() == 10);
			lutest(vec.front() == 10);
			lutest(vec.back() == 7);

			Vector<TestObject> vec2;
			vec2.resize(10, TestObject(7));

			const Vector<TestObject>& vec3 = vec2;

			lutest(vec3.data() == &vec3[0]);
			lutest(*vec3.data() == TestObject(7));
			lutest(vec3.front() == TestObject(7));
			lutest(vec3.back() == TestObject(7));
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

			Vector<int> vec;
			vec.resize(20);
			int i;
			for (i = 0; i < 20; ++i)
			{
				vec[i] = i;
			}
			i = 0;
			for (auto iter = vec.begin(); iter != vec.end(); ++iter)
			{
				lutest(*iter == i);
				++i;
			}
			i = (int)vec.size() - 1;
			for (auto iter = vec.rbegin(); iter != vec.rend(); ++iter)
			{
				lutest(*iter == i);
				--i;
			}
		}

		lutest(TestObject::is_clear());
		TestObject::reset();

		{
			// void    swap(vector& x);
			// void    assign(size_type n, const value_type& value);
			// void    assign(InputIterator first, InputIterator last);

			const int A[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
			const int B[] = { 99, 99, 99, 99, 99 };
			const size_t N = sizeof(A) / sizeof(int);
			const size_t M = sizeof(B) / sizeof(int);

			// assign from pointer range
			Vector<int> vec;
			vec.assign(A, A + N);
			lutest(equal(vec.begin(), vec.end(), A));
			lutest(vec.size() == N);

			// assign from iterator range
			Vector<int> vec2;
			vec2.assign(vec.begin(), vec.end());
			lutest(equal(A, A + N, vec2.begin()));
			lutest(equal(vec2.begin(), vec2.end(), A));

			// assign from initializer range with resize
			vec2.assign(M, 99);
			lutest(equal(vec2.begin(), vec2.end(), B));
			lutest(equal(B, B + M, vec2.begin()));
			lutest((vec2.size() == M) && (M != N));

			// void assign(initializer_list<T> ilist);
			vec2.assign({ 0, 1, 2, 3 });
			lutest(vec2.size() == 4);
			lutest((vec2[0] == 0) && (vec2[3] == 3));
		}

		lutest(TestObject::is_clear());
		TestObject::reset();

		{
			// reference       operator[](size_type n);
			// const_reference operator[](size_type n) const;
			// reference       at(size_type n);
			// const_reference at(size_type n) const;

			Vector<int> vec;
			vec.resize(5, 0);
			lutest(vec[3] == 0);
			lutest(vec.at(3) == 0);

			Vector<TestObject> vec2;
			vec2.resize(5);
			lutest(vec2[3] == TestObject(0));
			lutest(vec2.at(3) == TestObject(0));
		}

		lutest(TestObject::is_clear());
		TestObject::reset();

		{
			// void push_back(const value_type& value);
			// void pop_back();
			// void push_back(T&& value);

			Vector<int> vec;
			vec.resize(6);
			int i;
			for (i = 0; i < 6; ++i)
			{
				vec[i] = i;
			}
			lutest(vec.size() == 6);
			lutest(vec[5] == 5);
			for (i = 0; i < 40; ++i)
			{
				vec.push_back(98);
			}
			lutest(vec.size() == 46);
			lutest(vec[45] = 98);
			for (i = 0; i < 30; ++i)
			{
				vec.pop_back();
			}
			lutest(vec.size() == 16);
			lutest(vec[5] = 5);
		}

		{
			// template<class... Args>
			// iterator emplace(const_iterator position, Args&&... args);

			// template<class... Args>
			// void emplace_back(Args&&... args);

			// iterator insert(const_iterator position, value_type&& value);
			// void push_back(value_type&& value);

			TestObject::reset();

			Vector<TestObject> vec;

			vec.emplace_back(2, 3, 4);
			lutest(vec.size() == 1);
			lutest(vec.back().m_value == (2 + 3 + 4));
			lutest(TestObject::g_ctor_count == 1);

			vec.emplace(vec.begin(), 3, 4, 5);
			lutest(vec.size() == 2);
			lutest(vec.back().m_value == (2 + 3 + 4));
			lutest(vec.front().m_value == (3 + 4 + 5));
			// lutest(TestObject::g_ctor_count == 2);  This behavior is uncertain, it depends on how `std::move_backward` is implemented.

			TestObject::reset();

			Vector<TestObject> vec2;
			vec2.push_back(TestObject(2, 3, 4));
			lutest(vec2.size() == 1);
			lutest(vec2.back().m_value == (2 + 3 + 4));
			lutest(TestObject::g_move_ctor_count == 1);

			vec2.insert(vec2.begin(), TestObject(3, 4, 5));
			lutest(vec2.size() == 2);
			lutest(vec2.back().m_value == (2 + 3 + 4));
			lutest(vec2.front().m_value == (3 + 4 + 5));
			// lutest(TestObject::g_move_ctor_count == 2); This behavior is uncertain, it depends on how `std::move_backward` is implemented.
		}

		TestObject::reset();

		{
			// iterator erase(iterator position);
			// iterator erase(iterator first, iterator last);
			// void     clear();

			Vector<int> vec;
			vec.resize(20);
			int i;
			for (i = 0; i < 20; ++i)
			{
				vec[i] = i;
			}
			vec.erase(vec.begin() + 10);// Becomes: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19
			lutest(vec.size() == 19);
			lutest(vec[0] == 0);
			lutest(vec[10] == 11);
			lutest(vec[18] == 19);

			vec.erase(vec.begin() + 10, vec.begin() + 15);// Becomes: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 16, 17, 18, 19
			lutest(vec.size() == 14);
			lutest(vec[9] == 9);
			lutest(vec[13] == 19);

			vec.erase(vec.begin() + 1, vec.begin() + 5); // Becomes: 0, 5, 6, 7, 8, 9, 16, 17, 18, 19
			lutest(vec.size() == 10);
			lutest(vec[0] == 0);
			lutest(vec[1] == 5);
			lutest(vec[9] == 19);

			vec.erase(vec.begin() + 7, vec.begin() + 10);	 // Becomes: 0, 5, 6, 7, 8, 9, 16
			lutest(vec.size() == 7);
			lutest(vec[0] == 0);
			lutest(vec[1] == 5);
			lutest(vec[6] == 16);

			vec.erase(vec.begin() + 3, vec.begin() + 7);	 // Becomes: 0, 5, 6.
			lutest(vec.size() == 3);
			lutest(vec[0] == 0);
			lutest(vec[1] == 5);
			lutest(vec[2] == 6);

			vec.clear();
			lutest(vec.empty());
			lutest(vec.size() == 0);
		}

		{
			// iterator insert(iterator position, const value_type& value);
			// iterator insert(iterator position, size_type n, const value_type& value);
			// iterator insert(iterator position, InputIterator first, InputIterator last);
			// iterator insert(const_iterator position, std::initializer_list<T> ilist);
			Vector<int> vec;
			vec.resize(7, 13);
			int seq1[] = { 13, 13, 13, 13, 13, 13, 13 };
			lutest(equal(vec.begin(), vec.end(), seq1));

			// insert at end of size and capacity.
			vec.insert(vec.end(), 99);
			int seq2[] = { 13, 13, 13, 13, 13, 13, 13, 99};
			lutest(equal(vec.begin(), vec.end(), seq2));

			// insert at end of size.
			vec.reserve(30);
			vec.insert(vec.end(), 999);
			int seq3[] = { 13, 13, 13, 13, 13, 13, 13, 99, 999};
			lutest(equal(vec.begin(), vec.end(), seq3));

			// Insert in middle.
			auto iter = vec.insert(vec.begin() + 7, 49);
			int seq4[] = { 13, 13, 13, 13, 13, 13, 13, 49, 99, 999 };
			lutest(equal(vec.begin(), vec.end(), seq4));
			lutest(iter == (vec.begin() + 7));

			// Insert multiple copies
			iter = vec.insert(vec.begin() + 5, 3, 42);
			int seq5[] = { 13, 13, 13, 13, 13, 42, 42, 42, 13, 13, 49, 99, 999 };
			lutest(equal(vec.begin(), vec.end(), seq5));
			lutest(iter == (vec.begin() + 5));

			// Insert multiple copies with count == 0
			auto at = vec.end();
			iter = vec.insert(at, 0, 666);
			lutest(equal(vec.begin(), vec.end(), seq5));
			lutest(iter == at);

			// Insert iterator range
			const int data[] = { 2, 3, 4, 5 };
			at = vec.begin() + 1;
			iter = vec.insert(at, data, data + 4);
			lutest(iter == at);
			int seq6[] = { 13, 2, 3, 4, 5, 13, 13, 13, 13, 42, 42,
				42, 13, 13, 49, 99, 999 };
			lutest(equal(vec.begin(), vec.end(), seq6));

			// Insert empty iterator range
			at = vec.begin() + 1;
			iter = vec.insert(at, data + 4, data + 4);
			lutest(iter == at);
			lutest(equal(vec.begin(), vec.end(), seq6));

			// Insert with reallocation
			iter = vec.insert(vec.end() - 3, 6, 17);
			lutest(iter == vec.end() - (3 + 6));
			int seq7[] = { 13, 2, 3, 4, 5, 13, 13, 13, 13, 42, 42,
				42, 13, 13, 17, 17, 17, 17, 17, 17, 49, 99, 999 };
			lutest(equal(vec.begin(), vec.end(), seq7));

			// Single insert with reallocation
			Vector<int> vec2;
			vec2.reserve(100);
			vec2.insert(vec2.begin(), 100, 17);
			lutest(vec2.size() == 100);
			lutest(vec2[0] == 17);
			vec2.insert(vec2.begin() + 50, 42);
			lutest(vec2.size() == 101);
			lutest(vec2[50] == 42);
		}

		lutest(TestObject::is_clear());
		TestObject::reset();

		{
			// reserve / resize / capacity / clear
			Vector<int> vec;
			vec.assign(10, 17);
			vec.reserve(20);
			lutest(vec.size() == 10);
			lutest(vec.capacity() == 20);

			vec.resize(7);	// Shrink
			lutest(vec.capacity() == 20);

			vec.resize(17); // Grow without reallocation
			lutest(vec.capacity() == 20);

			vec.resize(42);	// Grow with reallocation
			lutest(vec.capacity() >= 42);

			vec.resize(44, 19);	// Grow with reallocation
			lutest(vec[43] == 19);

			auto c = vec.capacity();
			vec.clear();
			lutest(vec.empty());
			lutest(vec.capacity() == c);

			vec.shrink_to_fit();
			lutest(vec.size() == vec.capacity());

			Vector<int> vec2;
			vec2.swap(vec);
			lutest(vec.empty());
			lutest(vec.capacity() == 0);
		}
	}
}