// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file StbTrueType.hpp
* @author JXMaster
* @date 2020/3/16
*/
#pragma once
#include <Base/IAllocator.hpp>
#define stbtt_uint8 luna::uint8
#define stbtt_int8 luna::int8
#define stbtt_uint16 luna::uint16
#define stbtt_int16 luna::int16
#define stbtt_uint32 luna::uint32
#define stbtt_int32 luna::int32

inline void* luna_stbtt_malloc(size_t x, void* u)
{
	return ((luna::IAllocator*)u)->allocate(x);
}

inline void luna_stbtt_free(void* x, void* u)
{
	((luna::IAllocator*)u)->free(x);
}

#define STBTT_malloc(x,u) luna_stbtt_malloc(x, u)
#define STBTT_free(x,u) luna_stbtt_free(x, u)

#define STBTT_assert(x)    luassert(x)
#include <Stb/stb_rect_pack.h>
#include <Stb/stb_truetype.h>