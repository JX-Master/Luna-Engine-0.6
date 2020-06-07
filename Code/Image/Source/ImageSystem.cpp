// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ImageSystem.cpp
* @author JXMaster
* @date 2020/3/6
*/
#include "ImageSystem.hpp"
#include "Image.hpp"
#include "IO/STBImage.hpp"
namespace luna
{
	namespace image
	{
		void deinit() {}

		void init()
		{
			stbi_init();
			add_module("Image", deinit);
		}

		P<IImage> new_image(const ImageDesc& desc)
		{
			P<Image> img = box_ptr(new_obj<Image>(get_module_allocator()));
			img->init(desc);
			return img;
		}

		//RP<IImage> load_image(IStream* stream, EImageSourceFormat format)
		//{
		//	luassert_usr(stream);
		//	P<Image> img;
		//	lutry
		//	{
		//		if (format == EImageSourceFormat::dds)
		//		{
		//			// To be implemented later.
		//			return e_not_available;
		//		}
		//		img = box_ptr(new_obj<Image>(get_module_allocator()));
		//		if (!img)
		//		{
		//			return e_bad_memory_alloc;
		//		}
		//		lulet(subimg, load_subimage(stream, format));
		//		img->m_subimages.resize(1);
		//		ImageDesc& d = img->m_desc;
		//		SubimageDesc& sd = img->m_subimages[0]->m_desc;
		//		d.array_size = 1;
		//		d.depth = 1;
		//		d.flags = EImageDescFlag::none;
		//		d.format = sd.format;
		//		d.height = sd.height;
		//		d.mip_levels = 1;
		//		d.type = gfx::EResourceType::texture_2d;
		//		d.width = sd.width;
		//	}
		//	lucatchret;
		//	return img;
		//}

		static ImageDesc stbi_make_subimage_desc(int w, int h, int comp, int is_hdr, int is_16bit)
		{
			luassert(comp >= 1 && comp <= 4);
			ImageDesc d;
			d.width = (uint32)w;
			d.height = (uint32)h;
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
			uint64 pos = stream->tell();
			int x, y, comp;
			if (!stbi_info_from_callbacks(&stbi_iocb, (void*)stream, &x, &y, &comp))
			{
				// data corrupted.
				auto _ = stream->seek(pos, ESeekMode::begin);
				set_err(e_bad_arguments, "Failed to load image description info: Data corrupted.");
				return e_user_failure;
			}
			int is_16_bit, is_hdr;
			lutry
			{
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
			luassert_usr(stream);
			P<Image> img;

			img = box_ptr(new_obj<Image>(stbi_alloc));

			uint64 pos = stream->tell();

			// Use stb_image library to load image.

			// Prefetch the data info.
			lutry
			{
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
					return e_failure;
				}
				img->m_buffer = data;
				img->m_desc = desc;
				img->m_buffer_size = desc.width * desc.height * size_per_pixel(desc.format);
			}
			lucatchret;
			return img;
		}
	}
}