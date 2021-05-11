// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file GraphicDevice.hpp
* @author JXMaster
* @date 2019/7/17
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12

#include "../Windows/CommonInclude.hpp"
#include "d3d12.h"
#include "GraphicSystem.hpp"

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			class ResourceHeap;
			class ViewTableHeap;
			struct ViewTableHeapDesc;
			class PipelineState;
			class CommandAllocator;
			class CommandList;
			class CommandQueue;
			class Fence;
			class CommittedResource;
			class UploadBuffer;
			class DeviceContext;

			//! @class GraphicDevice
			class GraphicDevice final : public IGraphicDevice
			{
			public:
				lucid("{038b4cb4-5e16-41a1-ad6f-7e2a49e2241e}");
				luiimpl(GraphicDevice, IGraphicDevice, IObject);
				
				ComPtr<ID3D12Device> m_device;

				D3D12_FEATURE_DATA_D3D12_OPTIONS m_feature_options;

				bool m_swap_chain_initialized;

				Blob m_swap_chain_vs;
				Blob m_swap_chain_ps;
				ComPtr<ID3D12Resource> m_swap_chain_vert_buf;
				ComPtr<ID3DBlob> m_swap_chain_root_signature_data;

				GraphicDevice() :
					m_swap_chain_initialized(false) {}
				~GraphicDevice();

				bool init(ID3D12Device* dev);

				virtual EDeviceType type() override
				{
					return EDeviceType::d3d12;
				}
				virtual void check_feature_support(EDeviceFeature feature, void* data) override;
				virtual RP<IResource> new_resource(const ResourceDesc& desc, const ClearValue* optimized_clear_value = nullptr) override;
				virtual RP<IResourceHeap> new_resource_heap(const ResourceHeapDesc& desc) override;
				virtual RP<IShaderInputLayout> new_shader_input_layout(const ShaderInputLayoutDesc& desc) override;
				virtual RP<IPipelineState> new_graphics_pipeline_state(IShaderInputLayout* shader_input_layout, IRenderPass* render_pass, const GraphicsPipelineStateDesc& desc) override;
				virtual RP<IPipelineState> new_compute_pipline_state(IShaderInputLayout* shader_input_layout, const ComputePipelineStateDesc& desc) override;
				virtual RP<ICommandQueue> new_command_queue(const CommandQueueDesc& desc) override;
				virtual RP<IRenderPass> new_render_pass(const RenderPassDesc& desc) override;
				virtual RP<IFrameBuffer> new_frame_buffer(IRenderPass* render_pass, u32 num_rtvs, IResource** rts, RenderTargetViewDesc** rtvs, IResource* ds, DepthStencilViewDesc* dsv) override;
				virtual RP<IViewSet> new_view_set(IShaderInputLayout* shader_input_layout, const ViewSetDesc& desc) override;
				virtual void calc_texture_subresource_buffer_placement(u32 width, u32 height, u32 depth, EResourceFormat format,
					usize* row_pitch, usize* slice_pitch, usize* res_pitch) override;
			};
		}
	}
}

#endif