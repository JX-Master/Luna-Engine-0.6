// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Resource.hpp
* @author JXMaster
* @date 2019/8/10
* @brief D3D12 implementation of Resource Object
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12
#include "D3D12Common.hpp"
#include "GraphicDevice.hpp"
#include <Runtime/TSAssert.hpp>

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			class Resource : public IResource
			{
			public:
				lucid("{dd9486e7-5195-4be3-96a4-b27c2e06bc80}");
				luiimpl(Resource, IResource, IGraphicDeviceChild, IObject);
				lutsassert_lock();

				P<GraphicDevice> m_device;

				ComPtr<ID3D12Resource> m_res;
				ResourceDesc m_desc;

				//! One for each subresource, 0 if this resource does not have a global state.
				Vector<EResourceState> m_states;

				Resource() {}

				u32 count_subresources()
				{
					if (m_desc.type == EResourceType::texture_3d)
					{
						return m_desc.mip_levels;
					}
					return m_desc.mip_levels * m_desc.depth_or_array_size;
				}

				RV init(const ResourceDesc& desc, const ClearValue* optimized_clear_value);

				virtual IGraphicDevice* get_device() override
				{
					return m_device;
				}

				virtual ResourceDesc desc() override
				{
					return m_desc;
				}

				virtual EResourceState global_state(u32 subresource) override
				{
					if (m_states.empty())
					{
						switch (m_desc.access_type)
						{
						case EAccessType::gpu_local:
							return EResourceState::common;
						case EAccessType::upload:
							return EResourceState::generic_read;
						case EAccessType::readback:
							return EResourceState::copy_dest;
						default:
							lupanic();
							return EResourceState::common;
						}
					}
					else
					{
						lucheck(subresource < m_states.size());
						return m_states[subresource];
					}
				}

				virtual R<void*> map_subresource(u32 subresource, bool read_full_range, usize read_range_begin = 0, usize read_range_end = 0) override;
				virtual void unmap_subresource(u32 subresource, bool write_full_range, usize write_range_begin = 0, usize write_range_end = 0) override;
				virtual RV write_subresource(u32 subresource, const void* src, u32 src_row_pitch, u32 src_depth_pitch, const BoxU& write_box) override;
				virtual RV read_subresource(void* dest, u32 dest_row_pitch, u32 dest_depth_pitch, u32 subresource, const BoxU& read_box) override;
			};
		}
	}
}

#endif