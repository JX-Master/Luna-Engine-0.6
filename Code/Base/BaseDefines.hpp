// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file BaseDefines.hpp
* @author JXMaster
* @date 2018/10/26
* @brief Defines basic types and functions to be used in Luna Engine.
 */
#pragma once

#include "PlatformDefines.hpp"

#include <cstdint>
#include <cfloat>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <new>
#include <type_traits>
#include <initializer_list>

namespace luna
{
	using int8 = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
	using int64 = std::int64_t;

	using uint8 = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;

#ifndef LUNA_COMPILER_CPP20
	using char8_t = char;
	static_assert(sizeof(char8_t) == 1, "Wrong char8_t size");
#endif

	using float32 = float;
	using float64 = double;

	static_assert(sizeof(float32) == 4, "Wrong float32 size");
	static_assert(sizeof(float64) == 8, "Wrong float64 size");

	// From cstddef

	using std::size_t;
	using std::ptrdiff_t;
	using std::max_align_t;
	using std::nullptr_t;

	// From cstdio
	using std::sprintf;
	using std::snprintf;
	using std::vsprintf;
	using std::vsnprintf;

	// Prefer using constexpr instead of macros because constexpr 
	// is typed rather than a plain number.
	constexpr int8 int8_max_v = (int8)INT8_MAX;
	constexpr int8 int8_min_v = (int8)INT8_MIN;
	constexpr int16 int16_max_v = (int16)INT16_MAX;
	constexpr int16 int16_min_v = (int16)INT16_MIN;
	constexpr int32 int32_max_v = (int32)INT32_MAX;
	constexpr int32 int32_min_v = (int32)INT32_MIN;
	constexpr int64 int64_max_v = (int64)INT64_MAX;
	constexpr int64 int64_min_v = (int64)INT64_MIN;
	constexpr uint8 uint8_max_v = (uint8)UINT8_MAX;
	constexpr uint16 uint16_max_v = (uint16)UINT16_MAX;
	constexpr uint32 uint32_max_v = (uint32)UINT32_MAX;
	constexpr uint64 uint64_max_v = (uint64)UINT64_MAX;
#ifdef LUNA_PLATFORM_64BIT
	constexpr size_t size_t_max_v = uint64_max_v;
	constexpr ptrdiff_t ptrdiff_t_max_v = int64_max_v;
#else
	constexpr size_t size_t_max_v = uint32_max_v;
	constexpr ptrdiff_t ptrdiff_t_max_v = int32_max_v;
#endif
	constexpr size_t max_align_v = alignof(max_align_t);

	//! byte_t type is used to declare that you want treat the data as raw binary data.
	using byte_t = uint8;
	//! handlt_t type is used to represent a opaque pointer to a specified resource.
	using handle_t = void*;

	inline constexpr uint64 operator"" _kb(uint64 v)
	{
		return v * 1024;
	}

	inline constexpr uint64 operator"" _mb(uint64 v)
	{
		return v * 1024 * 1024;
	}

	inline constexpr uint64 operator"" _gb(uint64 v)
	{
		return v * 1024 * 1024 * 1024;
	}

	inline constexpr uint64 operator"" _tb(uint64 v)
	{
		return v * 1024 * 1024 * 1024 * 1024;
	}

	//! `result_t` is used to report a success case or an failure case for the function invoking to the caller.
	//! A failure/error is the case when the function cannot successfully do what it is told to do. If one function
	//! may fail, it must reports if the operation fails by returning `result_t` type. Any non-negative value means 
	//! success, and any negative value means failure.
	//! 
	//! The value form -32768 to 32767 is reserved for common error/success codes used widely through all modules, the value
	//! -32769 and smaller or 32768 and greater can be used by every module to define module-specific result codes. Such
	//! return value can only be used in Result Objects, see docs for details about how to report and handle errors properly.
	using result_t = int32;

	//! Checks if the return value is a success value.
	inline constexpr bool succeeded(result_t r)
	{
		return r >= 0;
	}

	//! Checks if the return value is a failure value.
	inline constexpr bool failed(result_t r)
	{
		return r < 0;
	}

	//-------------------------- General Return Value [-32768 ~ 32767] --------------------------
	//! General success returns. Success values are 0 and above.
	constexpr result_t s_ok = 0;
	//! The operation is already done and it does not need to be done a second time.
	//! This is not regarded as an error.
	constexpr result_t s_already_done = 1;
	//! Returned by `ILuaState::resume` function when the calling coroutine is yielded (not returned).
	constexpr result_t s_lua_yield = 2;
	//! The specified element already exists.
	constexpr result_t s_item_already_exists = 3;

