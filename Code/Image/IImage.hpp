// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IImage.hpp
* @author JXMaster
* @date 2020/3/5
*/
#pragma once
#include <Gfx/Gfx.hpp>

namespace luna
{
	namespace image
	{
		enum class EImagePixelFormat
		{
			// Supported by all formats except .hdr
			r8_unorm,
			rg8_unorm,
			rgb8_unorm,
			rgba8_unorm,

			// Supported only by .png
			r16_unorm,
			rg16_unorm,
			rgb16_unorm,
			rgba16_unorm,

			// Supported only by .hdr
			r32_float,
			rg32_float,
			rgb32_float,
			rgba32_float,
		};

		inline size_t size_per_pixel(EImagePixelFormat format)
		{
			switch (format)
			{
			case EImagePixelFormat::r8_unorm:
				return 1;
			case EImagePixelFormat::rg8_unorm:
			case EImagePixelFormat::r16_unorm:
				return 2;
			case EImagePixelFormat::rgb8_unorm:
				return 3;
			case EImagePixelFormat::rgba8_unorm:
			case EImagePixelFormat::rg16_unorm:
			case EImagePixelFormat::r32_float:
				return 4;
			case EImagePixelFormat::rgb16_unorm:
				return 6;
			case EImagePixelFormat::rgba16_unorm:
			case EImagePixelFormat::rg32_float:
				return 8;
			case EImagePixelFormat::rgb32_float:
				return 12;
			case EImagePixelFormat::rgba32_float:
				return 16;
			default:
				lupanic();
				return 0;
			}
		}

		inline gfx::EResourceFormat pixel_format_to_resource_format(EImagePixelFormat format)
		{
			switch (format)
			{
			case EImagePixelFormat::r8_unorm:
				return gfx::EResourceFormat::r8_unorm;
			case EImagePixelFormat::rg8_unorm:
				return gfx::EResourceFormat::rg8_unorm;
			case EImagePixelFormat::r16_unorm:
				return gfx::EResourceFormat::r16_unorm;
			case EImagePixelFormat::rgba8_unorm:
				return gfx::EResourceFormat::rgba8_unorm;
			case EImagePixelFormat::rg16_unorm:
				return gfx::EResourceFormat::rg16_unorm;
			case EImagePixelFormat::r32_float:
				return gfx::EResourceFormat::r32_float;
			case EImagePixelFormat::rgba16_unorm:
				return gfx::EResourceFormat::rgba16_unorm;
			case EImagePixelFormat::rg32_float:
				return gfx::EResourceFormat::rg32_float;
			case EImagePixelFormat::rgb32_float:
				return gfx::EResourceFormat::rgb32_float;
			case EImagePixelFormat::rgba32_float:
				return gfx::EResourceFormat::rgba32_float;
			default:
				lupanic();
				return gfx::EResourceFormat::unknown;
			}
		}

		enum class EImageTargetFormat : uint32
		{
			png = 1,
			bmp = 2,
			tga = 3,
			jpeg = 4,
			hdr = 5,
		};

		//! Describes one image, which may be composed by several subimages.
		struct ImageDesc
		{
			//! The pixel format of the image.
			EImagePixelFormat format;
			//! The width of the image.
			uint32 width;
			//! The height of the image.
			uint32 height;
		};

		//! @interface IImage
		//! Represents the decoded image data that is loaded from image file and can be written to file.
		struct IImage : public IBlob
		{
			luiid("{926b3b20-dd80-4bf8-884c-5e0a35856398}");

			//! Gets the description of this image.
			virtual ImageDesc desc() = 0;

			//! Resets the image data.
			//! The original data will be returned by a new image object.
			virtual P<IImage> reset(const ImageDesc& desc) = 0;

			//! Gets the image data buffer.
			virtual void* buffer() = 0;

			//! Encodes the image using the specified file format.
			virtual RV save(IStream* target_stream, EImageTargetFormat format) = 0;

			//! Uploads the data in this image to the specified texture subresource.
			//! @param[in] resource The resource to upload data to.
			//! @param[in] subresource The index of the subresource to upload data to.
			//! @param[in] cmd_buffer The command buffer used to record the copy commands. The command buffer must be in recording
			//! state. The command buffer will be submitted, waited and reset by this call.
			//! @param[in] dest_x The width offset of the copy region in the destination subresource.
			//! @param[in] dest_y The height offset of the copy region in the destination subresource.
			//! @param[in] dest_z The depth offset of the copy region in the destination subresource.
			//! @param[in] src_rect The bounding rect of the copy region in this image.
			virtual RV upload_data_to_texture(gfx::IResource* resource, uint32 subresource, gfx::ICommandBuffer* cmd_buffer,
				uint32 dest_x, uint32 dest_y, uint32 dest_z, const RectU& src_rect) = 0;
		};
	}
}