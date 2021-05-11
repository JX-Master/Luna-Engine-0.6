// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file StringTest.cpp
* @author JXMaster
* @date 2020/8/10
*/
#include "TestCommon.hpp"
#include <Runtime/String.hpp>

namespace Luna
{
	void string_test()
	{
		{
			// Default ctor.
			String str1;
			lutest(str1.empty());
			lutest(str1.size() == 0);
			lutest(str1.data() == nullptr);
			lutest(!strcmp(str1.c_str(), u8""));

			// usize count, value_type ch, IAllocator* alloc
			String str2(10, 'a');
			lutest(str2.size() == 10);
			lutest(!strcmp(str2.c_str(), u8"aaaaaaaaaa"));

			// const value_type* s
			String str3(u8"abcdefg");
			lutest(!strcmp(str3.c_str(), u8"abcdefg"));

			// const value_type* s, usize count
			String str4(u8"abcdefg", 5);
			lutest(!strcmp(str4.c_str(), u8"abcde"));

			// const BasicString& rhs, usize pos, usize count = npos
			String str5(str3, 3);
			lutest(!strcmp(str5.c_str(), u8"defg"));

			// const BasicString& rhs
			String str6(str5);
			lutest(!strcmp(str6.c_str(), u8"defg"));

			// BasicString&& rhs
			String str7(move(str6));
			lutest(!strcmp(str7.c_str(), u8"defg"));
			lutest(str6.empty());

			// InitializerList<value_type> ilist
			String str8{ 'a', 'b', 'c', 'd' };
			lutest(!strcmp(str8.c_str(), u8"abcd"));

			// operator=(const BasicString& rhs)
			str8 = str7;
			lutest(!strcmp(str8.c_str(), u8"defg"));
			lutest(!strcmp(str7.c_str(), u8"defg"));

			// operator=(BasicString&& rhs)
			str8 = move(str4);
			lutest(!strcmp(str8.c_str(), u8"abcde"));
			lutest(!strcmp(str4.c_str(), u8""));
			lutest(str4.empty());

			// operator=(const value_type* s)
			str8 = u8"Sample String";
			lutest(!strcmp(str8.c_str(), u8"Sample String"));

			// operator=(value_type ch)
			str8 = 'c';
			lutest(!strcmp(str8.c_str(), u8"c"));

			// operator=(InitializerList<value_type> ilist)
			str8 = { 'e', 't', 'f' };
			lutest(!strcmp(str8.c_str(), u8"etf"));

		}

		{
			// pointer			data()
			// const_pointer	data() const
			// const_pointer	c_str() const
			
			String s;
			lutest(s.data() == nullptr);
			lutest(!strcmp(s.c_str(), u8""));
		}

		{
			// iterator               begin();
			// const_iterator         begin() const;
			// iterator               end();
			// const_iterator         end() const;
			// reverse_iterator       rbegin();
			// const_reverse_iterator rbegin() const;
			// reverse_iterator       rend();
			// const_reverse_iterator rend() const;

			const c8* str = u8"Sample String";
			String s = str;
			auto iter = s.begin();
			lutest(*iter == 'S');
			++iter;
			lutest(*iter == 'a');
			--iter;
			lutest(*iter == 'S');
			auto iter2 = iter + 3;
			lutest(*iter2 == 'p');

			for (auto i = s.begin(); i != s.end(); ++i)
			{
				lutest(*i == str[i - s.begin()]);
			}

			for (auto i = s.cbegin(); i != s.cend(); ++i)
			{
				lutest(*i == str[i - s.cbegin()]);
			}

			for (auto i = s.rbegin(); i != s.rend(); ++i)
			{
				lutest(*i == str[i.base() - s.begin() - 1]);
			}

			for (auto i = s.crbegin(); i != s.crend(); ++i)
			{
				lutest(*i == str[i.base() - s.cbegin() - 1]);
			}
		}

		{
			// usize	size() const
			// usize	length() const
			// usize	capacity() const
			// bool		empty() const

			String s;
			lutest(s.empty());
			s = u8"Sample String";
			lutest(s.size() == 13);
			lutest(s.length() == 13);
			lutest(s.capacity() >= s.size());
			lutest(!s.empty());
		}

		{
			// void		reserve(usize new_cap)
			// void		resize(usize n, value_type v)
			// void		shrink_to_fit()

			String s;
			s.reserve(30);
			lutest(s.capacity() >= 30);
			lutest(s.size() == 0);
			lutest(s.empty());
			s.resize(15, 'c');
			lutest(s.size() == 15);
			lutest(!strcmp(s.c_str(), u8"ccccccccccccccc"));
			s.shrink_to_fit();
			lutest(!strcmp(s.c_str(), u8"ccccccccccccccc"));
			lutest(s.capacity() == 15);
		}

		{
			// reference		operator[](usize n)
			// const_reference	operator[](usize n) const
			// reference		at(usize n)
			// const_reference	at(usize n) const
			// reference		front()
			// const_reference	front() const
			// reference		back()
			// const_reference	back() const

			String s = u8"Sample String";
			lutest(s[0] == 'S');
			lutest(s[3] == 'p');
			lutest(s.at(0) == 'S');
			lutest(s.at(4) == 'l');
			lutest(s.front() == 'S');
			lutest(s.back() == 'g');
		}

		{
			// void clear()
			// void push_back(value_type ch)
			// void pop_back()
			
			String s = u8"Sample String";
			auto sz = s.size();
			lutest(!s.empty());
			s.push_back('A');
			lutest(s.back() == 'A');
			s.pop_back();
			lutest(sz == s.size());
			s.clear();
			lutest(s.empty());
		}

		{
			// void assign(usize count, value_type ch)
			// void assign(const BasicString& str)
			// void assign(const BasicString& str, usize pos, usize count = npos)
			// void assign(BasicString&& str)
			// void assign(const value_type* s, usize count)
			// void assign(const value_type* s)
			// template <typename _InputIt> void assign(_InputIt first, _InputIt last)
			// void assign(InitializerList<value_type> il)

			String s;
			s.assign(10, 'c');
			lutest(!strcmp(s.c_str(), u8"cccccccccc"));
			lutest(s.size() == 10);
			String s2;
			s2.assign(s);
			lutest(!strcmp(s2.c_str(), u8"cccccccccc"));
			lutest(s2.size() == 10);
			s = "abcde";
			s2.assign(s, 1, 2);
			lutest(!strcmp(s2.c_str(), u8"bc"));
			lutest(s2.size() == 2);
			s2.assign(move(s));
			lutest(!strcmp(s2.c_str(), u8"abcde"));
			lutest(s.empty());
			lutest(s2.size() == 5);
			s.assign(u8"aaaaaaaa", 3);
			lutest(!strcmp(s.c_str(), u8"aaa"));
			lutest(s.size() == 3);
			s.assign(u8"aaaaaaaa");
			lutest(!strcmp(s.c_str(), u8"aaaaaaaa"));
			lutest(s.size() == 8);
			s2.assign(s.begin(), s.end());
			lutest(!strcmp(s2.c_str(), u8"aaaaaaaa"));
			s2.assign({ 'a', 'b', 'c' });
			lutest(!strcmp(s2.c_str(), u8"abc"));
			lutest(s2.size() == 3);
		}
	}
}