	//! General failure returns. The user should avoid returning generic `e_failure` if it can determine
	//! what is going wrong and can report that, since returning `e_failure` does not provide any information 
	//! for the caller to handle the error.
	constexpr result_t e_failure = -1;
	//! The specified item does not exist.
	constexpr result_t e_item_not_exist = -2;
	//! The specified item already exists.
	constexpr result_t e_item_already_exists = -3;
	//! Invalid arguments are specified.
	constexpr result_t e_bad_arguments = -4;
	//! The function is called in improper time, like trying to reset the resource when the resource is still using.
	constexpr result_t e_bad_calling_time = -5;
	//! Memory allocation failed.
	constexpr result_t e_bad_memory_alloc = -6;
	//! This call cannot be handled by this object. Note that this differs from `e_bad_calling_time`, if one call 
	//! returns `e_not_available`, any further call to the same object returns `e_not_available`. In most cases
	//! this means this call is not implemented by this object.
	constexpr result_t e_not_available = -7;
	//! A call to the underlying operating system is failed and the reason cannot be represented by any other
	//! result code.
	constexpr result_t e_bad_system_call = -8;
	//! The specified file does not exist.
	constexpr result_t e_file_not_exist = -9;
	//! The specified file already exists.
	constexpr result_t e_file_already_exists = -10;
	//! The access to the file or resource is denied.
	constexpr result_t e_access_denied = -11;
	//! The specified directory is not found.
	constexpr result_t e_dir_not_exist = -12;
	//! The specified directory already exists.
	constexpr result_t e_dir_already_exists = -13;
	//! The memory buffer provided by the user is not large enough to contain all returned elements.
	constexpr result_t e_insufficient_buffer = -14;
	//! The operation is pending in another thread and is not finished yet.
	constexpr result_t e_pending = -15;
	//! The type required to finish this call does not match the actual type the object is.
	constexpr result_t e_type_dismatch = -16;
	//! The number or container overflows.
	constexpr result_t e_overflow = -17;
	//! The query_interface or cast_interface operation failed because the interface queried is not supported.
	constexpr result_t e_interface_not_supported = -18;
	//! The object is expired. (IObject::expired returns `true`)
	constexpr result_t e_expired = -19;
	//! The operation fails because a runtime error is triggered in Lua state.
	constexpr result_t e_lua_errrun = -20;
	//! The operation fails because a runtime error is triggered while running the message handler. 
	constexpr result_t e_lua_errerr = -21;
	//! The operation fails because a runtime error is triggered while running a lua __gc metamethod. 
	//! For such errors, Lua does not call the message handler (as this kind of error typically has 
	//! no relation with the function being called). 
	constexpr result_t e_lua_errgcmm = -22;
	//! Syntax error detected when parsing scripts.
	constexpr result_t e_syntax_error = -23;
	//! The operation is canceled by user.
	constexpr result_t e_user_canceled = -24;

	//! The operation succeeded and the result value is returned by the Result Object. See docs for using and
	//! handling results returned by Result Object.
	constexpr result_t s_user_success = int16_max_v;

	//! The operation failed and the result value is returned by Result Object. See docs for using and
	//! handling results returned by Result Object.
	constexpr result_t e_user_failure = int16_min_v;

	// From type_traits.

	using std::integral_constant;
	using std::true_type;
	using std::false_type;

	using std::is_void;
	using std::is_null_pointer;
	using std::is_integral;
	using std::is_floating_point;
	using std::is_array;
	using std::is_enum;
	using std::is_union;
	using std::is_class;
	using std::is_function;
	using std::is_pointer;
	using std::is_lvalue_reference;
	using std::is_rvalue_reference;
	using std::is_member_object_pointer;
	using std::is_member_function_pointer;

