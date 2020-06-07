// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Image.cpp
* @author JXMaster
* @date 2020/3/6
*/
#include "Image.hpp"
#include "IO/STBImageWrite.hpp"

namespace luna
{
	namespace image
	{
		inline int stbiw_get_comp(EImagePixelFormat format)
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

		bool check_format(EImageTargetFormat format, EImagePixelFormat pixel_format)
		{
			switch (format)
			{
			case EImageTargetFormat::png:
				return (
					(pixel_format == EImagePixelFormat::r8_unorm) ||
					(pixel_format == EImagePixelFormat::rg8_unorm) ||
					(pixel_format == EImagePixelFormat::rgba8_unorm) ||
					(pixel_format == EImagePixelFormat::r16_unorm) ||
					(pixel_format == EImagePixelFormat::rg16_unorm) ||
					(pixel_format == EImagePixelFormat::rgba16_unorm)
					) ? true : false;
			case EImageTargetFormat::bmp:
			case EImageTargetFormat::tga:
			case EImageTargetFormat::jpeg:
				return (
					(pixel_format == EImagePixelFormat::r8_unorm) ||
					(pixel_format == EImagePixelFormat::rg8_unorm) ||
					(pixel_format == EImagePixelFormat::rgba8_unorm)
					) ? true : false;
			case EImageTargetFormat::hdr:
				return (
					(pixel_format == EImagePixelFormat::r32_float) ||
					(pixel_format == EImagePixelFormat::rg32_float) ||
					(pixel_format == EImagePixelFormat::rgb32_float) ||
					(pixel_format == EImagePixelFormat::rgba32_float)
					) ? true : false;
			default:
				return false;
			}
		}

		void Image::init(const ImageDesc& desc)
		{
			m_desc = desc;
			m_buffer_size = desc.width * desc.height * size_per_pixel(desc.format);
			m_buffer = memalloc(m_buffer_size);
		}

		P<IImage> Image::reset(const ImageDesc& desc)
		{
			P<Image> img = box_ptr(new_obj<Image>(get_module_allocator()));
			img->m_desc = m_desc;
			img->m_buffer = m_buffer;
			img->m_buffer_size = m_buffer_size;
			init(desc);
			return img;
		}

		RV Image::save(IStream* target_stream, EImageTargetFormat format)
		{
			lutsassert();
			luassert_usr(target_stream);
			if (!check_format(format, m_desc.format))
			{
				set_err(e_type_dismatch, "The specified encode format does not support the image pixel format.");
				return e_user_failure;
			}
			int comp = stbiw_get_comp(m_desc.format);
			int res;
			auto cur = target_stream->tell();
			switch (format)
			{
			case EImageTargetFormat::png:
				res = stbi_write_png_to_func(stbi_write_func, (void*)target_stream, m_desc.width, m_desc.height, comp, m_buffer, m_desc.width * size_per_pixel(m_desc.format));
				break;
			case EImageTargetFormat::bmp:
				res = stbi_write_bmp_to_func(stbi_write_func, (void*)target_stream, m_desc.width, m_desc.height, comp, m_buffer);
				break;
			case EImageTargetFormat::tga:
				res = stbi_write_tga_to_func(stbi_write_func, (void*)target_stream, m_desc.width, m_desc.height, comp, m_buffer);
				break;
			case EImageTargetFormat::jpeg:
				res = stbi_write_jpg_to_func(stbi_write_func, (void*)target_stream, m_desc.width, m_desc.height, comp, m_buffer, 80);
				break;
			case EImageTargetFormat::hdr:
				res = stbi_write_hdr_to_func(stbi_write_func, (void*)target_stream, m_desc.width, m_desc.height, comp, (float32*)m_buffer);
				break;
			default:
				lupanic();
			}
			if (!res)
			{
				auto _ = target_stream->seek(cur, ESeekMode::begin);
				return e_failure;
			}
			return s_ok;
		}

		RV Image::upload_data_to_texture(gfx::IResource* resource, uint32 subresource, gfx::ICommandBuffer* cmd_buffer,
			uint32 dest_x, uint32 dest_y, uint32 dest_z, const RectU& src_rect)
		{
			using namespace gfx;
			lutry
			{
				if (m_desc.format == EImagePixelFormat::rgb8_unorm ||
				m_desc.format == EImagePixelFormat::rgb16_unorm)
				{
					set_err(e_type_dismatch, "Image pixel formats rgb8_unorm and rgb16_unorm are not supported by Gfx, consider using rgba alternatives.");
					return e_user_failure;
				}
				auto dev = resource->get_device();
				// Calculates the data required for upload buffer.
				uint32 pitch_alignment;
				dev->check_feature_support(EDeviceFeature::texture_data_pitch_alignment, &pitch_alignment);
				size_t copy_size_per_row = size_per_pixel(m_desc.format) * (src_rect.right - src_rect.left);
				size_t buf_row_pitch = align_upper(copy_size_per_row, pitch_alignment);
				size_t buf_slice_pitch = buf_row_pitch * (src_rect.bottom - src_rect.top);
				size_t buf_size = buf_slice_pitch;
				lulet(buf, dev->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::none, buf_size)));
				lulet(buf_data, buf->map_subresource(0, false, 1, 0));
				memcpy_bitmap3d(buf_data, m_buffer, copy_size_per_row, (src_rect.bottom - src_rect.top),
					1, buf_row_pitch, m_desc.width * size_per_pixel(m_desc.format), buf_slice_pitch, m_buffer_size);
				buf->unmap_subresource(0, true);
				cmd_buffer->resource_barrier(ResourceBarrierDesc::as_transition(resource, EResourceState::copy_dest, subresource));
				TextureCopyLocation srcl, destl;
				srcl.type = ETextureCopyType::placed_footprint;
				srcl.placed_footprint.offset = 0;
				srcl.placed_footprint.footprint.width = src_rect.right - src_rect.left;
				srcl.placed_footprint.footprint.height = src_rect.bottom - src_rect.top;
				srcl.placed_footprint.footprint.depth = 1;
				srcl.placed_footprint.footprint.row_pitch = (uint32)buf_row_pitch;
				EResourceFormat res_format = pixel_format_to_resource_format(m_desc.format);
				srcl.placed_footprint.footprint.format = res_format;
				srcl.resource = buf;
				destl.type = ETextureCopyType::subresource_index;
				destl.subresource_index = subresource;
				destl.resource = resource;
				cmd_buffer->attach_graphic_object(buf);
				cmd_buffer->copy_texture_region(destl, dest_x, dest_y, dest_z, srcl, &BoxU(src_rect.left, src_rect.top, 0, src_rect.right, src_rect.bottom, 1));
				luexp(cmd_buffer->submit());
				cmd_buffer->wait();
			}
			lucatchret;
			return s_ok;
		}
	}
}