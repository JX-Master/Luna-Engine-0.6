// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FrameBuffer.hpp
* @author JXMaster
* @date 2020/3/11
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12
#include "D3D12Common.hpp"
#include "GraphicDevice.hpp"
#include "Resource.hpp"
#include "RenderPass.hpp"

namespace luna
{
	namespace gfx
	{
		namespace d3d12
		{
			class FrameBuffer : public IFrameBuffer
			{
			public:
				lucid("{03431afd-8d69-4928-b048-1da54ea43223}");
				luiimpl(FrameBuffer, IFrameBuffer, IGraphicDeviceChild, IObject);

				P<GraphicDevice> m_device;
				Vector<P<Resource>> m_rts;
				Vector<RenderTargetViewDesc> m_rtvs;
				P<Resource> m_ds;
				DepthStencilViewDesc m_dsv;

				size_t m_rtv_size;
				ComPtr<ID3D12DescriptorHeap> m_rtv_heap;
				ComPtr<ID3D12DescriptorHeap> m_dsv_heap;

				FrameBuffer() :
					luibind(get_module_allocator()),
					m_rts(get_module_allocator()),
					m_rtvs(get_module_allocator()){}

				RV init(uint32 num_rtvs, IResource** rts, RenderTargetViewDesc** rtvs, IResource* ds, DepthStencilViewDesc* dsv);

				D3D12_CPU_DESCRIPTOR_HANDLE get_rtv_cpu_handle(uint32 index)
				{
					D3D12_CPU_DESCRIPTOR_HANDLE h = m_rtv_heap->GetCPUDescriptorHandleForHeapStart();
					h.ptr += index * m_rtv_size;
					return h;
				}

				D3D12_CPU_DESCRIPTOR_HANDLE get_dsv_cpu_handle()
				{
					return m_dsv_heap->GetCPUDescriptorHandleForHeapStart();
				}

				uint32 count_rtv()
				{
					return (uint32)m_rtvs.size();
				}

				virtual IGraphicDevice* get_device() override
				{
					return m_device;
				}
			};
		}
	}
}

#endif