	template<class _T> constexpr bool is_void_v = is_void<_T>::value;
	template<class _T> constexpr bool is_null_pointer_v = is_null_pointer<_T>::value;
	template<class _T> constexpr bool is_integral_v = is_integral<_T>::value;
	template<class _T> constexpr bool is_floating_point_v = is_floating_point<_T>::value;
	template<class _T> constexpr bool is_enum_v = is_enum<_T>::value;
	template<class _T> constexpr bool is_union_v = is_union<_T>::value;
	template<class _T> constexpr bool is_class_v = is_class<_T>::value;
	template<class _T> constexpr bool is_function_v = is_function<_T>::value;
	template<class _T> constexpr bool is_pointer_v = is_pointer<_T>::value;
	template<class _T> constexpr bool is_lvalue_reference_v = is_lvalue_reference<_T>::value;
	template<class _T> constexpr bool is_rvalue_reference_v = is_rvalue_reference<_T>::value;
	template<class _T> constexpr bool is_member_object_pointer_v = is_member_object_pointer<_T>::value;
	template<class _T> constexpr bool is_member_function_pointer_v = is_member_function_pointer<_T>::value;

	using std::is_fundamental;
	using std::is_arithmetic;
	using std::is_scalar;
	using std::is_object;
	using std::is_compound;
	using std::is_reference;
	using std::is_member_pointer;

	template<class _T> constexpr bool is_fundamental_v = is_fundamental<_T>::value;
	template<class _T> constexpr bool is_arithmetic_v = is_arithmetic<_T>::value;
	template<class _T> constexpr bool is_scalar_v = is_scalar<_T>::value;
	template<class _T> constexpr bool is_object_v = is_object<_T>::value;
	template<class _T> constexpr bool is_compound_v = is_compound<_T>::value;
	template<class _T> constexpr bool is_reference_v = is_reference<_T>::value;
	template<class _T> constexpr bool is_member_pointer_v = is_member_pointer<_T>::value;

	using std::is_const;
	using std::is_volatile;
	using std::is_trivial;
	using std::is_trivially_copyable;
	using std::is_standard_layout;
	using std::is_pod;
	//using std::is_literal_type;
	using std::is_empty;
	using std::is_polymorphic;
	using std::is_abstract;
	using std::is_final;
	using std::is_signed;
	using std::is_unsigned;

	template<class _T> constexpr bool is_const_v = is_const<_T>::value;
	template<class _T> constexpr bool is_volatile_v = is_volatile<_T>::value;
	template<class _T> constexpr bool is_trivial_v = is_trivial<_T>::value;
	template<class _T> constexpr bool is_trivially_copyable_v = is_trivially_copyable<_T>::value;
	template<class _T> constexpr bool is_standard_layout_v = is_standard_layout<_T>::value;
	template<class _T> constexpr bool is_pod_v = is_pod<_T>::value;
	//template<class _T> constexpr bool is_literal_type_v = is_literal_type<_T>::value;
	template<class _T> constexpr bool is_empty_v = is_empty<_T>::value;
	template<class _T> constexpr bool is_polymorphic_v = is_polymorphic<_T>::value;
	template<class _T> constexpr bool is_abstract_v = is_abstract<_T>::value;
	template<class _T> constexpr bool is_final_v = is_final<_T>::value;
	template<class _T> constexpr bool is_signed_v = is_signed<_T>::value;
	template<class _T> constexpr bool is_unsigned_v = is_unsigned<_T>::value;

	using std::is_constructible;
	using std::is_trivially_constructible;
	using std::is_nothrow_constructible;
	using std::is_default_constructible;
	using std::is_trivially_default_constructible;
	using std::is_nothrow_default_constructible;
	using std::is_copy_constructible;
	using std::is_trivially_copy_constructible;
	using std::is_nothrow_copy_constructible;
	using std::is_move_constructible;
	using std::is_trivially_move_constructible;
	using std::is_nothrow_move_constructible;
	using std::is_assignable;
	using std::is_trivially_assignable;
	using std::is_nothrow_assignable;
	using std::is_copy_assignable;
	using std::is_trivially_copy_assignable;
	using std::is_nothrow_copy_assignable;
	using std::is_move_assignable;
	using std::is_trivially_move_assignable;
	using std::is_nothrow_move_assignable;
	using std::is_destructible;
	using std::is_trivially_destructible;
	using std::is_nothrow_destructible;
	using std::has_virtual_destructor;

