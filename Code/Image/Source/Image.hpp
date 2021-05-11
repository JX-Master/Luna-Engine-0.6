// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Image.hpp
* @author JXMaster
* @date 2020/3/6
*/
#pragma once
#include "ImageHeader.hpp"
#include <Core/Interface.hpp>
#include <Runtime/TSAssert.hpp>

namespace Luna
{
	namespace Image
	{
		class Image : public IImage
		{
		public:
			lucid("{a069bfc7-7481-43e9-80c2-baa2f5bd241b}");
			luiimpl(Image, IImage, IObject);
			lutsassert_lock();

			ImageDesc m_desc;
			Blob m_blob;

			Image() {}

			~Image() {}

			void init(const ImageDesc& desc);

			virtual Blob& data() override
			{
				return m_blob;
			}

			virtual ImageDesc desc() override
			{
				return m_desc;
			}

			virtual P<IImage> reset(const ImageDesc& desc) override;

			virtual RV save(IStream* target_stream, EImageTargetFormat format) override;
			virtual RV upload_data_to_texture(Gfx::IResource* resource, u32 subresource, Gfx::ICommandBuffer* cmd_buffer,
				u32 dest_x, u32 dest_y, u32 dest_z, const RectU& src_rect) override;
		};
	}
}