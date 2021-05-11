// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ViewSet.cpp
* @author JXMaster
* @date 2020/3/12
*/
#include "ViewSet.hpp"

#ifdef LUNA_GFX_D3D12

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			RV ViewSet::init(const ViewSetDesc& desc)
			{
				m_desc = desc;
				m_cbvs.resize(m_desc.num_cbvs);
				m_srvs.resize(m_desc.num_srvs);
				m_uavs.resize(m_desc.num_uavs);
				m_cbs.resize(m_desc.num_cbvs);
				m_srs.resize(m_desc.num_srvs);
				m_uas.resize(m_desc.num_uavs);
				m_counter_ress.resize(m_desc.num_uavs);
				D3D12_DESCRIPTOR_HEAP_DESC d;
				d.NumDescriptors = m_desc.num_cbvs + m_desc.num_srvs + m_desc.num_uavs;
				d.NodeMask = 0;
				d.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				d.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				m_cbv_srv_uav_size = m_device->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				m_sampler_size = m_device->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
				if (d.NumDescriptors)
				{
					if (FAILED(m_device->m_device->CreateDescriptorHeap(&d, IID_PPV_ARGS(&m_cbv_srv_uav_heap))))
					{
						return BasicError::bad_system_call();
					}
				}

				d.NumDescriptors = m_desc.num_samplers;
				d.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
				if (d.NumDescriptors)
				{
					if (FAILED(m_device->m_device->CreateDescriptorHeap(&d, IID_PPV_ARGS(&m_sampler_heap))))
					{
						return BasicError::bad_system_call();
					}
				}
				return RV();
			}
			void ViewSet::set_cbv(u32 index, IResource* res, const ConstantBufferViewDesc& cbv)
			{
				lucheck(index < m_desc.num_cbvs && res);
				lutsassert();
				Resource* r = static_cast<Resource*>(res);
				D3D12_CONSTANT_BUFFER_VIEW_DESC d;
				d.BufferLocation = r->m_res->GetGPUVirtualAddress() + cbv.offset;
				d.SizeInBytes = cbv.size;
				usize addr = m_cbv_srv_uav_heap->GetCPUDescriptorHandleForHeapStart().ptr + index * m_cbv_srv_uav_size;
				D3D12_CPU_DESCRIPTOR_HANDLE h;
				h.ptr = addr;
				m_device->m_device->CreateConstantBufferView(&d, h);
				m_cbvs[index] = cbv;
				m_cbs[index] = res;
			}
			static ShaderResourceViewDesc get_default_srv(IResource* res)
			{
				ResourceDesc d = res->desc();
				switch (d.type)
				{
				case EResourceType::texture_1d:
					return (d.depth_or_array_size) == 1 ?
						ShaderResourceViewDesc::as_tex1d(d.format, 0, d.mip_levels, 0.0f) :
						ShaderResourceViewDesc::as_tex1darray(d.format, 0, d.mip_levels, 0, d.depth_or_array_size, 0.0f);
				case EResourceType::texture_2d:
					return (d.depth_or_array_size == 1) ?
						((d.sample_count == 1) ?
							ShaderResourceViewDesc::as_tex2d(d.format, 0, d.mip_levels, 0.0f) :
							ShaderResourceViewDesc::as_tex2dms(d.format)) :
							((d.sample_count == 1) ?
								ShaderResourceViewDesc::as_tex2darray(d.format, 0, d.mip_levels, 0, d.depth_or_array_size, 0.0f) :
								ShaderResourceViewDesc::as_tex2dmsarray(d.format, 0, d.depth_or_array_size)
								);
				case EResourceType::texture_3d:
					return ShaderResourceViewDesc::as_tex3d(d.format, 0, d.mip_levels, 0.0f);
				case EResourceType::buffer:
					return ShaderResourceViewDesc::as_buffer(EResourceFormat::unknown, 0, (u32)d.width, 1, false);
				default:
					break;
				}
				lupanic();
				return ShaderResourceViewDesc();
			}
			void ViewSet::set_srv(u32 index, IResource* res, const ShaderResourceViewDesc* srv)
			{
				lucheck(index < m_desc.num_srvs && res);
				lutsassert();
				Resource* r = static_cast<Resource*>(res);
				usize addr = m_cbv_srv_uav_heap->GetCPUDescriptorHandleForHeapStart().ptr + (m_desc.num_cbvs + index) * m_cbv_srv_uav_size;
				D3D12_CPU_DESCRIPTOR_HANDLE h;
				h.ptr = addr;
				if (srv)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC d;
					d.Format = encode_resource_format(srv->format);
					d.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					switch (srv->type)
					{
					case EShaderResourceViewType::buffer:
						d.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
						d.Buffer.FirstElement = srv->buffer.offset;
						d.Buffer.NumElements = srv->buffer.count;
						d.Buffer.StructureByteStride = srv->buffer.element_size;
						d.Buffer.Flags = srv->buffer.raw_view ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE;
						break;
					case EShaderResourceViewType::tex1d:
						d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
						d.Texture1D.MipLevels = srv->tex1d.mip_levels;
						d.Texture1D.MostDetailedMip = srv->tex1d.most_detailed_mip;
						d.Texture1D.ResourceMinLODClamp = srv->tex1d.resource_min_lod_clamp;
						break;
					case EShaderResourceViewType::tex1darray:
						d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
						d.Texture1DArray.ArraySize = srv->tex1darray.array_size;
						d.Texture1DArray.FirstArraySlice = srv->tex1darray.first_array_slice;
						d.Texture1DArray.MipLevels = srv->tex1darray.mip_levels;
						d.Texture1DArray.MostDetailedMip = srv->tex1darray.most_detailed_mip;
						d.Texture1DArray.ResourceMinLODClamp = srv->tex1darray.resource_min_lod_clamp;
						break;
					case EShaderResourceViewType::tex2d:
						d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
						d.Texture2D.MipLevels = srv->tex2d.mip_levels;
						d.Texture2D.MostDetailedMip = srv->tex2d.most_detailed_mip;
						d.Texture2D.PlaneSlice = 0;
						d.Texture2D.ResourceMinLODClamp = srv->tex2d.resource_min_lod_clamp;
						break;
					case EShaderResourceViewType::tex2darray:
						d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
						d.Texture2DArray.ArraySize = srv->tex2darray.array_size;
						d.Texture2DArray.FirstArraySlice = srv->tex2darray.first_array_slice;
						d.Texture2DArray.MipLevels = srv->tex2darray.mip_levels;
						d.Texture2DArray.MostDetailedMip = srv->tex2darray.most_detailed_mip;
						d.Texture2DArray.PlaneSlice = 0;
						d.Texture2DArray.ResourceMinLODClamp = srv->tex2darray.resource_min_lod_clamp;
						break;
					case EShaderResourceViewType::tex2dms:
						d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
						break;
					case EShaderResourceViewType::tex2dmsarray:
						d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
						d.Texture2DMSArray.ArraySize = srv->tex2dmsarray.array_size;
						d.Texture2DMSArray.FirstArraySlice = srv->tex2dmsarray.first_array_slice;
						break;
					case EShaderResourceViewType::tex3d:
						d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
						d.Texture3D.MipLevels = srv->tex3d.mip_levels;
						d.Texture3D.MostDetailedMip = srv->tex3d.most_detailed_mip;
						d.Texture3D.ResourceMinLODClamp = srv->tex3d.resource_min_lod_clamp;
						break;
					case EShaderResourceViewType::texcube:
						d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
						d.TextureCube.MipLevels = srv->texcube.mip_levels;
						d.TextureCube.MostDetailedMip = srv->texcube.most_detailed_mip;
						d.TextureCube.ResourceMinLODClamp = srv->texcube.resource_min_lod_clamp;
						break;
					case EShaderResourceViewType::texcubearray:
						d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
						d.TextureCubeArray.First2DArrayFace = srv->texcubearray.first_2darray_face;
						d.TextureCubeArray.MipLevels = srv->texcubearray.mip_levels;
						d.TextureCubeArray.MostDetailedMip = srv->texcubearray.most_detailed_mip;
						d.TextureCubeArray.NumCubes = srv->texcubearray.num_cubes;
						d.TextureCubeArray.ResourceMinLODClamp = srv->texcubearray.resource_min_lod_clamp;
						break;
					default:
						lupanic();
						break;
					}
					m_device->m_device->CreateShaderResourceView(r->m_res.Get(), &d, h);
					m_srvs[index] = *srv;
				}
				else
				{
					m_device->m_device->CreateShaderResourceView(r->m_res.Get(), nullptr, h);
					m_srvs[index] = get_default_srv(res);
				}
				m_srs[index] = res;
			}

			static UnorderedAccessViewDesc get_default_uav(IResource* res)
			{
				ResourceDesc d = res->desc();
				switch (d.type)
				{
				case EResourceType::buffer:
					return UnorderedAccessViewDesc::as_buffer(EResourceFormat::unknown, 0, (u32)d.width, 1, 0, false);
				case EResourceType::texture_1d:
					return (d.depth_or_array_size) == 1 ?
						UnorderedAccessViewDesc::as_tex1d(d.format, 0) :
						UnorderedAccessViewDesc::as_tex1darray(d.format, 0, 0, d.depth_or_array_size);
				case EResourceType::texture_2d:
					return (d.depth_or_array_size == 1) ?
						UnorderedAccessViewDesc::as_tex2d(d.format, 0) :
						UnorderedAccessViewDesc::as_tex2darray(d.format, 0, 0, d.depth_or_array_size);
				case EResourceType::texture_3d:
					return UnorderedAccessViewDesc::as_tex3d(d.format, 0, 0, d.depth_or_array_size);
				default:
					break;
				}
				lupanic();
				return UnorderedAccessViewDesc();
			}

			void ViewSet::set_uav(u32 index, IResource* res, IResource* counter_resource, const UnorderedAccessViewDesc* uav)
			{
				lucheck(index < m_desc.num_uavs && res);
				lutsassert();
				ID3D12Resource* r = static_cast<Resource*>(res)->m_res.Get();
				ID3D12Resource* cr = counter_resource ? static_cast<Resource*>(counter_resource)->m_res.Get() : nullptr;
				usize addr = m_cbv_srv_uav_heap->GetCPUDescriptorHandleForHeapStart().ptr + (m_desc.num_cbvs + m_desc.num_srvs + index) * m_cbv_srv_uav_size;
				D3D12_CPU_DESCRIPTOR_HANDLE h;
				h.ptr = addr;
				if (uav)
				{
					D3D12_UNORDERED_ACCESS_VIEW_DESC d;
					d.Format = encode_resource_format(uav->format);
					switch (uav->type)
					{
					case EUnorderedAccessViewType::buffer:
						d.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
						d.Buffer.CounterOffsetInBytes = uav->buffer.counter_offset_in_bytes;
						d.Buffer.FirstElement = uav->buffer.offset;
						d.Buffer.Flags = uav->buffer.raw_view ? D3D12_BUFFER_UAV_FLAG_RAW : D3D12_BUFFER_UAV_FLAG_NONE;
						d.Buffer.NumElements = uav->buffer.count;
						d.Buffer.StructureByteStride = uav->buffer.element_size;
						break;
					case EUnorderedAccessViewType::tex1d:
						d.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
						d.Texture1D.MipSlice = uav->tex1d.mip_slice;
						break;
					case EUnorderedAccessViewType::tex1darray:
						d.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
						d.Texture1DArray.ArraySize = uav->tex1darray.array_size;
						d.Texture1DArray.FirstArraySlice = uav->tex1darray.first_array_slice;
						d.Texture1DArray.MipSlice = uav->tex1darray.mip_slice;
						break;
					case EUnorderedAccessViewType::tex2d:
						d.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
						d.Texture2D.MipSlice = uav->tex2d.mip_slice;
						d.Texture2D.PlaneSlice = 0;
						break;
					case EUnorderedAccessViewType::tex2darray:
						d.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
						d.Texture2DArray.ArraySize = uav->tex2darray.array_size;
						d.Texture2DArray.FirstArraySlice = uav->tex2darray.first_array_slice;
						d.Texture2DArray.MipSlice = uav->tex2darray.mip_slice;
						d.Texture2DArray.PlaneSlice = 0;
						break;
					case EUnorderedAccessViewType::tex3d:
						d.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
						d.Texture3D.FirstWSlice = uav->tex3d.first_layer_slice;
						d.Texture3D.MipSlice = uav->tex3d.mip_slice;
						d.Texture3D.WSize = uav->tex3d.layer_size;
						break;
					default:
						lupanic();
						break;
					}

					m_device->m_device->CreateUnorderedAccessView(r, cr, &d, h);
					m_uavs[index] = *uav;
				}
				else
				{
					m_device->m_device->CreateUnorderedAccessView(r, cr, nullptr, h);
					m_uavs[index] = get_default_uav(res);
				}
				m_uas[index] = res;
				m_counter_ress[index] = counter_resource;
			}
			void ViewSet::set_sampler(u32 index, const SamplerDesc& sampler)
			{
				lucheck(index < m_desc.num_samplers);
				lutsassert();
				D3D12_SAMPLER_DESC d;
				d.AddressU = encode_address_mode(sampler.address_u);
				d.AddressV = encode_address_mode(sampler.address_v);
				d.AddressW = encode_address_mode(sampler.address_w);
				d.BorderColor[0] = sampler.border_color[0];
				d.BorderColor[1] = sampler.border_color[1];
				d.BorderColor[2] = sampler.border_color[2];
				d.BorderColor[3] = sampler.border_color[3];
				d.ComparisonFunc = encode_comparison_func(sampler.comparison_func);
				d.Filter = encode_filter(sampler.filter);
				d.MaxAnisotropy = sampler.max_anisotropy;
				d.MaxLOD = sampler.max_lod;
				d.MinLOD = sampler.min_lod;
				d.MipLODBias = sampler.mip_lod_bias;
				usize addr = m_sampler_heap->GetCPUDescriptorHandleForHeapStart().ptr + index * m_sampler_size;
				D3D12_CPU_DESCRIPTOR_HANDLE h;
				h.ptr = addr;
				m_device->m_device->CreateSampler(&d, h);
			}
		}
	}
}

#endif