	template<class _T> constexpr bool is_constructible_v = is_constructible<_T>::value;
	template<class _T> constexpr bool is_trivially_constructible_v = is_trivially_constructible<_T>::value;
	template<class _T> constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<_T>::value;
	template<class _T> constexpr bool is_default_constructible_v = is_default_constructible<_T>::value;
	template<class _T> constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<_T>::value;
	template<class _T> constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<_T>::value;
	template<class _T> constexpr bool is_copy_constructible_v = is_copy_constructible<_T>::value;
	template<class _T> constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<_T>::value;
	template<class _T> constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<_T>::value;
	template<class _T> constexpr bool is_move_constructible_v = is_move_constructible<_T>::value;
	template<class _T> constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<_T>::value;
	template<class _T> constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<_T>::value;
	template<class _T> constexpr bool is_assignable_v = is_assignable<_T>::value;
	template<class _T> constexpr bool is_trivially_assignable_v = is_trivially_assignable<_T>::value;
	template<class _T> constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<_T>::value;
	template<class _T> constexpr bool is_copy_assignable_v = is_copy_assignable<_T>::value;
	template<class _T> constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<_T>::value;
	template<class _T> constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<_T>::value;
	template<class _T> constexpr bool is_move_assignable_v = is_move_assignable<_T>::value;
	template<class _T> constexpr bool is_trivially_move_assignable_v = is_trivially_move_assignable<_T>::value;
	template<class _T> constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<_T>::value;
	template<class _T> constexpr bool is_destructible_v = is_destructible<_T>::value;
	template<class _T> constexpr bool is_trivially_destructible_v = is_trivially_destructible<_T>::value;
	template<class _T> constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<_T>::value;
	template<class _T> constexpr bool has_virtual_destructor_v = has_virtual_destructor<_T>::value;

	using std::is_same;
	using std::is_base_of;
	using std::is_convertible;
	template<class _T> constexpr bool is_same_v = is_same<_T>::value;
	template<class _T> constexpr bool is_base_of_v = is_base_of<_T>::value;
	template<class _T> constexpr bool is_convertible_v = is_convertible<_T>::value;

	using std::remove_cv;
	using std::remove_const;
	using std::remove_volatile;
	using std::remove_cv_t;
	using std::remove_const_t;
	using std::remove_volatile_t;

	using std::add_cv;
	using std::add_const;
	using std::add_volatile;
	using std::add_cv_t;
	using std::add_const_t;
	using std::add_volatile_t;

	using std::remove_reference;
	using std::remove_reference_t;
	using std::add_lvalue_reference;
	using std::add_lvalue_reference_t;
	using std::add_rvalue_reference;
	using std::add_rvalue_reference_t;

	using std::remove_pointer;
	using std::remove_pointer_t;
	using std::add_pointer;
	using std::add_pointer_t;
	using std::make_signed;
	using std::make_signed_t;
	using std::make_unsigned;
	using std::make_unsigned_t;
	using std::remove_extent;
	using std::remove_extent_t;
	using std::remove_all_extents;
	using std::remove_all_extents_t;

	using std::aligned_storage;
	using std::aligned_storage_t;
	using std::aligned_union;
	using std::aligned_union_t;
	using std::decay;
	using std::decay_t;
	using std::enable_if;
	using std::enable_if_t;
	using std::conditional;
	using std::conditional_t;
	using std::common_type;
	using std::common_type_t;
	using std::underlying_type;
	using std::underlying_type_t;
	//using std::result_of;
	//using std::result_of_t;

	namespace impl
	{
		template <typename _Ty>
		inline constexpr bool is_over_aligned()
		{
			return alignof(_Ty) > max_align_v;
		}
	}

	//! Checks if the specified type's alignment requirement is greater than alignof(max_align_t).
	template <typename _Ty>
	struct is_over_aligned : integral_constant<bool, impl::is_over_aligned<_Ty>()> {};

	template<typename _Ty>
	constexpr bool is_over_aligned_v = is_over_aligned<_Ty>::value;

	// Introduced in C++17, but is very useful.

	using std::conjunction;
	using std::disjunction;
	using std::negation;

	template<typename... _B>
	constexpr bool conjunction_v = conjunction<_B...>::value;
	template<typename... _B>
	constexpr bool disjunction_v = disjunction<_B...>::value;
	template<typename _B>
	constexpr bool negation_v = negation<_B>::value;

	// Define forward and move.

	template <typename _Ty>
	constexpr _Ty&& forward(typename remove_reference<_Ty>::type& x)
	{
		return static_cast<_Ty&&>(x);
	}

	template <typename _Ty>
	constexpr _Ty&& forward(typename remove_reference<_Ty>::type&& x)
	{
		static_assert(!is_lvalue_reference<_Ty>::value, "forward _Ty isn't lvalue reference");
		return static_cast<_Ty&&>(x);
	}

