// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file STBImage.hpp
* @author JXMaster
* @date 2020/3/6
*/
#pragma once
#include <Base/Assert.hpp>
#define STBI_ASSERT(x) luassert(x)

#include <Base/IAllocator.hpp>
#include "../ImageHeader.hpp"

namespace luna
{
	namespace image
	{
		// The global allocator being used by STB.
		extern IAllocator* stbi_alloc;

		inline void* stbi_malloc(size_t sz)
		{
			return stbi_alloc->allocate(sz);
		}

		inline void stbi_free(void* p)
		{
			stbi_alloc->free(p);
		}

		inline void* stbi_realloc(void* p, size_t newsz)
		{
			return stbi_alloc->reallocate(p, newsz);
		}
	}
}

#define STBI_MALLOC(sz)			luna::image::stbi_malloc(sz)
#define STBI_REALLOC(p,newsz)	luna::image::stbi_realloc(p,newsz)
#define STBI_FREE(p)			luna::image::stbi_free(p)

#ifdef LUNA_PLATFORM_WINDOWS
#define STBI_WINDOWS_UTF8
#endif

#define STBI_NO_STDIO

#include <Stb/stb_image.h>

namespace luna
{
	namespace image
	{
		extern stbi_io_callbacks stbi_iocb;

		// Initializes the STB context.
		void stbi_init();
	}
}