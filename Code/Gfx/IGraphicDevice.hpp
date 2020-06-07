// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IGraphicDevice.hpp
* @author JXMaster
* @date 2019/7/17
*/
#pragma once
#include "IResource.hpp"
#include "IPipelineState.hpp"
#include "IResourceHeap.hpp"
#include "ICommandQueue.hpp"
#include "IRenderPass.hpp"
#include "IFrameBuffer.hpp"
#include "IViewSet.hpp"
#include "Views.hpp"
#include "IShaderInputLayout.hpp"

namespace luna
{
	namespace gfx
	{
		enum class EDeviceType : uint32
		{
			d3d12 = 1,
		};

		enum class EDeviceFeature : uint32
		{
			//! (uint32) The alignment for the size of one row texture data to follow when placing texture data in buffers.
			texture_data_pitch_alignment = 1,
			//! (uint32) The alignment for the start offset of texture data to follow when placing texture data in buffers.
			texture_data_placement_alignment,
			//! (uint32) The alignment for the buffer data start location and size.
			buffer_data_alignment,
			//! (bool) If the device underlying API supports resource heap creation and placed resource creation.
			//! If this is `false`, then `IResourceHeap` cannot be created on the current device.
			placed_resource_supported,
		};

		inline constexpr uint32 calc_subresource_index(uint32 mip_slice, uint32 array_slice, uint32 mip_levels)
		{
			return mip_slice + array_slice * mip_levels;
		}

		inline constexpr void calc_mip_array_slice(uint32 subresource, uint32 mip_levels, uint32& mip_slice, uint32& array_slice)
		{
			mip_slice = subresource % mip_levels;
			array_slice = subresource / mip_levels;
		}

		//! @interface IGraphicDevice
		//! @threadsafe
		//! Represents a virtual graphic device provided to the application to access to graphic related
		//! facilities.
		struct IGraphicDevice : public IObject
		{
			luiid("{9ec1c048-7be9-45fd-96f2-a83add6f373d}");

			//! Get the device type, which is the Graphic API this device bounds to.
			virtual EDeviceType type() = 0;

			//! Checks device feature support information.
			virtual void check_feature_support(EDeviceFeature feature, void* data) = 0;

			//! Creates a new resource.
			//! @param[in] desc The descriptor object.
			//! @param[in] optimized_clear_value The optional optimized clear value for a texture resource. Specify `nullptr` if this is a buffer
			//! resource or the resource does not have a optimized clear value.
			virtual RP<IResource> new_resource(const ResourceDesc& desc, const ClearValue* optimized_clear_value = nullptr) = 0;

			//! Creates a new resource heap.
			virtual RP<IResourceHeap> new_resource_heap(const ResourceHeapDesc& desc) = 0;

			//! Creates a new shader input layout.
			virtual RP<IShaderInputLayout> new_shader_input_layout(const ShaderInputLayoutDesc& desc) = 0;

			//! Creates a graphic pipeline state.
			//! @param[in] input_layout The input layout this graphic pipeline compacts to.
			//! @param[in] render_pass The render pass this graphic pipeline compacts to.
			//! @param[in] desc The descriptor object.
			virtual RP<IPipelineState> new_graphics_pipeline_state(IShaderInputLayout* input_layout, IRenderPass* render_pass, const GraphicsPipelineStateDesc& desc) = 0;

			//! Creates a compute pipeline state.
			virtual RP<IPipelineState> new_compute_pipline_state(IShaderInputLayout* input_layout, const ComputePipelineStateDesc& desc) = 0;

			//! Creates a new command queue.
			virtual RP<ICommandQueue> new_command_queue(const CommandQueueDesc& desc) = 0;

			//! Creates a new render pass object.
			virtual RP<IRenderPass> new_render_pass(const RenderPassDesc& desc) = 0;

			//! Creates a new frame buffer object. The frame buffer object is immutable after creation.
			//! @param[in] render_pass The render pass object this frame buffer compacts to.
			//! @param[in] num_rtvs The number of render target views bounded to the frame buffer.
			//! @param[in] rts An array of textures to set as render target with at least `num_rtvs` elements.
			//! @param[in] rtvs An array of render target views with at least `num_rtvs` elements.
			//! @param[in] ds The resource used for depth stencil, specify `nullptr` to bind an empty one.
			//! @param[in] dsv The depth stencil view, specify `nullptr` to deduce from the depth stencil resource, or when `ds` is `nullptr`.
			virtual RP<IFrameBuffer> new_frame_buffer(IRenderPass* render_pass, uint32 num_rtvs, IResource** rts, RenderTargetViewDesc** rtvs, IResource* ds, DepthStencilViewDesc* dsv) = 0;

			//! Creates a new view set object that describes the SRV/CBV/UAVs that is bound to the pipeline.
			//! @param[in] target_input_layout The shader input layout object this view set compacts to.
			virtual RP<IViewSet> new_view_set(IShaderInputLayout* target_input_layout, const ViewSetDesc& desc) = 0;

			//! Calculates the placement information for the specified texture subresource. The texture data is arranged in row-major order.
			//! @param[in] width The width of the subresource.
			//! @param[in] height The height of the subresource.
			//! @param[in] depth The depth of the subresource.
			//! @param[in] format The format of the subresource.
			//! @param[out] row_pitch The row pitch of the subresource. Specify `nullptr` if this is not needed.
			//! @param[out] slice_pitch The slice pitch (size) of the subresource. Specify `nullptr` if this is not needed.
			//! @param[out] res_pitch The pitch of the whole subresource, which is the size of the subresource. Specify `nullptr` if this is not needed.
			virtual void calc_texture_subresource_buffer_placement(uint32 width, uint32 height, uint32 depth, EResourceFormat format,
				size_t* row_pitch, size_t* slice_pitch, size_t* res_pitch) = 0;
		};
	}
}