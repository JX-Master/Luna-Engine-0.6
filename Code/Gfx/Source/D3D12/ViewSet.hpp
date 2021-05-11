// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ViewSet.hpp
* @author JXMaster
* @date 2020/3/11
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12
#include "D3D12Common.hpp"
#include "GraphicDevice.hpp"
#include "Resource.hpp"

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			class ViewSet : public IViewSet
			{
			public:
				lucid("{6937b6b1-ce6d-4211-a5d5-5af9a6eba60a}");
				luiimpl(ViewSet, IViewSet, IGraphicDeviceChild, IObject);
				lutsassert_lock();

				P<GraphicDevice> m_device;
				ViewSetDesc m_desc;

				usize m_cbv_srv_uav_size;
				usize m_sampler_size;

				ComPtr<ID3D12DescriptorHeap> m_cbv_srv_uav_heap;
				ComPtr<ID3D12DescriptorHeap> m_sampler_heap;

				// Holds reference to the resource object.
				Vector<P<Resource>> m_cbs;
				Vector<P<Resource>> m_srs;
				Vector<P<Resource>> m_uas;
				Vector<P<Resource>> m_counter_ress;

				// Holds the view object.
				Vector<ConstantBufferViewDesc> m_cbvs;
				Vector<ShaderResourceViewDesc> m_srvs;
				Vector<UnorderedAccessViewDesc> m_uavs;

				ViewSet() {}

				RV init(const ViewSetDesc& desc);

				virtual IGraphicDevice* get_device() override
				{
					return m_device;
				}
				virtual ViewSetDesc desc() override
				{
					return m_desc;
				}
				virtual void set_cbv(u32 index, IResource* res, const ConstantBufferViewDesc& cbv) override;
				virtual void set_srv(u32 index, IResource* res, const ShaderResourceViewDesc* srv = nullptr) override;
				virtual void set_uav(u32 index, IResource* res, IResource* counter_resource = nullptr, const UnorderedAccessViewDesc* uav = nullptr) override;
				virtual void set_sampler(u32 index, const SamplerDesc& sampler) override;
				virtual void reset_cbv(u32 index) override
				{
					m_cbs[index] = nullptr;
				}
				virtual void reset_srv(u32 index) override
				{
					m_srs[index] = nullptr;
				}
				virtual void reset_uav(u32 index) override
				{
					m_uas[index] = nullptr;
				}
				virtual void reset_all() override
				{
					m_cbs.clear();
					m_srs.clear();
					m_uas.clear();
				}
			};
		}
	}
}

#endif