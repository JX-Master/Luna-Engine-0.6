// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FontTexture.cpp
* @author JXMaster
* @date 2019/10/5
*/
#include "FontTexture.hpp"

namespace luna
{
	namespace edraw
	{
		RV FontTexture::reset(font::IFontAtlas* atlas, gfx::ICommandBuffer* command_buffer)
		{
			using namespace gfx;
			m_tex = nullptr;
			m_font = atlas;

			auto image = atlas->get_rendered_image();
			if (!image)
			{
				set_err(e_bad_arguments, "IFontTexture::reset - No image exist for font atlas.");
				return e_user_failure;
			}

			// Uploads the image.
			lutry
			{
				auto device = command_buffer->get_device();
				auto img_desc = image->desc();
				luset(m_tex, device->new_resource(ResourceDesc::tex2d(image::pixel_format_to_resource_format(img_desc.format), EAccessType::gpu_local, EResourceUsageFlag::shader_resource,
					img_desc.width, img_desc.height, 1, 1, 1, 0)));
				luexp(image->upload_data_to_texture(m_tex, 0, command_buffer, 0, 0, 0, RectU(0, 0, img_desc.width, img_desc.height)));
			}
			lucatchret;
			return s_ok;
		}
	}
}