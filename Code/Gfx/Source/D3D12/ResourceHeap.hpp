// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ResourceHeap.hpp
* @author JXMaster
* @date 2019/8/9
* @brief D3D12 implementation of Resource Heap
*/
#pragma once

#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12

#include "../Windows/CommonInclude.hpp"
#include <d3d12.h>
#include "GraphicDevice.hpp"
#include <Runtime/TSAssert.hpp>

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			class ResourceHeap : public IResourceHeap
			{
			public:
				lucid("{17072071-663f-427c-835c-798cc25f5cbb}");
				luiimpl(ResourceHeap, IResourceHeap, IGraphicDeviceChild, IObject);
				lutsassert_lock();

				u64 m_alignment;
				P<GraphicDevice> m_device;
				ResourceHeapDesc m_desc;
				ComPtr<ID3D12Heap> m_heap;

				ResourceHeap(GraphicDevice* device) :
					m_device(device) {}

				RV init(const ResourceHeapDesc& desc);

				virtual IGraphicDevice* get_device() override
				{
					return m_device.get();
				}
				virtual ResourceHeapDesc desc() override
				{
					return m_desc;
				}
				virtual RP<IResource> new_resource(u64 offset, const ResourceDesc& desc, const ClearValue* optimized_clear_value) override;
			};
		}
	}
}

#endif