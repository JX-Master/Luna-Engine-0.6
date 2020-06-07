// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file STBImageWrite.hpp
* @author JXMaster
* @date 2020/3/7
*/
#pragma once
#include <Base/Assert.hpp>
#define STBIW_ASSERT(x) luassert(x)

#include <Base/IAllocator.hpp>

namespace luna
{
	namespace image
	{
		// The global allocator being used by STB.
		extern IAllocator* stbi_alloc;

		inline void* stbiw_malloc(size_t sz)
		{
			return stbi_alloc->allocate(sz);
		}

		inline void stbiw_free(void* p)
		{
			stbi_alloc->free(p);
		}

		inline void* stbiw_realloc(void* p, size_t newsz)
		{
			return stbi_alloc->reallocate(p, newsz);
		}
	}
}

#define STBIW_MALLOC(sz)			luna::image::stbiw_malloc(sz)
#define STBIW_REALLOC(p,newsz)		luna::image::stbiw_realloc(p,newsz)
#define STBIW_FREE(p)				luna::image::stbiw_free(p)

#define STBI_WRITE_NO_STDIO

#include <Stb/stb_image_write.h>

namespace luna
{
	namespace image
	{
		void stbi_write_func(void* context, void* data, int size);

	}
}