// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PipelineState.hpp
* @author JXMaster
* @date 2019/8/15
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12

#include "GraphicDevice.hpp"
#include "ShaderInputLayout.hpp"

namespace luna
{
	namespace gfx
	{
		namespace d3d12
		{
			class PipelineState : public IPipelineState
			{
			public:
				lucid("{6f1c9839-a567-43e8-95e0-4fa8c1c67f0f}");
				luiimpl(PipelineState, IPipelineState, IGraphicDeviceChild, IObject);
				
				P<GraphicDevice> m_device;
				ComPtr<ID3D12PipelineState> m_pso;

				bool m_is_graphic;

				PipelineState(GraphicDevice* dev) :
					luibind(get_module_allocator()),
					
					m_device(dev) {}
				
				virtual IGraphicDevice* get_device() override
				{
					return m_device.get();
				}
				
				bool init_as_graphic(const GraphicsPipelineStateDesc& desc, IRenderPass* render_pass, IShaderInputLayout* shader_input_layout);
				bool init_as_compute(const ComputePipelineStateDesc& desc, IShaderInputLayout* shader_input_layout);
			};
		}
	}
}

#endif