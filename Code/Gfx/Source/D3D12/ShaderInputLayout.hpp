// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ShaderInputLayout.hpp
* @author JXMaster
* @date 2020/3/14
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
			class ShaderInputLayout : public IShaderInputLayout
			{
			public:
				lucid("{0a7ccb6d-bcf0-433a-af5b-ee454c37e5e2}");
				luiimpl(ShaderInputLayout, IShaderInputLayout, IGraphicDeviceChild, IObject);

				P<GraphicDevice> m_device;
				ComPtr<ID3D12RootSignature> m_rs;
				Vector<ShaderInputGroupDesc> m_groups;

				ShaderInputLayout() {}
				

				RV init(const ShaderInputLayoutDesc& desc);

				virtual IGraphicDevice* get_device() override
				{
					return m_device.get();
				}
			};
		}
	}
}

#endif