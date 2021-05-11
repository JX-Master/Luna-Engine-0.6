// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file MemoryUtils.hpp
* @author JXMaster
* @date 2020/2/16
*/
#pragma once
#include "Iterator.hpp"

/*
	Provides low-level manual object construction/destruction/assignment operations on a single object 
	or a range of objects.
	
	Functions in MemoryUtils.hpp:
	
	addressof
	default_construct
	value_construct
	copy_construct
	move_construct
	direct_construct
	destruct
	copy_assign
	move_assign
	default_construct_range
	value_construct_range
	copy_construct_range
	move_construct_range
	destruct_range
	copy_assign_range
	move_assign_range
	move_assign_range_backward
	fill_construct_range
	fill_assign_range
	copy_relocate_range
	move_relocate_range
	move_relocate_range_backward
*/

namespace Luna
{
	//! @class Unconstructed
	//! `Unconstructed` provides a way to allocate the memory for a C++ object without their constructor/destructor
	//! being called by system. You have the ability to call their constructor/destructor manually.
	//! Such feature is useful when you need to declare some static constructed objects and want to control their construction/
	//! destruction orders.
	//! Note that the `Unconstructed` class does not actually know if the object is constructed, you need to manage it manually and
	//! always call the destructor of the object when you want to destroy it.
	template<typename _Ty>
	class Unconstructed
	{
	private:
		alignas(_Ty) u8 m_buffer[sizeof(_Ty)];
	public:

		//! Get a reference to the object.
		_Ty& get()
		{
			return *reinterpret_cast<_Ty*>(&m_buffer);
		}
		const _Ty& get() const
		{
			return *reinterpret_cast<const _Ty*>(&m_buffer);
		}

		//! Manually construct object.
		template< typename... Args>
		void construct(Args&&... args)
		{
			new (m_buffer) _Ty(forward<Args>(args)...);
		}

		//! Manually destruct object.
		void destruct()
		{
			get().~_Ty();
		}
	};

	//! Gets the real address for object or function `args`, even if the `operator&` of the object
	//! has been overridden.
	template<typename _Ty>
	_Ty* addressof(_Ty& value)
	{
#ifdef LUNA_COMPILER_MSVC
		return (__builtin_addressof(value));
#else
		return reinterpret_cast<_Ty*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(value)));
