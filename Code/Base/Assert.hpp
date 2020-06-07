// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Assert.hpp
* @author JXMaster
* @date 2018/10/26
 */
#pragma once
#include "BaseDefines.hpp"
#include <cassert>

#if defined(LUNA_PLATFORM_WINDOWS)
#define luassert_always(_condition) (void)(                                                       \
            (!!(_condition)) ||                                                              \
            (_wassert(_CRT_WIDE(#_condition), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
        )
#define lupanic_always() (void)(                                                       \
            (_wassert(_CRT_WIDE("Assertion Failed!"), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
        )
#define luassert_msg_always(_condition, _message) (void)(                                                       \
            (!!(_condition)) ||                                                              \
            (_wassert(_CRT_WIDE(_message), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
        )
#define lupanic_msg_always(_message) (void)(                                                       \
            (_wassert(_CRT_WIDE(_message), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
        )
#else
#error "Assertion is not implemented on this platform."
#endif

//! luassert will only be used if the current module is compiled in debug case.
#ifdef LUNA_DEBUG
#define luassert(_condition) luassert_always(_condition)
#define lupanic() lupanic_always()
#define luassert_msg(_condition, _message) luassert_msg_always(_condition, _message)
#define lupanic_msg(_message) lupanic_msg_always(_message)
#else
#define luassert(_condition) ((void)0)
#define lupanic() ((void)0)
#define luassert_msg(_condition, _message) ((void)0)
#define lupanic_msg(_message) ((void)0)
#endif

//! luassert_usr will only be used if the current module is compiled in debug or profile case.
#ifdef LUNA_PROFILE
#define luassert_usr(_condition) luassert_always(_condition)
#define lupanic_usr() lupanic_always()
#define luassert_msg_usr(_condition, _message) luassert_msg_always(_condition, _message)
#define lupanic_msg_usr(_message) lupanic_msg_always(_message)
#else
#define luassert_usr(_condition) ((void)0)
#define lupanic_usr() ((void)0)
#define luassert_msg_usr(_condition, _message) ((void)0)
#define lupanic_msg_usr(_message) ((void)0)
#endif

#ifdef LUNA_DEBUG

#if defined(LUNA_PLATFORM_WINDOWS)
#define ludebugbreak() __debugbreak()
#endif

#else
#define ludebugbreak() ((void)0)
#endif

#define luassert_succeeded(_condition) luassert(luna::succeeded(_condition))
#define luassert_failed(_condition) luassert(luna::failed(_condition))