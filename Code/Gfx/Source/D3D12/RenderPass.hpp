// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RenderPass.hpp
* @author JXMaster
* @date 2020/3/10
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12

#include "GraphicDevice.hpp"

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			class RenderPass : public IRenderPass
			{
			public:
				lucid("{7b01f489-6965-4e15-bfb8-84e21bf19ac3}");
				luiimpl(RenderPass, IRenderPass, IGraphicDeviceChild, IObject);

				P<GraphicDevice> m_device;
				RenderPassDesc m_desc;

				RenderPass() {}

				virtual IGraphicDevice* get_device() override
				{
					return m_device.get();
				}
				virtual RenderPassDesc desc() override
				{
					return m_desc;
				}
			};
		}
	}
}

#endif