	template <typename _Ty>
	constexpr typename remove_reference<_Ty>::type&& move(_Ty&& x)
	{
		return ((typename remove_reference<_Ty>::type&&)x);
	}

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
		alignas(_Ty) byte_t m_buffer[sizeof(_Ty)];
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

	// From cstring.

	using std::strcpy;
	using std::strncpy;
	using std::strcat;
	using std::strncat;
	using std::strxfrm;
	using std::strlen;
	using std::strcmp;
	using std::strncmp;
	using std::strcoll;
	using std::strchr;
	using std::strrchr;
	using std::strspn;
	using std::strcspn;
	using std::strpbrk;
	using std::strstr;
	using std::strtok;
	using std::memchr;
	using std::memcmp;
	using std::memset;
	using std::memcpy;
	using std::memmove;

	// From initializer_list.

	using std::initializer_list;

	//! Checks if the specified type can be trivially relocatable.
	//! One object is trivially relocatable if the data of one well-constructed instance of object can be copied to a new 
	//! uninitialized memory by bitwise copy (for example, `memcpy`, `memmove` or `realloc`) and the new copied object behaves
	//! the same as the original object, such copy is called a "relocating operation". After the operation, the original memory
	//! for the object is treated as uninitialized and does not have destructor called before it is freed.
	//! 
	//! If one object can be trivially relocated, then when the memory that holding the object needs to be reallocated (for example
	//! when the container needs to expand its capacity), it performs `memcpy`, `memmove` or any other bitwise copy algorithms 
	//! directly on the data without any move constructors and destructors being called. If one object cannot be trivially relocated,
	//! then the relocating operation will call the move constructor for the object on the new memory (passing the old object as rvalue 
	//! reference), then call the destructor for the old object to properly destruct itself before freeing the memory for the old object.
	//! 
	//! All objects are trivially relocatable unless the user explicitly creates a template specialization that evaluates this to 
	//! `false_type` for the object that cannot be trivially relocated. In fact, the case that one type cannot be trivially relocatable
	//! is very rare, this only happens if the type holds a pointer to `this` and needs to update the pointer if the object is relocated.
	template <typename _Ty>
	struct is_trivially_relocatable : integral_constant<bool, true> {};

	template <typename _Ty>
	constexpr bool is_trivially_relocatable_v = is_trivially_relocatable<_Ty>::value;

	// Grants bitwise operations for all enum class types. The operations may be invalid for non-bitwise enum class types,
	// but the users will not mess up with that.

	template <typename _Ty>
	constexpr auto operator|(_Ty const left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		return static_cast<_Ty>(static_cast<underlying_type_t<_Ty>>(left) | static_cast<underlying_type_t<_Ty>>(right));
	}

	template <typename _Ty>
	constexpr auto operator|=(_Ty& left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		left = left | right;
		return left;
	}

	template <typename _Ty>
	constexpr auto operator&(_Ty const left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		return static_cast<_Ty>(static_cast<underlying_type_t<_Ty>>(left) & static_cast<underlying_type_t<_Ty>>(right));
	}

	template <typename _Ty>
	constexpr auto operator&=(_Ty& left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		left = left & right;
		return left;
	}

