// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FontTexture.cpp
* @author JXMaster
* @date 2019/10/5
*/
#include "FontTexture.hpp"

namespace Luna
{
	namespace EasyDraw
	{
		RV FontTexture::reset(Font::IFontAtlas* atlas, Gfx::ICommandBuffer* command_buffer)
		{
			using namespace Gfx;
			m_tex = nullptr;
			m_font = atlas;

			auto image = atlas->get_rendered_image();
			if (!image)
			{
				return custom_error(BasicError::bad_arguments(), "IFontTexture::reset - No image exist for font atlas.");
			}

			// Uploads the image.
			lutry
			{
				auto device = command_buffer->get_device();
				auto img_desc = image->desc();
				luset(m_tex, device->new_resource(ResourceDesc::tex2d(Image::pixel_format_to_resource_format(img_desc.format), EAccessType::gpu_local, EResourceUsageFlag::shader_resource,
					img_desc.width, img_desc.height, 1, 1, 1, 0)));
				luexp(image->upload_data_to_texture(m_tex, 0, command_buffer, 0, 0, 0, RectU(0, 0, img_desc.width, img_desc.height)));
			}
			lucatchret;
			return RV();
		}
	}
}