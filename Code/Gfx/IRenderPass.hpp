// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IRenderPass.hpp
* @author JXMaster
* @date 2020/3/10
*/
#pragma once
#include "IGraphicDeviceChild.hpp"
#include "GraphicDefines.hpp"

namespace Luna
{
	namespace Gfx
	{
		enum class EAttachmentLoadOp : u32
		{
			//! The previous contents of the texture within the render area will be preserved.
			load = 0,
			//! The contents within the render area will be cleared to a uniform value
			clear = 1,
			//! The previous contents within the area need not be preserved, and we don't clear 
			//! the content before using it.
			dont_care = 2,
		};

		enum class EAttachmentStoreOp : u32
		{
			//! Preserves the content of the texture after ending the render pass.
			store = 0,
			//! The content of the texture will not be preserved after ending the render pass.
			//! This is used for attachments like depth buffer that is only used for the current 
			//! render pass.
			dont_care = 1
		};

		struct AttachmentDesc
		{
			//! The format of the render target.
			EResourceFormat format;
			//! The operation to perform before the attachment is used.
			EAttachmentLoadOp load_op;
			//! If to store the results of the attachment after this render pass ends.
			EAttachmentStoreOp store_op;

			AttachmentDesc() = default;
			AttachmentDesc(EResourceFormat _format,
				EAttachmentLoadOp _load_op,
				EAttachmentStoreOp _store_op) :
				format(_format),
				load_op(_load_op),
				store_op(_store_op) {}

		};

		struct RenderPassDesc
		{
			//! Number of attachments (render targets) this render pass have. The number is 1-8.
			u32 num_attachments;
			//! 8 color attachments.
			AttachmentDesc attachments[8];
			//! The format for depth stencil attachment.
			EResourceFormat depth_stencil_format;
			//! The load operation for depth attachment.
			EAttachmentLoadOp depth_load_op;
			//! The store operation for depth attachment.
			EAttachmentStoreOp depth_store_op;
			//! The load operation for stencil attachment.
			EAttachmentLoadOp stencil_load_op;
			//! The store operation for stencil attachment.
			EAttachmentStoreOp stencil_store_op;
			//! Specify the sample count, 1 if MSAA is not used.
			u32 sample_count;
			bool use_depth_stencil;

			RenderPassDesc() = default;
			RenderPassDesc(
				InitializerList<AttachmentDesc> _attachments,
				EResourceFormat _depth_stencil_format,
				EAttachmentLoadOp _depth_load_op,
				EAttachmentStoreOp _depth_store_op,
				EAttachmentLoadOp _stencil_load_op,
				EAttachmentStoreOp _stencil_store_op,
				u32 _sample_count,
				bool _use_depth_stencil
			) :
				depth_stencil_format(_depth_stencil_format),
				depth_load_op(_depth_load_op),
				depth_store_op(_depth_store_op),
				stencil_load_op(_stencil_load_op),
				stencil_store_op(_stencil_store_op),
				sample_count(_sample_count),
				use_depth_stencil(_use_depth_stencil)
			{
				num_attachments = min<u32>((u32)_attachments.size(), 8);
				for (u32 i = 0; i < num_attachments; ++i)
				{
					attachments[i] = *(_attachments.begin() + i);
				}
			}
		};

		//! @interface IRenderPass
		//! Describes the attachments and the load & store ops for a render pass.
		struct IRenderPass : public IGraphicDeviceChild
		{
			luiid("{c17740dc-fc0f-461f-9663-8b4207e5d7d6}");

			virtual RenderPassDesc desc() = 0;

		};
	}
}