// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FrameBuffer.cpp
* @author JXMaster
* @date 2020/3/11
*/
#pragma once
#include "FrameBuffer.hpp"

#ifdef LUNA_GFX_D3D12
namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			R<RenderTargetViewDesc> get_default_rtv(Resource* res)
			{
				ResourceDesc d = res->desc();
				if (d.format == EResourceFormat::unknown)
				{
					return BasicError::bad_arguments();
				}
				switch (d.type)
				{
				case EResourceType::buffer:
					return BasicError::bad_arguments();
				case EResourceType::texture_1d:
					return (d.depth_or_array_size) == 1 ? 
						RenderTargetViewDesc::as_tex1d(d.format, 0) :
						RenderTargetViewDesc::as_tex1darray(d.format, 0, 0, d.depth_or_array_size);
				case EResourceType::texture_2d:
					return (d.depth_or_array_size == 1) ?
						((d.sample_count == 1) ?
							RenderTargetViewDesc::as_tex2d(d.format, 0) :
							RenderTargetViewDesc::as_tex2dms(d.format)) :
							((d.sample_count == 1) ?
								RenderTargetViewDesc::as_tex2darray(d.format, 0, 0, d.depth_or_array_size) :
								RenderTargetViewDesc::as_tex2dmsarray(d.format, 0, d.depth_or_array_size)
								);
				case EResourceType::texture_3d:
					return RenderTargetViewDesc::as_tex3d(d.format, 0, 0, d.depth_or_array_size);
				default:
					lupanic();
					break;
				}
				return BasicError::failure();
			}

			R<DepthStencilViewDesc> get_default_dsv(Resource* res)
			{
				ResourceDesc d = res->desc();
				if (d.format != EResourceFormat::d16_unorm &&
					d.format != EResourceFormat::d24_unorm_s8 &&
					d.format != EResourceFormat::d32_float &&
					d.format != EResourceFormat::d32_float_s8x24)
				{
					return BasicError::bad_arguments();
				}
				switch (d.type)
				{
				case EResourceType::buffer:
				case EResourceType::texture_3d:
					return BasicError::bad_arguments();
				case EResourceType::texture_1d:
					return (d.depth_or_array_size) == 1 ?
						DepthStencilViewDesc::as_tex1d(d.format, 0) :
						DepthStencilViewDesc::as_tex1darray(d.format, 0, 0, d.depth_or_array_size);
				case EResourceType::texture_2d:
					return (d.depth_or_array_size == 1) ?
						((d.sample_count == 1) ?
							DepthStencilViewDesc::as_tex2d(d.format, 0) :
							DepthStencilViewDesc::as_tex2dms(d.format)) :
							((d.sample_count == 1) ?
								DepthStencilViewDesc::as_tex2darray(d.format, 0, 0, d.depth_or_array_size) :
								DepthStencilViewDesc::as_tex2dmsarray(d.format, 0, d.depth_or_array_size)
								);
				default:
					lupanic();
					break;
				}
				return BasicError::failure();
			}

			RV FrameBuffer::init(u32 num_rtvs, IResource** rts, RenderTargetViewDesc** rtvs, IResource* ds, DepthStencilViewDesc* dsv)
			{
				lutry
				{
					// initialize RTVs.
					m_rtvs.resize(num_rtvs);
					m_rts.resize(num_rtvs);
					for (u32 i = 0; i < num_rtvs; ++i)
					{
						m_rts[i] = rts[i];
						if (!rtvs)
						{
							luset(m_rtvs[i], get_default_rtv(m_rts[i]));
						}
						else
						{
							if (rtvs[i])
							{
								m_rtvs[i] = *rtvs[i];
							}
							else
							{
								luset(m_rtvs[i], get_default_rtv(m_rts[i]));
							}
						}
					}

					if (num_rtvs)
					{
						// Create heap.
						{
							D3D12_DESCRIPTOR_HEAP_DESC d;
							d.NodeMask = 0;
							d.NumDescriptors = num_rtvs;
							d.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
							d.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
							if (FAILED(m_device->m_device->CreateDescriptorHeap(&d, IID_PPV_ARGS(&m_rtv_heap))))
							{
								return BasicError::failure();
							}
						}
						// Fill heap.
						{
							m_rtv_size = m_device->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
							D3D12_RENDER_TARGET_VIEW_DESC rtv;
							for (u32 i = 0; i < num_rtvs; ++i)
							{
								ID3D12Resource* res = m_rts[i]->m_res.Get();
								switch (m_rtvs[i].type)
								{
								case ERenderTargetViewType::buffer:
									rtv.ViewDimension = D3D12_RTV_DIMENSION_BUFFER;
									rtv.Buffer.FirstElement = m_rtvs[i].buffer.offset;
									rtv.Buffer.NumElements = m_rtvs[i].buffer.count;
									break;
								case ERenderTargetViewType::tex1d:
									rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
									rtv.Texture1D.MipSlice = m_rtvs[i].tex1d.mip_slice;
									break;
								case ERenderTargetViewType::tex1darray:
									rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
									rtv.Texture1DArray.ArraySize = m_rtvs[i].tex1darray.array_size;
									rtv.Texture1DArray.FirstArraySlice = m_rtvs[i].tex1darray.first_array_slice;
									rtv.Texture1DArray.MipSlice = m_rtvs[i].tex1darray.mip_slice;
									break;
								case ERenderTargetViewType::tex2d:
									rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
									rtv.Texture2D.MipSlice = m_rtvs[i].tex2d.mip_slice;
									rtv.Texture2D.PlaneSlice = 0;
									break;
								case ERenderTargetViewType::tex2darray:
									rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
									rtv.Texture2D.MipSlice = m_rtvs[i].tex2d.mip_slice;
									rtv.Texture2D.PlaneSlice = 0;
									break;
								case ERenderTargetViewType::tex2dms:
									rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
									break;
								case ERenderTargetViewType::tex2dmsarray:
									rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
									rtv.Texture2DMSArray.ArraySize = m_rtvs[i].tex2dmsarray.array_size;
									rtv.Texture2DMSArray.FirstArraySlice = m_rtvs[i].tex2dmsarray.first_array_slice;
									break;
								case ERenderTargetViewType::tex3d:
									rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
									rtv.Texture3D.FirstWSlice = m_rtvs[i].tex3d.first_layer_slice;
									rtv.Texture3D.MipSlice = m_rtvs[i].tex3d.mip_slice;
									rtv.Texture3D.WSize = m_rtvs[i].tex3d.layer_size;
									break;
								default:
									lupanic();
								}
								rtv.Format = encode_resource_format(m_rtvs[i].format);
								usize addr = m_rtv_heap->GetCPUDescriptorHandleForHeapStart().ptr + i * m_rtv_size;
								D3D12_CPU_DESCRIPTOR_HANDLE h;
								h.ptr = addr;
								m_device->m_device->CreateRenderTargetView(res, &rtv, h);
							}
						}
					}

					// Create DSV.
					if (ds)
					{
						m_ds = ds;
						if (dsv)
						{
							m_dsv = *dsv;
						}
						else
						{
							luset(m_dsv, get_default_dsv(m_ds));
						}
						// Create heap.
						{
							D3D12_DESCRIPTOR_HEAP_DESC d;
							d.NodeMask = 0;
							d.NumDescriptors = 1;
							d.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
							d.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
							if (FAILED(m_device->m_device->CreateDescriptorHeap(&d, IID_PPV_ARGS(&m_dsv_heap))))
							{
								return BasicError::failure();
							}
						}
						// Fill heap.
						{
							usize dsv_size = m_device->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
							D3D12_DEPTH_STENCIL_VIEW_DESC d;
							DepthStencilViewDesc* desc = &m_dsv;
							d.Format = encode_resource_format(desc->format);
							d.Flags = D3D12_DSV_FLAG_NONE;
							if (desc->depth_read_only)
							{
								d.Flags = d.Flags | D3D12_DSV_FLAG_READ_ONLY_DEPTH;
							}
							if (desc->stencil_read_only)
							{
								d.Flags = d.Flags | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
							}
							switch (desc->type)
							{
							case EDepthStencilViewType::tex1d:
								d.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
								d.Texture1D.MipSlice = desc->tex1d.mip_slice;
								break;
							case EDepthStencilViewType::tex1darray:
								d.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
								d.Texture1DArray.ArraySize = desc->tex1darray.array_size;
								d.Texture1DArray.FirstArraySlice = desc->tex1darray.first_array_slice;
								d.Texture1DArray.MipSlice = desc->tex1darray.mip_slice;
								break;
							case EDepthStencilViewType::tex2d:
								d.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
								d.Texture2D.MipSlice = desc->tex2d.mip_slice;
								break;
							case EDepthStencilViewType::tex2darray:
								d.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
								d.Texture2DArray.ArraySize = desc->tex2darray.array_size;
								d.Texture2DArray.FirstArraySlice = desc->tex2darray.first_array_slice;
								d.Texture2DArray.MipSlice = desc->tex2darray.mip_slice;
								break;
							case EDepthStencilViewType::tex2dms:
								d.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
								break;
							case EDepthStencilViewType::tex2dmsarray:
								d.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
								d.Texture2DMSArray.ArraySize = desc->tex2dmsarray.array_size;
								d.Texture2DMSArray.FirstArraySlice = desc->tex2dmsarray.first_array_slice;
							default:
								lupanic();
							}
							m_device->m_device->CreateDepthStencilView(m_ds->m_res.Get(), &d, m_dsv_heap->GetCPUDescriptorHandleForHeapStart());
						}
					}
				}
				lucatchret;
				return RV();
			}
		}
	}
}
#endif