// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ImageSystem.cpp
* @author JXMaster
* @date 2020/3/6
*/
#include "ImageSystem.hpp"
#include "Image.hpp"
#include "IO/STBImage.hpp"
#include <Runtime/Module.hpp>
namespace Luna
{
	namespace Image
	{
		void deinit() {}

		RV init()
		{
			stbi_init();
			return RV();
		}

		StaticRegisterModule m("Image", "Core;Gfx", init, deinit);

		P<IImage> new_image(const ImageDesc& desc)
		{
			P<Image> img = newobj<Image>();
			img->init(desc);
			return img;
		}

		static ImageDesc stbi_make_subimage_desc(int w, int h, int comp, int is_hdr, int is_16bit)
		{
			luassert(comp >= 1 && comp <= 4);
			ImageDesc d;
			d.width = (u32)w;
			d.height = (u32)h;
			if (is_hdr)
			{
				switch (comp)
				{
				case 1: d.format = EImagePixelFormat::r32_float; break;
				case 2: d.format = EImagePixelFormat::rg32_float; break;
				case 3: d.format = EImagePixelFormat::rgb32_float; break;
				case 4: d.format = EImagePixelFormat::rgba32_float; break;
				default: lupanic(); break;
				}
			}
			else if (is_16bit)
			{
				switch (comp)
				{
				case 1: d.format = EImagePixelFormat::r16_unorm; break;
				case 2: d.format = EImagePixelFormat::rg16_unorm; break;
				case 3: d.format = EImagePixelFormat::rgb16_unorm; break;
				case 4: d.format = EImagePixelFormat::rgba16_unorm; break;
				default: lupanic(); break;
				}
			}
			else
			{
				switch (comp)
				{
				case 1: d.format = EImagePixelFormat::r8_unorm; break;
				case 2: d.format = EImagePixelFormat::rg8_unorm; break;
				case 3: d.format = EImagePixelFormat::rgb8_unorm; break;
				case 4: d.format = EImagePixelFormat::rgba8_unorm; break;
				default: lupanic(); break;
				}
			}
			return d;
		}

		R<ImageDesc> load_image_desc(IStream* stream)
		{
			int x, y, comp;
			int is_16_bit, is_hdr;
			lutry
			{
				lulet(pos, stream->tell());
				
				if (!stbi_info_from_callbacks(&stbi_iocb, (void*)stream, &x, &y, &comp))
				{
					// data corrupted.
					auto _ = stream->seek(pos, ESeekMode::begin);
					get_error_object() = Error(BasicError::bad_arguments(), "Failed to load image description info: Data corrupted.");
					return BasicError::error_object();
				}
				luexp(stream->seek(pos, ESeekMode::begin));
				is_16_bit = stbi_is_16_bit_from_callbacks(&stbi_iocb, (void*)stream);
				luexp(stream->seek(pos, ESeekMode::begin));
				is_hdr = stbi_is_hdr_from_callbacks(&stbi_iocb, (void*)stream);
				luexp(stream->seek(pos, ESeekMode::begin));
			}
			lucatchret;
			return stbi_make_subimage_desc(x, y, comp, is_hdr, is_16_bit);
		}

		static bool is_hdr(EImagePixelFormat format)
		{
			switch (format)
			{
			case EImagePixelFormat::r32_float:
			case EImagePixelFormat::rg32_float:
			case EImagePixelFormat::rgb32_float:
			case EImagePixelFormat::rgba32_float:
				return true;
			default:
				return false;
			}
		}

		static bool is_16_bit(EImagePixelFormat format)
		{
			switch (format)
			{
			case EImagePixelFormat::r16_unorm:
			case EImagePixelFormat::rg16_unorm:
			case EImagePixelFormat::rgb16_unorm:
			case EImagePixelFormat::rgba16_unorm:
				return true;
			default:
				return false;
			}
		}

		static int get_comp(EImagePixelFormat format)
		{
			switch (format)
			{
			case EImagePixelFormat::r8_unorm:
			case EImagePixelFormat::r16_unorm:
			case EImagePixelFormat::r32_float:
				return 1;
			case EImagePixelFormat::rg8_unorm:
			case EImagePixelFormat::rg16_unorm:
			case EImagePixelFormat::rg32_float:
				return 2;
			case EImagePixelFormat::rgb8_unorm:
			case EImagePixelFormat::rgb16_unorm:
			case EImagePixelFormat::rgb32_float:
				return 3;
			case EImagePixelFormat::rgba8_unorm:
			case EImagePixelFormat::rgba16_unorm:
			case EImagePixelFormat::rgba32_float:
				return 4;
			default:
				lupanic();
				return 0;
			}
		}

		RP<IImage> load_image(IStream* stream, EImagePixelFormat* desired_format)
		{
			lucheck(stream);
			P<Image> img;

			img = newobj<Image>();

			// Use stb_image library to load image.

			// Prefetch the data info.
			lutry
			{
				lulet(pos, stream->tell());
				lulet(desc, load_image_desc(stream));
				luexp(stream->seek(pos, ESeekMode::begin));
				if (desired_format)
				{
					desc.format = *desired_format;
				}

				void* data;
				int out_x, out_y, out_comp;

				if (is_hdr(desc.format))
				{
					data = stbi_loadf_from_callbacks(&stbi_iocb, (void*)stream, &out_x, &out_y, &out_comp, get_comp(desc.format));
				}
				else if (is_16_bit(desc.format))
				{
					data = stbi_load_16_from_callbacks(&stbi_iocb, (void*)stream, &out_x, &out_y, &out_comp, get_comp(desc.format));
				}
				else
				{
					data = stbi_load_from_callbacks(&stbi_iocb, (void*)stream, &out_x, &out_y, &out_comp, get_comp(desc.format));
				}
				if (!data)
				{
					auto _ = stream->seek(pos, ESeekMode::begin);
					return ImageError::file_parse_error();
				}
				img->m_blob.resize(desc.width * desc.height * size_per_pixel(desc.format));
				memcpy(img->m_blob.data(), data, img->m_blob.size());
				img->m_desc = desc;
			}
			lucatchret;
			return img;
		}

		namespace ImageError
		{
			LUNA_IMAGE_API errcode_t file_parse_error()
			{
				static errcode_t e = get_error_code_by_name(u8"file_parse_error");
				return e;
			}
		}
	}
}