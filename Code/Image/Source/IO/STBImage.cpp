// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file STBImage.cpp
* @author JXMaster
* @date 2020/3/6
*/
#include <Core/Core.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "STBImage.hpp"

namespace luna
{
	namespace image
	{
		IAllocator* stbi_alloc;
		stbi_io_callbacks stbi_iocb;
		int stbi_read(void *user, char *data, int size)
		{
			IStream* stream = reinterpret_cast<IStream*>(user);
			size_t read_bytes;
			result_t r = stream->read(data, size, &read_bytes);
			if (failed(r))
			{
				return 0;
			}
			return (int)read_bytes;
		}

		void stbi_skip(void *user, int n)
		{
			IStream* stream = reinterpret_cast<IStream*>(user);
			auto _ = stream->seek(n, ESeekMode::current);
		}

		int stbi_eof(void *user)
		{
			IStream* stream = reinterpret_cast<IStream*>(user);
			return (stream->tell() >= stream->size());
		}

		void stbi_init()
		{
			stbi_alloc = get_module_allocator();
			stbi_iocb.eof = stbi_eof;
			stbi_iocb.read = stbi_read;
			stbi_iocb.skip = stbi_skip;
		}
	}
}