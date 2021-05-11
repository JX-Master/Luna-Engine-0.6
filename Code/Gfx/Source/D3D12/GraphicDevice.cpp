// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file GraphicDevice.cpp
* @author JXMaster
* @date 2019/7/17
*/
#include "GraphicDevice.hpp"

#ifdef LUNA_GFX_D3D12

#include "Resource.hpp"
#include "PipelineState.hpp"
#include "CommandQueue.hpp"
#include "Resource.hpp"
#include "ResourceHeap.hpp"
#include "RenderPass.hpp"
#include "FrameBuffer.hpp"
#include "ViewSet.hpp"
#include "ShaderInputLayout.hpp"

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			GraphicDevice::~GraphicDevice()
			{
#ifdef LUNA_DEBUG_LEVEL_DEBUG
				ID3D12DebugDevice *pDebugDevice = nullptr;
				HRESULT hr = m_device->QueryInterface(__uuidof(ID3D12DebugDevice), reinterpret_cast<void**>(&pDebugDevice));
				if (SUCCEEDED(hr))
				{
					hr = pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
				}
				if (pDebugDevice != nullptr)
				{
					pDebugDevice->Release();
				}
#endif
			}

			bool GraphicDevice::init(ID3D12Device* dev)
			{
				m_device = dev;
				m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &m_feature_options, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS));

				return true;
			}
			void GraphicDevice::check_feature_support(EDeviceFeature feature, void* data)
			{
				switch (feature)
				{
				case EDeviceFeature::texture_data_pitch_alignment:
					*((u32*)data) = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
					break;
				case EDeviceFeature::texture_data_placement_alignment:
					*((u32*)data) = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
					break;
				case EDeviceFeature::buffer_data_alignment:
					*((u32*)data) = 256;
					break;
				case EDeviceFeature::placed_resource_supported:
					*((bool*)data) = true;
					break;
				default:
					break;
				}
			}
			RP<IResource> GraphicDevice::new_resource(const ResourceDesc& desc, const ClearValue* optimized_clear_value)
			{
				P<Resource> res = newobj<Resource>();
				if (!res)
				{
					return BasicError::bad_memory_alloc();
				}
				res->m_device = (GraphicDevice*)this;
				RV r = res->init(desc, optimized_clear_value);
				if (!r.valid())
				{
					return r.errcode();
				}
				return res;
			}
			RP<IResourceHeap> GraphicDevice::new_resource_heap(const ResourceHeapDesc& desc)
			{
				P<ResourceHeap> h = newobj<ResourceHeap>(this);
				if (!h)
				{
					return BasicError::bad_memory_alloc();
				}
				auto r = h->init(desc);
				if (!r.valid())
				{
					return r.errcode();
				}
				return h;
			}
			RP<IShaderInputLayout> GraphicDevice::new_shader_input_layout(const ShaderInputLayoutDesc& desc)
			{
				P<ShaderInputLayout> slayout = newobj<ShaderInputLayout>();
				if (!slayout)
				{
					return BasicError::bad_memory_alloc();
				}
				lutry
				{
					slayout->m_device = (GraphicDevice*)this;
					luexp(slayout->init(desc));
				}
				lucatchret;
				return slayout;
			}
			RP<IPipelineState> GraphicDevice::new_graphics_pipeline_state(IShaderInputLayout* shader_input_layout, IRenderPass* render_pass, const GraphicsPipelineStateDesc& desc)
			{
				P<PipelineState> s = newobj<PipelineState>(this);
				if (!s)
				{
					return BasicError::bad_memory_alloc();
				}
				if (!s->init_as_graphic(desc, render_pass, shader_input_layout))
				{
					return BasicError::failure();
				}
				return s;
			}
			RP<IPipelineState> GraphicDevice::new_compute_pipline_state(IShaderInputLayout* shader_input_layout, const ComputePipelineStateDesc& desc)
			{
				P<PipelineState> s = newobj<PipelineState>(this);
				if (!s)
				{
					return BasicError::bad_memory_alloc();
				}
				if (!s->init_as_compute(desc, shader_input_layout))
				{
					return BasicError::failure();
				}
				return s;
			}

			RP<ICommandQueue> GraphicDevice::new_command_queue(const CommandQueueDesc& desc)
			{
				P<CommandQueue> q = newobj<CommandQueue>(this);
				RV r = q->init(desc);
				if (!r.valid())
				{
					return r.errcode();
				}
				return q;
			}

			RP<IRenderPass> GraphicDevice::new_render_pass(const RenderPassDesc& desc)
			{
				P<RenderPass> rp = newobj<RenderPass>();
				if (!rp)
				{
					return BasicError::bad_memory_alloc();
				}
				rp->m_device = (GraphicDevice*)this;
				rp->m_desc = desc;
				return rp;
			}

			RP<IFrameBuffer> GraphicDevice::new_frame_buffer(IRenderPass* render_pass, u32 num_rtvs, IResource ** rts, RenderTargetViewDesc ** rtvs, IResource * ds, DepthStencilViewDesc * dsv)
			{
				P<FrameBuffer> rp = newobj<FrameBuffer>();
				rp->m_device = (GraphicDevice*)this;
				RV r = rp->init(num_rtvs, rts, rtvs, ds, dsv);
				if (!r.valid())
				{
					return r.errcode();
				}
				return rp;
			}

			RP<IViewSet> GraphicDevice::new_view_set(IShaderInputLayout* shader_input_layout, const ViewSetDesc& desc)
			{
				P<ViewSet> vs = newobj<ViewSet>();
				vs->m_device = (GraphicDevice*)this;
				RV r = vs->init(desc);
				if (!r.valid())
				{
					return r.errcode();
				}
				return vs;
			}

			void GraphicDevice::calc_texture_subresource_buffer_placement(u32 width, u32 height, u32 depth, EResourceFormat format, usize* row_pitch, usize* slice_pitch, usize* res_pitch)
			{
				u64 numBytes = 0;
				u64 rowBytes = 0;
				u64 numRows = 0;

				bool bc = false;
				bool packed = false;
				bool planar = false;
				usize bpe = 0;
				DXGI_FORMAT fmt = encode_resource_format(format);
				switch (fmt)
				{
				case DXGI_FORMAT_BC1_TYPELESS:
				case DXGI_FORMAT_BC1_UNORM:
				case DXGI_FORMAT_BC1_UNORM_SRGB:
				case DXGI_FORMAT_BC4_TYPELESS:
				case DXGI_FORMAT_BC4_UNORM:
				case DXGI_FORMAT_BC4_SNORM:
					bc = true;
					bpe = 8;
					break;

				case DXGI_FORMAT_BC2_TYPELESS:
				case DXGI_FORMAT_BC2_UNORM:
				case DXGI_FORMAT_BC2_UNORM_SRGB:
				case DXGI_FORMAT_BC3_TYPELESS:
				case DXGI_FORMAT_BC3_UNORM:
				case DXGI_FORMAT_BC3_UNORM_SRGB:
				case DXGI_FORMAT_BC5_TYPELESS:
				case DXGI_FORMAT_BC5_UNORM:
				case DXGI_FORMAT_BC5_SNORM:
				case DXGI_FORMAT_BC6H_TYPELESS:
				case DXGI_FORMAT_BC6H_UF16:
				case DXGI_FORMAT_BC6H_SF16:
				case DXGI_FORMAT_BC7_TYPELESS:
				case DXGI_FORMAT_BC7_UNORM:
				case DXGI_FORMAT_BC7_UNORM_SRGB:
					bc = true;
					bpe = 16;
					break;

				case DXGI_FORMAT_R8G8_B8G8_UNORM:
				case DXGI_FORMAT_G8R8_G8B8_UNORM:
				case DXGI_FORMAT_YUY2:
					packed = true;
					bpe = 4;
					break;

				case DXGI_FORMAT_Y210:
				case DXGI_FORMAT_Y216:
					packed = true;
					bpe = 8;
					break;

				case DXGI_FORMAT_NV12:
				case DXGI_FORMAT_420_OPAQUE:
				case DXGI_FORMAT_P208:
					planar = true;
					bpe = 2;
					break;

				case DXGI_FORMAT_P010:
				case DXGI_FORMAT_P016:
					planar = true;
					bpe = 4;
					break;

				default:
					break;
				}

				if (bc)
				{
					u64 numBlocksWide = 0;
					if (width > 0)
					{
						numBlocksWide = max<u64>(1u, (u64(width) + 3u) / 4u);
					}
					u64 numBlocksHigh = 0;
					if (height > 0)
					{
						numBlocksHigh = max<u64>(1u, (u64(height) + 3u) / 4u);
					}
					rowBytes = numBlocksWide * bpe;
					numRows = numBlocksHigh;
					numBytes = rowBytes * numBlocksHigh;
				}
				else if (packed)
				{
					rowBytes = ((u64(width) + 1u) >> 1) * bpe;
					numRows = u64(height);
					numBytes = rowBytes * height;
				}
				else if (fmt == DXGI_FORMAT_NV11)
				{
					rowBytes = ((u64(width) + 3u) >> 2) * 4u;
					numRows = u64(height) * 2u; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
					numBytes = rowBytes * numRows;
				}
				else if (planar)
				{
					rowBytes = ((u64(width) + 1u) >> 1) * bpe;
					numBytes = (rowBytes * u64(height)) + ((rowBytes* u64(height) + 1u) >> 1);
					numRows = height + ((u64(height) + 1u) >> 1);
				}
				else
				{
					usize bpp = bits_per_pixel(format);
					if (!bpp)
					{
						if (row_pitch)
						{
							*row_pitch = 0;
						}
						if (slice_pitch)
						{
							*slice_pitch = 0;
						}
						if (res_pitch)
						{
							*res_pitch = 0;
						}
						return;
					}


					rowBytes = (uint64_t(width) * bpp + 7u) / 8u; // round up to nearest byte
					numRows = uint64_t(height);
					numBytes = rowBytes * height;
				}

				if (row_pitch)
				{
					*row_pitch = (usize)rowBytes;
				}
				if (slice_pitch)
				{
					*slice_pitch = (usize)numBytes;
				}
				if (res_pitch)
				{
					*res_pitch = (usize)(numBytes * depth);
				}
			}
		}
	}
}

#endif