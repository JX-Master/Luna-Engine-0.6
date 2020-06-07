// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Resource.cpp
* @author JXMaster
* @date 2019/8/10
* @brief D3D12 implementation of Resource Object
*/
#include "Resource.hpp"

#ifdef LUNA_GFX_D3D12

namespace luna
{
	namespace gfx
	{
		namespace d3d12
		{
			inline uint32 calc_mip_levels(uint32 width, uint32 height, uint32 depth)
			{
				uint32 mip_levels = 0;
				while (width > 1 || height > 1 || depth > 1)
				{
					++mip_levels;
					width >>= 1;
					height >>= 1;
					depth >>= 1;
				}
				return mip_levels;
			}

			result_t Resource::init(const ResourceDesc& desc, const ClearValue* optimized_clear_value)
			{
				luassert_usr(desc.access_type != EAccessType::unknown);
				m_desc = desc;

				if (!m_desc.mip_levels)
				{
					if (m_desc.type != EResourceType::texture_3d)
					{
						m_desc.mip_levels = calc_mip_levels((uint32)desc.width, desc.height, 1);
					}
					else
					{
						m_desc.mip_levels = calc_mip_levels((uint32)desc.width, desc.height, desc.depth_or_array_size);
					}
				}

				D3D12_HEAP_PROPERTIES hp;
				hp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				hp.CreationNodeMask = 0;
				hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				switch (m_desc.access_type)
				{
				case EAccessType::gpu_local:
					hp.Type = D3D12_HEAP_TYPE_DEFAULT;
					break;
				case EAccessType::readback:
					hp.Type = D3D12_HEAP_TYPE_READBACK;
					break;
				case EAccessType::upload:
					hp.Type = D3D12_HEAP_TYPE_UPLOAD;
					break;
				default:
					lupanic();
					break;
				}
				hp.VisibleNodeMask = 0;
				D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;
				//if ((desc.usages & EResourceUsageFlag::back_buffer) != EResourceUsageFlag::none)
				//{
				//	flags |= D3D12_HEAP_FLAG_ALLOW_DISPLAY;
				//}
				D3D12_RESOURCE_DESC rd = encode_resource_desc(m_desc);
				D3D12_CLEAR_VALUE* pcv = nullptr;
				D3D12_CLEAR_VALUE cv;
				if (optimized_clear_value)
				{
					cv.Format = encode_resource_format(optimized_clear_value->format);
					if (optimized_clear_value->type == EClearValueType::color)
					{
						cv.Color[0] = optimized_clear_value->color[0];
						cv.Color[1] = optimized_clear_value->color[1];
						cv.Color[2] = optimized_clear_value->color[2];
						cv.Color[3] = optimized_clear_value->color[3];
					}
					else
					{
						cv.DepthStencil.Depth = optimized_clear_value->depth_stencil.depth;
						cv.DepthStencil.Stencil = optimized_clear_value->depth_stencil.stencil;
					}
					pcv = &cv;
				}
				D3D12_RESOURCE_STATES s;
				if (desc.access_type == EAccessType::gpu_local)
				{
					s = D3D12_RESOURCE_STATE_COMMON;
				}
				else if (desc.access_type == EAccessType::upload)
				{
					s = D3D12_RESOURCE_STATE_GENERIC_READ;
				}
				else if (desc.access_type == EAccessType::readback)
				{
					s = D3D12_RESOURCE_STATE_COPY_DEST;
				}
				else
				{
					lupanic();
				}
				if (FAILED(m_device->m_device->CreateCommittedResource(&hp, flags, &rd, s,
					pcv, IID_PPV_ARGS(&m_res))))
				{
					return e_bad_system_call;
				}

				if (desc.type != EResourceType::buffer && ((desc.usages & EResourceUsageFlag::simultaneous_access) == EResourceUsageFlag::none))
				{
					m_states.resize(count_subresources(), EResourceState::common);
				}

				return s_ok;
			}
			R<void*> Resource::map_subresource(uint32 subresource, bool read_full_range, size_t read_range_begin, size_t read_range_end)
			{
				lutsassert();
				if (read_full_range)
				{
					void* r;
					HRESULT hr = m_res->Map(subresource, nullptr, &r);
					if (FAILED(hr))
					{
						return e_bad_system_call;
					}
					return r;
				}
				else
				{
					D3D12_RANGE range;
					range.Begin = read_range_begin;
					range.End = read_range_end;
					void* r;
					HRESULT hr = m_res->Map(subresource, &range, &r);
					if (FAILED(hr))
					{
						return e_bad_system_call;
					}
					return r;
				}
			}
			void Resource::unmap_subresource(uint32 subresource, bool write_full_range, size_t write_range_begin, size_t write_range_end)
			{
				lutsassert();
				if (write_full_range)
				{
					m_res->Unmap(subresource, nullptr);
				}
				else
				{
					D3D12_RANGE range;
					range.Begin = write_range_begin;
					range.End = write_range_end;
					m_res->Unmap(subresource, &range);
				}
			}
			RV Resource::write_subresource(uint32 subresource, const void* src, uint32 src_row_pitch, uint32 src_depth_pitch, const BoxU& write_box)
			{
				lutsassert();
				D3D12_BOX b;
				b.left = write_box.left;
				b.right = write_box.right;
				b.top = write_box.top;
				b.bottom = write_box.bottom;
				b.front = write_box.front;
				b.back = write_box.back;
				return SUCCEEDED(m_res->WriteToSubresource(subresource, &b, src, src_row_pitch, src_depth_pitch)) ? s_ok : e_failure;
			}
			RV Resource::read_subresource(void* dest, uint32 dest_row_pitch, uint32 dest_depth_pitch, uint32 subresource, const BoxU& read_box)
			{
				lutsassert();
				D3D12_BOX b;
				b.left = read_box.left;
				b.right = read_box.right;
				b.top = read_box.top;
				b.bottom = read_box.bottom;
				b.front = read_box.front;
				b.back = read_box.back;
				return SUCCEEDED(m_res->ReadFromSubresource(dest, dest_row_pitch, dest_depth_pitch, subresource, &b)) ? s_ok : e_failure;
			}
		}
	}
}

#endif