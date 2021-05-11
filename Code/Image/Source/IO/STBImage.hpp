// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file STBImage.hpp
* @author JXMaster
* @date 2020/3/6
*/
#pragma once
#include <Runtime/Assert.hpp>
#define STBI_ASSERT(x) luassert(x)
#include <Runtime/Memory.hpp>
#include "../ImageHeader.hpp"

namespace Luna
{
	namespace Image
	{

		inline void* stbi_malloc(usize sz)
		{
			return memalloc(sz);
		}

		inline void stbi_free(void* p)
		{
			memfree(p);
		}

		inline void* stbi_realloc(void* p, usize newsz)
		{
			return memrealloc(p, newsz);
		}
	}
}

#define STBI_MALLOC(sz)			Luna::Image::stbi_malloc(sz)
#define STBI_REALLOC(p,newsz)	Luna::Image::stbi_realloc(p,newsz)
#define STBI_FREE(p)			Luna::Image::stbi_free(p)

#ifdef LUNA_PLATFORM_WINDOWS
#define STBI_WINDOWS_UTF8
#endif

#define STBI_NO_STDIO

#include <Stb/stb_image.h>

namespace Luna
{
	namespace Image
	{
		extern stbi_io_callbacks stbi_iocb;

		// Initializes the STB context.
		void stbi_init();
	}
}