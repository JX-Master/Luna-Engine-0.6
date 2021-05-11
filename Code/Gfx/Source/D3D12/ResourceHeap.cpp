// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ResourceHeap.cpp
* @author JXMaster
* @date 2019/8/9
* @brief D3D12 implementation of Resource Heap
*/
#include "ResourceHeap.hpp"

#ifdef LUNA_GFX_D3D12

#include "Resource.hpp"
#include "GraphicDevice.hpp"
#include "GraphicSystem.hpp"
#include "D3D12Common.hpp"

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			RV ResourceHeap::init(const ResourceHeapDesc& desc)
			{
				m_desc = desc;
				lucheck_msg(m_desc.access_type != EAccessType::unknown, "Cannot use EAccessType::unknown to create resource heap.");
				D3D12_HEAP_DESC d;
				d.SizeInBytes = desc.size;
				switch (m_desc.access_type)
				{
				case EAccessType::gpu_local:
					d.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
					break;
				case EAccessType::readback:
					d.Properties.Type = D3D12_HEAP_TYPE_READBACK;
					break;
				case EAccessType::upload:
					d.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
					break;
				default:
					lupanic();
					break;
				}
				d.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				d.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				d.Properties.CreationNodeMask = 0;
				d.Properties.VisibleNodeMask = 0;
				if ((desc.resource_heap_usages & EResourceHeapUsageFlag::texture_ms) != EResourceHeapUsageFlag::none)
				{
					d.Alignment = D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT;
				}
				else
				{
					d.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
				}
				d.Flags = D3D12_HEAP_FLAG_NONE;
				if (m_device->m_feature_options.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_1)
				{
					if (m_desc.resource_heap_usages == EResourceHeapUsageFlag::buffer)
					{
						d.Flags |= D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
					}
					else if (m_desc.resource_heap_usages == EResourceHeapUsageFlag::texture_rt_ds)
					{
						d.Flags |= D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
					}
					else if (m_desc.resource_heap_usages == EResourceHeapUsageFlag::texture_non_rt_ds)
					{
						d.Flags |= D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
					}
					else
					{
						get_error_object() = Error(BasicError::bad_system_call(), "On D3D12 Resource Heap Tier 1 hardware, only one type of EResourceHeapUsageFlag::buffer, EResourceHeapUsageFlag::texture_rt_ds or EResourceHeapUsageFlag::texture_non_rt_ds can be specified for one heap.");
						return BasicError::error_object();
					}
				}
				else
				{
					if ((m_desc.resource_heap_usages & EResourceHeapUsageFlag::buffer) == EResourceHeapUsageFlag::none)
					{
						d.Flags |= D3D12_HEAP_FLAG_DENY_BUFFERS;
					}
					if ((m_desc.resource_heap_usages & EResourceHeapUsageFlag::texture_rt_ds) == EResourceHeapUsageFlag::none)
					{
						d.Flags |= D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
					}
					if ((m_desc.resource_heap_usages & EResourceHeapUsageFlag::texture_non_rt_ds) == EResourceHeapUsageFlag::none)
					{
						d.Flags |= D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
					}
				}
				if (FAILED(m_device->m_device->CreateHeap(&d, IID_PPV_ARGS(&m_heap))))
				{
					return BasicError::failure();
				}
				return RV();
			}

			const char* print_access_type(EAccessType t)
			{
				switch (t)
				{
				case EAccessType::gpu_local:
					return "EAccessType::gpu_local";
				case EAccessType::upload:
					return "EAccessType::upload";
				case EAccessType::readback:
					return "EAccessType::readback";
				default:
					lupanic();
					return nullptr;
				}
			}

			RP<IResource> ResourceHeap::new_resource(u64 offset, const ResourceDesc& desc, const ClearValue* optimized_clear_value)
			{
				lutsassert();
				// Check requirements.
				if (desc.access_type != EAccessType::unknown)
				{
					if ((desc.access_type == EAccessType::readback && m_desc.access_type != EAccessType::readback) ||
						(desc.access_type == EAccessType::upload && m_desc.access_type != EAccessType::upload) ||
						((desc.access_type == EAccessType::gpu_local) &&
							m_desc.access_type != EAccessType::gpu_local))
					{
						get_error_object() = Error(BasicError::bad_arguments(), "IResourceHeap::new_resource - Resource access type dismatch: Heap is %s while resource is %s", print_access_type(m_desc.access_type), print_access_type(desc.access_type));
						return BasicError::error_object();
					}
				}
				
				if (desc.type == EResourceType::buffer && ((m_desc.resource_heap_usages & EResourceHeapUsageFlag::buffer) == EResourceHeapUsageFlag::none))
				{
					get_error_object() = Error(BasicError::bad_arguments(), "IResourceHeap::new_resource - Buffer resources are not allowed to be created in this heap.");
					return BasicError::error_object();
				}
				if (desc.type == EResourceType::texture_1d || desc.type == EResourceType::texture_2d || desc.type == EResourceType::texture_3d)
				{
					if (((m_desc.resource_heap_usages & EResourceHeapUsageFlag::texture_ms) == EResourceHeapUsageFlag::none) && (desc.sample_count != 1))
					{
						get_error_object() = Error(BasicError::bad_arguments(), "IResourceHeap::new_resource - Multi-sample textures are not allowed to be created in this heap.");
						return BasicError::error_object();
					}
					if (((m_desc.resource_heap_usages & EResourceHeapUsageFlag::texture_non_rt_ds) == EResourceHeapUsageFlag::none) && ((desc.usages & EResourceUsageFlag::shader_resource) != EResourceUsageFlag::none))
					{
						get_error_object() = Error(BasicError::bad_arguments(), "IResourceHeap::new_resource - Non-rt and non-ds textures are not allowed to be created in this heap.");
						return BasicError::error_object();
					}
					if (((m_desc.resource_heap_usages & EResourceHeapUsageFlag::texture_rt_ds) == EResourceHeapUsageFlag::none) && (((desc.usages & EResourceUsageFlag::render_target) != EResourceUsageFlag::none) || ((desc.usages & EResourceUsageFlag::depth_stencil) != EResourceUsageFlag::none)))
					{
						get_error_object() = Error(BasicError::bad_arguments(), "IResourceHeap::new_resource - RT and DS textures are not allowed to be created in this heap.");
						return BasicError::error_object();
					}
				}
				if (desc.type == EResourceType::unknown)
				{
					get_error_object() = Error(BasicError::bad_arguments(), "IResourceHeap::new_resource - The resource type is not allowed to be EResourceType::unknown when creating resources.");
					return BasicError::error_object();
				}
				//! Create desc object.
				D3D12_RESOURCE_DESC rd = encode_resource_desc(desc);

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

				// Create object.
				ComPtr<ID3D12Resource> res;
				D3D12_RESOURCE_STATES s;
				if (desc.access_type == EAccessType::gpu_local)
				{
					s = D3D12_RESOURCE_STATE_COMMON;
				}
				else if (desc.access_type == EAccessType::readback)
				{
					s = D3D12_RESOURCE_STATE_COPY_DEST;
				}
				else if (desc.access_type == EAccessType::upload)
				{
					s = D3D12_RESOURCE_STATE_GENERIC_READ;
				}
				else
				{
					lupanic();
				}
				if (FAILED(m_device->m_device->CreatePlacedResource(m_heap.Get(), offset, &rd, s, pcv, IID_PPV_ARGS(&res))))
				{
					return BasicError::failure();
				}

				P<Resource> robj = newobj<Resource>();
				if (!robj)
				{
					return BasicError::bad_memory_alloc();
				}
				robj->m_device = m_device;
				robj->m_desc = desc;
				robj->m_res = res;
				return robj;
			}
		}
	}
}

#endif