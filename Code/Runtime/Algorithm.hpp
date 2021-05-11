// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Algorithm.hpp
* @author JXMaster
* @date 2020/2/17
* @brief Defines most commonly used algorithms.
*/
#pragma once
#include "Base.hpp"

/*
	Functions in Algorithm.hpp:
	
	min
	max
	swap
	equal
*/

namespace Luna
{
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

	template <typename _Ty>
	inline constexpr const _Ty& min(const _Ty& a, const _Ty& b)
	{
		return (b < a) ? b : a;
	}

	template <typename _Ty, typename _LessComp>
	inline constexpr const _Ty& min(const _Ty& a, const _Ty& b, _LessComp less_comp)
	{
		return (less_comp(b, a)) ? b : a;
	}

	template <typename _Ty>
	inline constexpr const _Ty& max(const _Ty& a, const _Ty& b)
	{
		return (a < b) ? b : a;
	}

	template <typename _Ty, typename _LessComp>
	inline constexpr const _Ty& max(const _Ty& a, const _Ty& b, _LessComp less_comp)
	{
		return (less_comp(a, b)) ? b : a;
	}

	template <typename _Ty>
	inline void swap(_Ty& a, _Ty& b)
	{
		_Ty temp(move(a));
		a = move(b);
		b = move(temp);
	}

	template <typename _Iter1, typename _Iter2>
	inline bool equal(_Iter1 first1, _Iter1 last1, _Iter2 first2)
	{
		for (; first1 != last1; ++first1, ++first2) 
		{
			if (!(*first1 == *first2)) 
			{
				return false;
			}
		}
		return true;
	}

	template <typename _Iter1, typename _Iter2, typename _EqualComp>
	inline bool equal(_Iter1 first1, _Iter1 last1, _Iter2 first2, _EqualComp equal_compare)
	{
		for (; first1 != last1; ++first1, ++first2) 
		{
			if (!equal_compare(*first1, *first2)) 
			{
				return false;
			}
		}
		return true;
	}
}