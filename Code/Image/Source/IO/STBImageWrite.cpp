// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file STBImageWrite.cpp
* @author JXMaster
* @date 2020/3/7
*/
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STBImageWrite.hpp"
#include <Core/Core.hpp>

namespace luna
{
	namespace image
	{
		void stbi_write_func(void* context, void* data, int size)
		{
			IStream* stream = (IStream*)context;
			auto _ = stream->write(data, size);
		}
	}
}