// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Image.hpp
* @author JXMaster
* @date 2020/3/6
*/
#pragma once
#include "IImage.hpp"

#ifndef LUNA_IMAGE_API
#define LUNA_IMAGE_API
#endif

namespace luna
{
	namespace image
	{

		//! Initializes the image module.
		LUNA_IMAGE_API void init();

		//! Creates one new empty image object. The data in the image is uninitialized.
		LUNA_IMAGE_API P<IImage> new_image(const ImageDesc& desc);

		//! Loads the image description information from the stream. The system detects the format of the image 
		//! data automatically.
		//! 
		//! @param[in] stream The stream to load the encoded image data from. The cursor must be set to the 
		//! first byte of the encoded image data.
		//! 
		//! The cursor of the stream will not be reset after this operation.
		LUNA_IMAGE_API R<ImageDesc> load_image_desc(IStream* stream);

		//! Creates one image by parsing data in the stream. The system detects the format of the image 
		//! data automatically.
		//! 
		//! Supported image formats are: .jpg/.jpeg/.png/.tga/.bmp/.psd (without layer info)/.gif/.hdr/.pic
		//! 
		//! @param[in] stream The stream to load the encoded image data from. The cursor must be set to the 
		//! first byte of the encoded image data.
		//! 
		//! The cursor of the stream will not be reset after this operation.
		LUNA_IMAGE_API RP<IImage> load_image(IStream* stream, EImagePixelFormat* desired_format = nullptr);
	}
}