// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file StbTrueType.hpp
* @author JXMaster
* @date 2020/3/16
*/
#pragma once
#include <Runtime/Memory.hpp>
#define stbtt_uint8 Luna::u8
#define stbtt_int8 Luna::i8
#define stbtt_uint16 Luna::u16
#define stbtt_int16 Luna::i16
#define stbtt_uint32 Luna::u32
#define stbtt_int32 Luna::i32

inline void* luna_stbtt_malloc(size_t x, void* u)
{
	return Luna::memalloc(x);
}

inline void luna_stbtt_free(void* x, void* u)
{
	Luna::memfree(x);
}

#define STBTT_malloc(x,u) luna_stbtt_malloc(x, u)
#define STBTT_free(x,u) luna_stbtt_free(x, u)

#define STBTT_assert(x)    luassert(x)
#include <Stb/stb_rect_pack.h>
#include <Stb/stb_truetype.h>