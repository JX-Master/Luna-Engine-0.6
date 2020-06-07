// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Image.hpp
* @author JXMaster
* @date 2020/3/6
*/
#pragma once
#include "ImageHeader.hpp"
#include <Base/Interface.hpp>
#include <Base/TSAssert.hpp>

namespace luna
{
	namespace image
	{
		class Image : public IImage
		{
		public:
			lucid("{a069bfc7-7481-43e9-80c2-baa2f5bd241b}");
			luiimpl(Image, IImage, IBlob, IObject);
			lutsassert_lock();

			ImageDesc m_desc;
			void* m_buffer;
			size_t m_buffer_size;

			Image(IAllocator* alloc) :
				luibind(alloc),
				m_buffer(nullptr),
				m_buffer_size(0) {}

			~Image()
			{
				if (m_buffer)
				{
					memfree(m_buffer);
					m_buffer = nullptr;
				}
				m_buffer_size = 0;
			}

			void init(const ImageDesc& desc);

			virtual const void* data() override
			{
				return m_buffer;
			}

			virtual size_t size() override
			{
				return m_buffer_size;
			}

			virtual ImageDesc desc() override
			{
				return m_desc;
			}

			virtual P<IImage> reset(const ImageDesc& desc) override;

			virtual void* buffer() override
			{
				return m_buffer;
			}

			virtual RV save(IStream* target_stream, EImageTargetFormat format) override;
			virtual RV upload_data_to_texture(gfx::IResource* resource, uint32 subresource, gfx::ICommandBuffer* cmd_buffer,
				uint32 dest_x, uint32 dest_y, uint32 dest_z, const RectU& src_rect) override;
		};
	}
}