#endif
	}

	//! Calls the default constructor for the object pointed by iterator.
	template <typename _Iter>
	inline void default_construct(_Iter dest)
	{
		new (static_cast<void*>(addressof(*dest)))
			typename iterator_traits<_Iter>::value_type;
	}

	//! Calls the value constructor for the object pointed by iterator.
	template <typename _Iter>
	inline void value_construct(_Iter dest)
	{
		new (static_cast<void*>(addressof(*dest)))
			typename iterator_traits<_Iter>::value_type();
	}

	//! Calls the copy constructor for the object pointed by iterator.
	template <typename _Iter1, typename _Iter2>
	inline void copy_construct(_Iter1 dest, _Iter2 src)
	{
		new (static_cast<void*>(addressof(*dest)))
			typename iterator_traits<_Iter1>::value_type(*src);
	}

	//! Calls the move constructor for the object pointed by iterator.
	template <typename _Iter1, typename _Iter2>
	inline void move_construct(_Iter1 dest, _Iter2 src)
	{
		new (static_cast<void*>(addressof(*dest)))
			typename iterator_traits<_Iter1>::value_type(move(*src));
	}

	//! Calls the direct constructor for the object pointed by iterator.
	template <typename _Iter, typename... _Args>
	inline void direct_construct(_Iter dest, _Args&&... args)
	{
		new (static_cast<void*>(addressof(*dest)))
			typename iterator_traits<_Iter>::value_type(forward<_Args>(args)...);
	}

	//! Calls the destructor of the object pointed by iterator.
	template <typename _Iter>
	inline void destruct(_Iter dest)
	{
		using value_type = typename iterator_traits<_Iter>::value_type;
		static_cast<value_type*>(addressof(*dest))->~value_type();
	}

	//! Calls the copy assignment operator of the object `dest` iterator
	//! is pointing to,
	template <typename _Iter1, typename _Iter2>
	inline void copy_assign(_Iter1 dest, _Iter2 src)
	{
		(*dest) = (*src);
	}

	//! Calls the move assignment operator of the object `dest` iterator
	//! is pointing to,
	template <typename _Iter1, typename _Iter2>
	inline void move_assign(_Iter1 dest, _Iter2 src)
	{
		(*dest) = move(*src);
	}

	namespace Impl
	{
		template <typename _Iter>
		inline void default_construct_range(_Iter first, _Iter last, true_type)	// is_class is true
		{
			for (; first != last; ++first)
			{
				default_construct(first);
			}
		}

		template <typename _Iter>
		inline void default_construct_range(_Iter first, _Iter last, false_type) // is_class is false
		{
			// do nothing.
		}
	}

	//! Default-constructs a range of objects, that is, performs default initialization on each object in the range.
	//! If _Ty is a class type (`is_class<_Ty>::value` is `true_type`), this function calls the default constructor for each
	//! object. Otherwise, this call does nothing, as described by C++ standard.
	//! ref: https://en.cppreference.com/w/cpp/language/default_initialization
	template <typename _Iter>
	inline void default_construct_range(_Iter first, _Iter last)
	{
		using is_value_type_class = typename is_class<typename iterator_traits<_Iter>::value_type>::type;
		return Impl::default_construct_range(first, last, is_value_type_class());
	}

	namespace Impl
	{
		template <typename _Ty>
		inline void value_construct_range(_Ty* first, _Ty* last, true_type) // is a trivial type.
		{
			memzero(static_cast<void*>(first), (size_t)(last - first) * sizeof(_Ty));
		}

		template <typename _Iter>
		inline void value_construct_range(_Iter first, _Iter last, false_type) // is not a trivial type.
		{
			for (; first != last; ++first)
			{
				value_construct(first);
			}
		}
	}

	//! Value-constructs a range of objects, that is, performs value initialization on each object in the range.
	//! This call uses `memzero` to clear all bytes in the storage to 0 if all of the following conditions are matched:
	//! 1. `_Iter` is a pointer type.
	//! 2. `ValueType` of `_Iter` is not a class type.
	//! Otherwise, this function calls the value constructor of each object.
	//! ref: https://en.cppreference.com/w/cpp/language/value_initialization
	//! Todo: For types that satisfy `is_trivially_constructible<typename iterator_traits<_Iter>::value_type>`, when 
	//! the iterator is a pointer, memzero can also be applied.
	template <typename _Iter>
	inline void value_construct_range(_Iter first, _Iter last)
	{
		using is_value_type_trivial = typename conjunction<
			is_pointer<_Iter>,												// `_Iter` is a pointer type.
			negation<is_class<typename iterator_traits<_Iter>::value_type>>	// `ValueType` of `_Iter` is not a class type.
		>::type;
		return Impl::value_construct_range(first, last, is_value_type_trivial());
	}

	namespace Impl
	{
		template<typename _Ty1, typename _Ty2>
		inline _Ty2* copy_construct_range(_Ty1* first, _Ty1* last, _Ty2* d_first, true_type)
		{
			memcpy(static_cast<void*>(d_first), static_cast<const void*>(first), (size_t)(last - first) * sizeof(_Ty2));
			return d_first + (last - first);
		}

		template<typename _Iter1, typename _Iter2>
		inline _Iter2 copy_construct_range(_Iter1 first, _Iter1 last, _Iter2 d_first, false_type)
		{
			for (; first != last; ++d_first, (void) ++first)
			{
				copy_construct(d_first, first);
			}
			return d_first;
		}
	}

	//! Copy-constructs a range of objects.
	//! `memcpy` is used to copy the data directly if:
	//! 1. Both `_Iter1` and `_Iter2` are pointer types.
	//! 2. `ValueType` of `_Iter1` and `_Iter2` is same.
	//! 3. `ValueType` is trivially copy constructible.
	//! Otherwise, the copy constructor is called for every object to construct objects in destination range.
	//! 
	//! The source range and the destination range must not overlap.
	template<typename _Iter1, typename _Iter2>
	inline _Iter2 copy_construct_range(_Iter1 first, _Iter1 last, _Iter2 d_first)
	{
		using is_value_type_trivial = typename conjunction <
			is_pointer<_Iter1>,
			is_pointer<_Iter2>,
			is_same<typename iterator_traits<_Iter1>::value_type, typename iterator_traits<_Iter2>::value_type>,
			is_trivially_copy_constructible<typename iterator_traits<_Iter1>::value_type>
		>::type;

		return Impl::copy_construct_range(first, last, d_first, is_value_type_trivial());
	}

	namespace Impl
	{
		template<typename _Iter1, typename _Iter2>
		inline _Iter2 move_construct_range(_Iter1 first, _Iter1 last, _Iter2 d_first, true_type)	// is trivially move constructible
		{
			return Luna::copy_construct_range(first, last, d_first);
		}

		template<typename _Iter1, typename _Iter2>
		inline _Iter2 move_construct_range(_Iter1 first, _Iter1 last, _Iter2 d_first, false_type)	// is not trivially move constructible
		{
			for (; first != last; ++d_first, (void) ++first)
			{
				move_construct(d_first, first);
			}
		}
	}

	//! Move-constructs a range of objects.
	//! If `ValueType` of `_Iter2` is trivially move constructible, calls `copy_construct_range` to do the move
	//! construct.
	//! Otherwise, the move constructor is called for every object to construct objects in destination range.
	//! 
	//! The source range and the destination range must not overlap.
	template<typename _Iter1, typename _Iter2>
	inline _Iter2 move_construct_range(_Iter1 first, _Iter1 last, _Iter2 d_first)
	{
		using is_value_type_trivial = typename is_trivially_move_constructible<typename iterator_traits<_Iter1>::value_type>::type;
		return Impl::move_construct_range(first, last, d_first, is_value_type_trivial());
	}

	namespace Impl
	{
		template <typename _Iter>
		inline void destruct_range(_Iter first, _Iter last, true_type)	// is_trivially_destructible
		{
			// do nothing.
		}
		template <typename _Iter>
		inline void destruct_range(_Iter first, _Iter last, false_type)	// is_trivially_destructible
		{
			for (; first != last; ++first)
			{
				destruct(first);
			}
		}
	}

	//! Destructs every object in the range.
	//! If `ValueType` is trivially destructible, do nothing.
	//! Else, calls the destructor for each object in the range.
	template <typename _Iter>
	inline void destruct_range(_Iter first, _Iter last)
	{
		using is_value_type_trivial = typename is_trivially_destructible <typename iterator_traits<_Iter>::value_type>::type;
		Impl::destruct_range(first, last, is_value_type_trivial());
	}

	namespace Impl
	{
		template<typename _Ty1, typename _Ty2>
		inline _Ty2* copy_assign_range(_Ty1* first, _Ty1* last, _Ty2* d_first, true_type)
		{
			memcpy(static_cast<void*>(d_first), static_cast<const void*>(first), (size_t)(last - first) * sizeof(_Ty2));
			return d_first + (last - first);
		}

		template<typename _Iter1, typename _Iter2>
		inline _Iter2 copy_assign_range(_Iter1 first, _Iter1 last, _Iter2 d_first, false_type)
		{
			for (; first != last; ++d_first, (void) ++first)
			{
				copy_assign(d_first, first);
			}
			return d_first;
		}
	}

	//! Performs copy assignment operation on every object in the destination range, using the corresponding object in the source range.
	//! `memcpy` is used to copy the data directly if:
	//! 1. Both `_Iter1` and `_Iter2` are pointer types.
	//! 2. `ValueType` of `_Iter1` and `_Iter2` is same.
	//! 3. `ValueType` is trivially copy assignable.
	//! Otherwise, the copy assignment operator is called for every object to copy assign objects in destination range.
	//! 
	//! The source range and the destination range must not overlap.
	template<typename _Iter1, typename _Iter2>
	inline _Iter2 copy_assign_range(_Iter1 first, _Iter1 last, _Iter2 d_first)
	{
		using is_value_type_trivial = typename conjunction <
			is_pointer<_Iter1>,
			is_pointer<_Iter2>,
			is_same<typename iterator_traits<_Iter1>::value_type, typename iterator_traits<_Iter2>::value_type>,
			is_trivially_copy_assignable<typename iterator_traits<_Iter1>::value_type>
		>::type;

		return Impl::copy_assign_range(first, last, d_first, is_value_type_trivial());
	}

	namespace Impl
	{
		template<typename _Ty1, typename _Ty2>
		inline _Ty2* move_assign_range(_Ty1* first, _Ty1* last, _Ty2* d_first, true_type)
		{
			memmove(static_cast<void*>(d_first), static_cast<const void*>(first), (size_t)(last - first) * sizeof(_Ty2));
			return d_first + (last - first);
		}

		template<typename _Iter1, typename _Iter2>
		inline _Iter2 move_assign_range(_Iter1 first, _Iter1 last, _Iter2 d_first, false_type)
		{
			for (; first != last; ++d_first, (void) ++first)
			{
				move_assign(d_first, first);
			}
			return d_first;
		}

		template<typename _Ty1, typename _Ty2>
		inline _Ty2* move_assign_range_backward(_Ty1* first, _Ty1* last, _Ty2* d_last, true_type)
		{
			ptrdiff_t sz = (last - first);
			memmove(static_cast<void*>(d_last - sz), static_cast<const void*>(first), (size_t)(sz) * sizeof(_Ty2));
			return d_last - sz;
		}

		template<typename _Iter1, typename _Iter2>
		inline _Iter2 move_assign_range_backward(_Iter1 first, _Iter1 last, _Iter2 d_last, false_type)
		{
			while (first != last) 
			{
				--d_last;
				--last;
				move_assign(d_last, last);
			}
			return d_last;
		}
	}

	//! Performs move assignment operation on every object in the destination range, using the corresponding object in the source range.
	//! The move operation is performed from first to last, the first element in destination range must not in the source range.
	//! `memmove` is used to move the data directly if:
	//! 1. Both `_Iter1` and `_Iter2` are pointer types.
	//! 2. `ValueType` of `_Iter1` and `_Iter2` is same.
	//! 3. `ValueType` is trivially copy assignable and trivially move assignable. (If the type is trivially move assignable but not trivially copy assignable, the move assignment operator
	//! behaves the same as the copy assignment operator.)
	//! Otherwise, the move assignment operator is called for every object to move assign objects in destination range.
	template<typename _Iter1, typename _Iter2>
	inline _Iter2 move_assign_range(_Iter1 first, _Iter1 last, _Iter2 d_first)
	{
		using is_value_type_trivial = typename conjunction <
			is_pointer<_Iter1>,
			is_pointer<_Iter2>,
			is_same<typename iterator_traits<_Iter1>::value_type, typename iterator_traits<_Iter2>::value_type>,
			is_trivially_copy_assignable<typename iterator_traits<_Iter1>::value_type>,
			is_trivially_move_assignable<typename iterator_traits<_Iter1>::value_type>
		>::type;

		return Impl::move_assign_range(first, last, d_first, is_value_type_trivial());
	}

	//! Same as `move_assign_range`, but performs the move assign from back to front.
	//! The last element in destination range must not in the source range.
	template<typename _Iter1, typename _Iter2>
	inline _Iter2 move_assign_range_backward(_Iter1 first, _Iter1 last, _Iter2 d_last)
	{
		using is_value_type_trivial = typename conjunction <
			is_pointer<_Iter1>,
			is_pointer<_Iter2>,
			is_same<typename iterator_traits<_Iter1>::value_type, typename iterator_traits<_Iter2>::value_type>,
			is_trivially_copy_assignable<typename iterator_traits<_Iter1>::value_type>,
			is_trivially_move_assignable<typename iterator_traits<_Iter1>::value_type>
		>::type;

		return Impl::move_assign_range_backward(first, last, d_last, is_value_type_trivial());
	}

	//! Performs copy construct on each of the object in the range by taking a copy of the provided object.
	template <typename _Iter1, typename _Ty>
	inline _Iter1 fill_construct_range(_Iter1 first, _Iter1 last, const _Ty& value)
	{
		for (; first != last; ++first) 
		{
			new (static_cast<void*>(addressof(*first)))
				typename iterator_traits<_Iter1>::value_type(value);
		}
		return first;
	}

	//! Performs copy assignment on each of the object in the range by taking a copy of the provided object.
	template <typename _Iter1, typename _Ty>
	inline _Iter1 fill_assign_range(_Iter1 first, _Iter1 last, const _Ty& value)
	{
		for (; first != last; ++first) 
		{
			*first = value;
		}
		return first;
	}

	namespace Impl
	{
		template <typename _Ty1, typename _Ty2>
		inline _Ty2* copy_relocate_range_virtual(_Ty1* first, _Ty1* last, _Ty2* d_first, true_type)	// is_pointer
		{
			memcpy(static_cast<void*>(d_first), static_cast<const void*>(first), (size_t)(last - first) * sizeof(_Ty2));
			return d_first + (last - first);
		}

		template <typename _Iter1, typename _Iter2>
		inline _Iter2 copy_relocate_range_virtual(_Iter1 first, _Iter1 last, _Iter2 d_first, false_type) // not is_pointer
		{
			for (; first != last; ++d_first, (void) ++first)
			{
				memcpy(static_cast<void*>(addressof(*d_first)), static_cast<const void*>(addressof(*first)), sizeof(typename iterator_traits<_Iter2>::value_type));
			}
			return d_first;
		}

		template <typename _Iter1, typename _Iter2>
		inline _Iter2 copy_relocate_range(_Iter1 first, _Iter1 last, _Iter2 d_first, true_type)	// is_trivially_relocatable
		{
			using is_iterator_pointer = typename conjunction<is_pointer<_Iter1>, is_pointer<_Iter2>>::type;
			return copy_relocate_range_virtual(first, last, d_first, is_iterator_pointer());
		}

		template <typename _Iter1, typename _Iter2>
		inline _Iter2 copy_relocate_range(_Iter1 first, _Iter1 last, _Iter2 d_first, false_type)// not is_trivially_relocatable
		{
			for (; first != last; ++d_first, (void) ++first)
			{
				move_construct(d_first, first);
				destruct(first);
			}
			return d_first;
		}
	}

	//! Relocates objects in the source range to a new range that is not overlap with the source range.
	//! After this call, the objects in the destination range behaves the same as the corresponding objects 
	//! formerly in the source range, except that the place(memory address) for the object is changed.
	//! 
	//! The iterator for the source range and destination range must have the same `ValueType`.
	//! All objects in the destination range must in uninitialized state before this call. And after this call,
	//! all objects in the source range is in uninitialized state.
	//! 
	//! This call behaves differently in the following different conditions:
	//! 
	//! 1. If `ValueType` of `_Iter1` is trivially relocatable (which is always true unless the user explicitly
	//! demonstrates), `memcpy` is used to copy the data from source place to destination place. If `_Iter` and 
	//! `_Iter2` is both pointer types, the full range will be copied by `memcpy` in one call, otherwise, `memcpy`
	//! will be called once per object. No constructors and destructors are called in this period.
	//! 
	//! 2. If `ValueType` of `_Iter1` is not trivially relocatable, for every object in the destination range, 
	//! the move constructor will be called with the corresponding object in the source range passed in, then 
	//! the destructor of the corresponding object in the source range will be called.
	template <typename _Iter1, typename _Iter2>
	inline _Iter2 copy_relocate_range(_Iter1 first, _Iter1 last, _Iter2 d_first)
	{
		using is_value_type_trivial = typename is_trivially_relocatable<typename iterator_traits<_Iter1>::value_type>::type;
		
		return Impl::copy_relocate_range(first, last, d_first, is_value_type_trivial());
	}

	namespace Impl
	{
		template <typename _Ty1, typename _Ty2>
		inline _Ty2* move_relocate_range_virtual(_Ty1* first, _Ty1* last, _Ty2* d_first, true_type)	// is_pointer
		{
			memmove(static_cast<void*>(d_first), static_cast<const void*>(first), (size_t)(last - first) * sizeof(_Ty2));
			return d_first + (last - first);
		}

		template <typename _Iter1, typename _Iter2>
		inline _Iter2 move_relocate_range_virtual(_Iter1 first, _Iter1 last, _Iter2 d_first, false_type) // not is_pointer
		{
			for (; first != last; ++d_first, (void) ++first)
			{
				memcpy(static_cast<void*>(addressof(*d_first)), static_cast<const void*>(addressof(*first)), sizeof(typename iterator_traits<_Iter2>::value_type));
			}
			return d_first;
		}

		template <typename _Iter1, typename _Iter2>
		inline _Iter2 move_relocate_range(_Iter1 first, _Iter1 last, _Iter2 d_first, true_type)	// is_trivially_relocatable
		{
			using is_iterator_pointer = typename conjunction<is_pointer<_Iter1>, is_pointer<_Iter2>>::type;
			return move_relocate_range_virtual(first, last, d_first, is_iterator_pointer());
		}

		template <typename _Iter1, typename _Iter2>
		inline _Iter2 move_relocate_range(_Iter1 first, _Iter1 last, _Iter2 d_first, false_type)// not is_trivially_relocatable
		{
			for (; first != last; ++d_first, (void) ++first)
			{
				move_construct(d_first, first);
				destruct(first);
			}
			return d_first;
		}
	}

	//! This function behaves the same as `copy_relocate_range`, except that it allows the destination range 
	//! overlaps with the source range, proved that the first object in the destination range does not in the source range.
	//! `memmove` is used instead of `memcpy` if the whole range can be relocated in one call.
	template <typename _Iter1, typename _Iter2>
	inline _Iter2 move_relocate_range(_Iter1 first, _Iter1 last, _Iter2 d_first)
	{
		using is_value_type_trivial = typename is_trivially_relocatable<typename iterator_traits<_Iter1>::value_type>::type;

		return Impl::move_relocate_range(first, last, d_first, is_value_type_trivial());
	}

	namespace Impl
	{
		template <typename _Ty1, typename _Ty2>
		inline _Ty2* move_relocate_range_backward_trivial(_Ty1* first, _Ty1* last, _Ty2* d_last, true_type)	// iterator is_pointer
		{
			isize sz = last - first;
			memmove(static_cast<void*>(d_last - sz), static_cast<const void*>(first), (size_t)(sz) * sizeof(_Ty2));
			return d_last - sz;
		}

		template <typename _Iter1, typename _Iter2>
		inline _Iter2 move_relocate_range_backward_trivial(_Iter1 first, _Iter1 last, _Iter2 d_last, false_type) // iterator not is_pointer
		{
			while (first != last)
			{
				--last;
				--d_last;
				memcpy(static_cast<void*>(addressof(*d_last)), static_cast<const void*>(addressof(*last)), sizeof(typename iterator_traits<_Iter2>::value_type));
			}
			return d_last;
		}

		template <typename _Iter1, typename _Iter2>
		inline _Iter2 move_relocate_range_backward(_Iter1 first, _Iter1 last, _Iter2 d_last, true_type)	// is_trivially_relocatable
		{
			using is_iterator_pointer = typename conjunction<is_pointer<_Iter1>, is_pointer<_Iter2>>::type;
			return move_relocate_range_backward_trivial(first, last, d_last, is_iterator_pointer());
		}

		template <typename _Iter1, typename _Iter2>
		inline _Iter2 move_relocate_range_backward(_Iter1 first, _Iter1 last, _Iter2 d_last, false_type)// not is_trivially_relocatable
		{
			while (first != last)
			{
				--last;
				--d_last;
				move_construct(d_last, last);
				destruct(last);
			}
			return d_last;
		}
	}

	//! This function behaves the same as `move_relocate_range`, except that it relocates object from last to first, so
	//! the last object in the destination range should not in the source range.
	//! `memmove` is used instead of `memcpy` if the whole range can be relocated in one call.
	template <typename _Iter1, typename _Iter2>
	inline _Iter2 move_relocate_range_backward(_Iter1 first, _Iter1 last, _Iter2 d_last)
	{
		using is_value_type_trivial = typename is_trivially_relocatable<typename iterator_traits<_Iter1>::value_type>::type;

		return Impl::move_relocate_range_backward(first, last, d_last, is_value_type_trivial());
	}
}
