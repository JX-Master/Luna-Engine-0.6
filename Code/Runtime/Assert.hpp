// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Assert.hpp
* @author JXMaster
* @date 2018/10/26
 */
#pragma once
#include "Base.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

 //! The assertion function implemented by runtime.
namespace Luna
{
	//! Reports an assertion failure information to the underlying OS or CRT.
	//! This function works in all builds, and can be called even if the runtime is not initialized.
	//! The behavior of this function depends on the OS/CRT implementation, but in general it will 
	//! present an error message box and then terminate the program.
	//! @param[in] msg The UTF-8 error message to show.
	//! @param[in] file The UTF-8 name of the file that causes the panic.
	//! @param[in] line The code line the assertion is placed.
	LUNA_RUNTIME_API void assert_fail(const c8* msg, const c8* file, u32 line);

	//! Triggers a debug break, pauses the program and attaches the debugger to the program.
	//! This only works in debug build.
	LUNA_RUNTIME_API inline void debug_break();
}

#define luna_u8_string_(s) u8 ## s
#define luna_u8_string(s) luna_u8_string_(s)

//! Evaluates the given expression, and if the result value cannot pass `if` statement, calls `assert_fail`.
//! This function works in all builds.
#define luassert_always(_condition) (void)(                                                   \
            (!!(_condition)) ||                                                               \
            (Luna::assert_fail(u8###_condition, luna_u8_string(__FILE__), (unsigned)(__LINE__)), 0)       \
        )

//! Save as `luassert_always`, but displays a custom message instead of the expression.
#define luassert_msg_always(_condition, _message) (void)(                                     \
            (!!(_condition)) ||                                                               \
            (Luna::assert_fail(u8##_message, luna_u8_string(__FILE__), (unsigned)(__LINE__)), 0)         \
        )

//! Triggers an assertion failure directly.
#define lupanic_always() luassert_msg_always(false, "Panic has been called.")

//! Triggers an assertion failure with custom message.
#define lupanic_msg_always(_message) luassert_msg_always(false, _message)

//! Evaluates the expression and asserts the result is equal to the given value.
#define luna_eval_and_assert_equal_msg_always(_exp, _res, _message) luassert_msg_always((_exp) == (_res), _message)
#define luna_eval_and_assert_equal_always(_exp, _res) luassert_always((_exp) == (_res))

//! Evaluates the expression and asserts the result is not equal to the given value.
#define luna_eval_and_assert_nonequal_msg_always(_exp, _res, _message) luassert_msg_always((_exp) != (_res), _message)
#define luna_eval_and_assert_nonequal_always(_exp, _res) luassert_always((_exp) != (_res))

#ifdef LUNA_DEBUG
#define luassert(_condition) luassert_always(_condition)
#define lupanic() lupanic_always()
#define luassert_msg(_condition, _message) luassert_msg_always(_condition, _message)
#define lupanic_msg(_message) lupanic_msg_always(_message)
#define luna_eval_and_assert_equal_msg(_exp, _res, _message) luna_eval_and_assert_equal_msg_always(_exp, _res, _message)
#define luna_eval_and_assert_equal(_exp, _res) luna_eval_and_assert_equal_always(_exp, _res)
#define luna_eval_and_assert_nonequal_msg(_exp, _res, _message) luna_eval_and_assert_nonequal_msg_always(_exp, _res, _message)
#define luna_eval_and_assert_nonequal(_exp, _res) luna_eval_and_assert_nonequal_always(_exp, _res)
#else
#define luassert(_condition) ((void)0)
#define lupanic() ((void)0)
#define luassert_msg(_condition, _message) ((void)0)
#define lupanic_msg(_message) ((void)0)
#define luna_eval_and_assert_equal_msg(_exp, _res, _message) (_exp)
#define luna_eval_and_assert_equal(_exp, _res) (_exp)
#define luna_eval_and_assert_nonequal_msg(_exp, _res, _message) (_exp)
#define luna_eval_and_assert_nonequal(_exp, _res) (_exp)
#endif

#ifdef LUNA_ENABLE_CONTRACT_ASSERTION
#define lucheck(_condition) luassert_always(_condition)
#define lucheck_msg(_condition, _message) luassert_msg_always(_condition, _message)
#else
#define lucheck(_condition) ((void)0)
#define lucheck_msg(_condition, _message) ((void)0)
#endif