	template <typename _Ty>
	constexpr auto operator~(_Ty const value) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		return static_cast<_Ty>(~static_cast<underlying_type_t<_Ty>>(value));
	}

	template <typename _Ty>
	constexpr auto operator^(_Ty const left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		return static_cast<_Ty>(static_cast<underlying_type_t<_Ty>>(left) ^ static_cast<underlying_type_t<_Ty>>(right));
	}

	template <typename _Ty>
	constexpr auto operator^=(_Ty& left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		left = left ^ right;
		return left;
	}

	// bit ops library.

	namespace impl
	{
		constexpr const uint8 bit_mask[] = {
			0x01,	// 00000001
			0x02,	// 00000010
			0x04,	// 00000100
			0x08,	// 00001000
			0x10,	// 00010000
			0x20,	// 00100000
			0x40,	// 01000000
			0x80	// 10000000
		};

		constexpr const uint8 bit_mask_reverse[] = {
			0xfe,	// 11111110
			0xfd,	// 11111101
			0xfb,	// 11111011
			0xf7,	// 11110111
			0xef,	// 11101111
			0xdf,	// 11011111
			0xbf,	// 10111111
			0x7f	// 01111111
		};
	}

	//! Tests the specified bit. Returns `true` if the bit is 1, `false` if the bit is 0.
	inline bool bit_test(const void* addr, size_t bit_offset)
	{
		return (*(const uint8*)((size_t)addr + bit_offset / 8)) & impl::bit_mask[bit_offset % 8] ? true : false;;
	}

	//! Sets the specified bit in the position to 1.
	inline void bit_set(void* addr, size_t bit_offset)
	{
		*(uint8*)((size_t)addr + bit_offset / 8) |= impl::bit_mask[bit_offset % 8];
	}

	//! Sets the specified bit in the position to 0.
	inline void bit_reset(void* addr, size_t bit_offset)
	{
		*(uint8*)((size_t)addr + bit_offset / 8) &= impl::bit_mask_reverse[bit_offset % 8];
	}

	//! Returns the address/size that aligns the origin address/size to the nearest matched aligned 
	//! address/size that is greater than or equal to the the origin address/size.
	//! @param[in] origin The unaligned address/size.
	//! @param[in] alignment The alignment boundary.
	//! @return Returns the aligned address/size.
	inline constexpr size_t align_upper(size_t origin, size_t alignment)
	{
		return alignment ? ((origin + alignment - 1) / alignment * alignment) : origin;
	}

	//! decode one hex char to corresponding number. Returns 0 if the provided char is not a hex char.
	inline constexpr uint8 atohex(const char x)
	{
		return x >= '0' && x <= '9' ? x - '0' :
			x >= 'a' && x <= 'f' ? x - 'a' + 10 :
			x >= 'A' && x <= 'F' ? x - 'A' + 10 : 0;
	}
	static_assert(atohex('3') == 0x03, "atohex Wrong");
	static_assert(atohex('e') == 0x0e, "atohex Wrong");
	static_assert(atohex('C') == 0x0C, "atohex Wrong");

	struct Guid
	{
	public:
		uint64 high;
		uint64 low;
	private:
		constexpr uint64 parse_high(const char* s)
		{
			s = (*s == '{') ? s + 1 : s;
			return (uint64)(atohex(s[0])) << 60 |
				(uint64)(atohex(s[1])) << 56 |
				(uint64)(atohex(s[2])) << 52 |
				(uint64)(atohex(s[3])) << 48 |
				(uint64)(atohex(s[4])) << 44 |
				(uint64)(atohex(s[5])) << 40 |
				(uint64)(atohex(s[6])) << 36 |
				(uint64)(atohex(s[7])) << 32 |
				(uint64)(atohex(s[9])) << 28 |
				(uint64)(atohex(s[10])) << 24 |
				(uint64)(atohex(s[11])) << 20 |
				(uint64)(atohex(s[12])) << 16 |
				(uint64)(atohex(s[14])) << 12 |
				(uint64)(atohex(s[15])) << 8 |
				(uint64)(atohex(s[16])) << 4 |
				(uint64)(atohex(s[17]));
		}

		constexpr uint64 parse_low(const char* s)
		{
			s = (*s == '{') ? s + 1 : s;
			return (uint64)(atohex(s[19])) << 60 |
				(uint64)(atohex(s[20])) << 56 |
				(uint64)(atohex(s[21])) << 52 |
				(uint64)(atohex(s[22])) << 48 |
				(uint64)(atohex(s[24])) << 44 |
				(uint64)(atohex(s[25])) << 40 |
				(uint64)(atohex(s[26])) << 36 |
				(uint64)(atohex(s[27])) << 32 |
				(uint64)(atohex(s[28])) << 28 |
				(uint64)(atohex(s[29])) << 24 |
				(uint64)(atohex(s[30])) << 20 |
				(uint64)(atohex(s[31])) << 16 |
				(uint64)(atohex(s[32])) << 12 |
				(uint64)(atohex(s[33])) << 8 |
				(uint64)(atohex(s[34])) << 4 |
				(uint64)(atohex(s[35]));
		}
	public:
		constexpr Guid() : high(0), low(0) {}
		constexpr Guid(uint64 h, uint64 l) : high(h), low(l) {}
		constexpr Guid(const char* s) :
			high(parse_high(s)), low(parse_low(s)) {}
		constexpr Guid(const Guid& rhs) : high(rhs.high), low(rhs.low) {}

		constexpr bool operator==(const Guid& rhs) const
		{
			return low == rhs.low && high == rhs.high;
		}
		constexpr bool operator!=(const Guid& rhs) const
		{
			return low != rhs.low || high != rhs.high;
		}
		constexpr bool operator<(const Guid& rhs) const
		{
			return (high != rhs.high) ? (high < rhs.high) : (low < rhs.low);
		}
		constexpr Guid& operator&=(const Guid& rhs)
		{
			low &= rhs.low;
			high &= rhs.high;
			return *this;
		}
		constexpr Guid& operator|=(const Guid& rhs)
		{
			low |= rhs.low;
			high |= rhs.high;
			return *this;
		}
		constexpr Guid& operator^=(const Guid& rhs)
		{
			low ^= rhs.low;
			high ^= rhs.high;
			return *this;
		}
	};

	constexpr Guid operator&(const Guid& lhs, const Guid& rhs)
	{
		return Guid(lhs.high & rhs.high, lhs.low & rhs.low);
	}

	constexpr Guid operator|(const Guid& lhs, const Guid& rhs)
	{
		return Guid(lhs.high | rhs.high, lhs.low | rhs.low);
	}

	constexpr Guid operator^(const Guid& lhs, const Guid& rhs)
	{
		return Guid(lhs.high ^ rhs.high, lhs.low ^ rhs.low);
	}

	static_assert(sizeof(Guid) == 16, "Wrong Guid size");

	static_assert(Guid("{12345678-1234-5678-9ABC-123456789ABC}").low == 0x9ABC123456789ABC, "Guid function is incorrect.");
	static_assert(Guid("{12345678-1234-5678-9ABC-123456789ABC}").high == 0x1234567812345678, "Guid function is incorrect.");

	//! The system result code domain.
	constexpr Guid err_domain_base{ "{48f61739-e0fd-472e-bf06-e5a0c7c3208b}" };

	//! @struct Pair
	//! Stores a pair of values. This struct is ABI-compatible and can be used as parameters for interface methods, whether by value, by pointer
	//! or by reference.
	template <typename _Ty1, typename _Ty2>
	struct Pair
	{
		using first_type = _Ty1;
		using second_type = _Ty2;
		// -------------------- Begin of ABI compatible part --------------------
		first_type first;
		second_type second;
		// --------------------  End of ABI compatible part  --------------------

		constexpr Pair() = default;

		constexpr Pair(const _Ty1& x, const _Ty2& y) :
			first(x), second(y) {}

		template <typename _U1, typename _U2>
		constexpr Pair(_U1&& x, _U2&& y) :
			first(forward<_U1>(x)), second(forward<_U2>(y)) {}

		template <typename _U1, typename _U2>
		constexpr Pair(const Pair<_U1, _U2>& p) :
			first(p.first), second(p.second) {}

		template <typename _U1, typename _U2>
		constexpr Pair(Pair<_U1, _U2>&& p) :
			first(move(p.first)), second(move(p.second)) {}

		Pair(const Pair&) = default;
		Pair(Pair&&) = default;
		constexpr Pair& operator=(const Pair& rhs)
		{
			first = rhs.first;
			second = rhs.second;
			return *this;
		}
		template <typename _U1, typename _U2>
		constexpr Pair& operator=(const Pair<_U1, _U2>& rhs)
		{
			first = rhs.first;
			second = rhs.second;
			return *this;
		}
		constexpr Pair& operator=(Pair&& rhs)
		{
			first = move(rhs.first);
			second = move(rhs.second);
			return *this;
		}
		template <typename _U1, typename _U2>
		constexpr Pair& operator=(Pair<_U1, _U2>&& rhs)
		{
			first = move(rhs.first);
			second = move(rhs.second);
			return *this;
		}
		constexpr void swap(Pair& rhs)
		{
			Pair tmp(move(*this));
			*this = move(rhs);
			rhs = move(tmp);
		}
	};

	template <typename _Ty1, typename _Ty2>
	constexpr Pair<decay_t<_Ty1>, decay_t<_Ty2>> make_pair(_Ty1&& first, _Ty2&& second)
	{
		return Pair<decay_t<_Ty1>, decay_t<_Ty2>>(forward<_Ty1>(first), forward<_Ty2>(second));
	}

	//! Compares the two string until one of the string sequence reaches null terminator, and
	//! returns 0 if the compared part is equal. Examples:
	//! * "aaa" & "aaab" -> 0 (only "aaa" is compared).
	//! * "aaa" & "aa" -> 0 (only "aa" is compared).
	//! * "" & "aa" -> 0 (nothing is compared).
	//! * "aba" & "aaa" -> not 0.
	//! * "aaab" & "aaac" -> not 0.
	inline int32 strcmp_prefix(const char* str1, const char* str2)
	{
		char c1, c2;
		while (*str1 && *str2)
		{
			c1 = *str1;
			c2 = *str2;
			if (c1 != c2)
			{
				return (int32)c1 - (int32)c2;
			}
			++str1;
			++str2;
		}
		return 0;
	}

	inline size_t strlen16(const char16_t* s) 
	{
		if (!s) return 0;
		size_t len = 0;
		while (s[len++]);
		return len;
	}

	//! clear the specified memory region to 0.
	//! @param[in] dest The start address of memory region to clear.
	//! @param[in] byteCount The size, in bytes, of the memory region to clear.
	//! @return Returns the `dest` pointer.
	inline void* memzero(void* dest, size_t byte_count)
	{
		return memset(dest, 0, byte_count);
	}

	//! Copies the data for a 2D bitmap.
	//! @param[in] dest A pointer to the first pixel to be copied in destination bitmap.
	//! @param[in] src A pointer to the first pixel to be copied in source bitmap.
	//! @param[in] copy_size_per_row The size of the data to be copied for every row, in bytes.
	//! @param[in] num_rows The number of rows to copy.
	//! @param[in] dest_row_pitch The size to advance for one row in destination bitmap in bytes.
	//! @param[in] src_row_pitch The size to advance for one row in destination bitmap in bytes.
	//! @return Returns the `dest` pointer.
	inline void* memcpy_bitmap(void* dest, const void* src, size_t copy_size_per_row, size_t num_rows, size_t dest_row_pitch, size_t src_row_pitch)
	{
		for (size_t r = 0; r < num_rows; ++r)
		{
			memcpy((void*)((size_t)dest + r * dest_row_pitch), (const void*)((size_t)src + r * src_row_pitch), copy_size_per_row);
		}
		return dest;
	}

	//! Copies the data for a 3D bitmap.
	//! @param[in] dest A pointer to the first pixel to be copied in destination bitmap.
	//! @param[in] src A pointer to the first pixel to be copied in source bitmap.
	//! @param[in] copy_size_per_row The size of the data to be copied for every row, in bytes.
	//! @param[in] num_rows The number of rows to copy.
	//! @param[in] num_slices The number of slices (layers) to copy.
	//! @param[in] dest_row_pitch The size to advance for one row in destination bitmap in bytes.
	//! @param[in] src_row_pitch The size to advance for one row in destination bitmap in bytes.
	//! @param[in] dest_slice_pitch The size to advance for one slice (layer) in destination bitmap in bytes.
	//! @param[in] src_slice_pitch The size to advance for one slice (layer) in destination bitmap in bytes.
	//! @return Returns the `dest` pointer.
	inline void* memcpy_bitmap3d(
		void* dest, const void* src,
		size_t copy_size_per_row,
		size_t num_rows, size_t num_slices,
		size_t dest_row_pitch, size_t src_row_pitch,
		size_t dest_slice_pitch, size_t src_slice_pitch)
	{
		for (size_t r = 0; r < num_slices; ++r)
		{
			memcpy_bitmap((void*)((size_t)dest + r * dest_slice_pitch), (const void*)((size_t)src + r * src_slice_pitch), copy_size_per_row,
				num_rows, dest_row_pitch, src_row_pitch);
		}
		return dest;
	}

	//! Returns a pointer that offsets the specified pixels in the texture.
	inline void* pixel_offset(void* base, size_t x, size_t y, size_t z, size_t bytes_per_pixel, size_t row_pitch, size_t slice_pitch)
	{
		size_t r = (size_t)base;
		r += z * slice_pitch + y * row_pitch + x * bytes_per_pixel;
		return (void*)r;
	}
	inline const void* pixel_offset(const void* base, size_t x, size_t y, size_t z, size_t bytes_per_pixel, size_t row_pitch, size_t slice_pitch)
	{
		size_t r = (size_t)base;
		r += z * slice_pitch + y * row_pitch + x * bytes_per_pixel;
		return (const void*)r;
	}
}

// IObject ID
#define luiid(x) static constexpr luna::Guid _guid { luna::Guid(x) };
// Class ID
#define lucid(x) static constexpr luna::Guid _guid { luna::Guid(x) };