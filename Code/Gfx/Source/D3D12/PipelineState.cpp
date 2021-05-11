// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PipelineState.cpp
* @author JXMaster
* @date 2019/8/15
*/
#include "PipelineState.hpp"

#ifdef LUNA_GFX_D3D12

#include "GraphicSystem.hpp"
#include "D3D12Common.hpp"

#include <Runtime/Vector.hpp>

#include "RenderPass.hpp"

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			D3D12_BLEND encode_blend_factor(EBlendFactor f)
			{
				switch (f)
				{
				case EBlendFactor::zero:
					return D3D12_BLEND_ZERO;
				case EBlendFactor::one:
					return D3D12_BLEND_ONE;
				case EBlendFactor::src_color:
					return D3D12_BLEND_SRC_COLOR;
				case EBlendFactor::inv_src_color:
					return D3D12_BLEND_INV_SRC1_COLOR;
				case EBlendFactor::src_alpha:
					return D3D12_BLEND_SRC_ALPHA;
				case EBlendFactor::inv_src_alpha:
					return D3D12_BLEND_INV_SRC_ALPHA;
				case EBlendFactor::dest_alpha:
					return D3D12_BLEND_DEST_ALPHA;
				case EBlendFactor::inv_dest_alpha:
					return D3D12_BLEND_INV_DEST_ALPHA;
				case EBlendFactor::dest_color:
					return D3D12_BLEND_DEST_COLOR;
				case EBlendFactor::inv_dest_color:
					return D3D12_BLEND_INV_DEST_COLOR;
				case EBlendFactor::src_alpha_sat:
					return D3D12_BLEND_SRC_ALPHA_SAT;
				case EBlendFactor::blend_factor:
					return D3D12_BLEND_BLEND_FACTOR;
				case EBlendFactor::inv_blend_factor:
					return D3D12_BLEND_INV_BLEND_FACTOR;
				case EBlendFactor::src1_color:
					return D3D12_BLEND_SRC1_COLOR;
				case EBlendFactor::inv_src1_color:
					return D3D12_BLEND_INV_SRC1_COLOR;
				case EBlendFactor::src1_alpha:
					return D3D12_BLEND_SRC1_ALPHA;
				case EBlendFactor::inv_src1_alpha:
				default:
					lupanic();
					return D3D12_BLEND_ONE;
				}
			}

			D3D12_BLEND_OP encode_blend_op(EBlendOp o)
			{
				switch (o)
				{
				case EBlendOp::add:
					return D3D12_BLEND_OP_ADD;
				case EBlendOp::subtract:
					return D3D12_BLEND_OP_SUBTRACT;
				case EBlendOp::rev_subtract:
					return D3D12_BLEND_OP_REV_SUBTRACT;
				case EBlendOp::min:
					return D3D12_BLEND_OP_MIN;
				case EBlendOp::max:
				default:
					lupanic();
					return D3D12_BLEND_OP_MAX;
				}
			}

			D3D12_LOGIC_OP encode_logic_op(ELogicOp o)
			{
				switch (o)
				{
				case ELogicOp::clear:
					return D3D12_LOGIC_OP_CLEAR;
				case ELogicOp::set:
					return D3D12_LOGIC_OP_SET;
				case ELogicOp::copy:
					return D3D12_LOGIC_OP_COPY;
				case ELogicOp::copy_inverted:
					return D3D12_LOGIC_OP_COPY_INVERTED;
				case ELogicOp::noop:
					return D3D12_LOGIC_OP_NOOP;
				case ELogicOp::invert:
					return D3D12_LOGIC_OP_INVERT;
				case ELogicOp::and:
					return D3D12_LOGIC_OP_AND;
				case ELogicOp::nand:
					return D3D12_LOGIC_OP_NAND;
				case ELogicOp::or:
					return D3D12_LOGIC_OP_OR;
				case ELogicOp::nor:
					return D3D12_LOGIC_OP_NOR;
				case ELogicOp::xor:
					return D3D12_LOGIC_OP_XOR;
				case ELogicOp::equiv:
					return D3D12_LOGIC_OP_EQUIV;
				case ELogicOp::and_reverse:
					return D3D12_LOGIC_OP_AND_REVERSE;
				case ELogicOp::and_inverted:
					return D3D12_LOGIC_OP_AND_INVERTED;
				case ELogicOp::or_reverse:
					return D3D12_LOGIC_OP_OR_REVERSE;
				case ELogicOp::or_inverted:
				default:
					lupanic();
					return D3D12_LOGIC_OP_OR_INVERTED;
				}
			}

			D3D12_STENCIL_OP encode_stencil_op(EStencilOp op)
			{
				switch (op)
				{
				case EStencilOp::keep:
					return D3D12_STENCIL_OP_KEEP;
				case EStencilOp::zero:
					return D3D12_STENCIL_OP_ZERO;
				case EStencilOp::replace:
					return D3D12_STENCIL_OP_REPLACE;
				case EStencilOp::incr_sat:
					return D3D12_STENCIL_OP_INCR_SAT;
				case EStencilOp::decr_sat:
					return D3D12_STENCIL_OP_DECR;
				case EStencilOp::invert:
					return D3D12_STENCIL_OP_INVERT;
				case EStencilOp::incr:
					return D3D12_STENCIL_OP_INCR;
				case EStencilOp::decr:
				default:
					lupanic();
					return D3D12_STENCIL_OP_KEEP;
				}
			}

			bool PipelineState::init_as_graphic(const GraphicsPipelineStateDesc& desc, IRenderPass* render_pass, IShaderInputLayout* shader_input_layout)
			{
				RenderPass* rp = static_cast<RenderPass*>(render_pass);
				ShaderInputLayout* slayout = static_cast<ShaderInputLayout*>(shader_input_layout);
				D3D12_GRAPHICS_PIPELINE_STATE_DESC d;
				d.pRootSignature = slayout->m_rs.Get();
				d.VS.BytecodeLength = desc.vs.length;
				d.VS.pShaderBytecode = desc.vs.code;
				d.PS.BytecodeLength = desc.ps.length;
				d.PS.pShaderBytecode = desc.ps.code;
				d.DS.BytecodeLength = desc.ds.length;
				d.DS.pShaderBytecode = desc.ds.code;
				d.HS.BytecodeLength = desc.hs.length;
				d.HS.pShaderBytecode = desc.hs.code;
				d.GS.BytecodeLength = desc.gs.length;
				d.GS.pShaderBytecode = desc.gs.code;

				Vector<D3D12_SO_DECLARATION_ENTRY> stream_entries;
				d.StreamOutput.NumEntries = desc.stream_output.num_entries;
				d.StreamOutput.NumStrides = desc.stream_output.num_strides;
				d.StreamOutput.pBufferStrides = desc.stream_output.buffer_strides;
				d.StreamOutput.RasterizedStream = desc.stream_output.rasterized_stream;

				if (desc.stream_output.entries)
				{
					stream_entries.resize(desc.stream_output.num_entries);
					d.StreamOutput.pSODeclaration = stream_entries.data();
					for (usize i = 0; i < stream_entries.size(); ++i)
					{
						D3D12_SO_DECLARATION_ENTRY& dst = stream_entries[i];
						const StreamOutputDeclarationEntry& src = desc.stream_output.entries[i];
						dst.ComponentCount = src.component_count;
						dst.OutputSlot = src.output_slot;
						dst.SemanticIndex = src.semantic_index;
						dst.SemanticName = src.semantic_name;
						dst.StartComponent = src.start_component;
						dst.Stream = src.stream;
					}
				}
				else
				{
					d.StreamOutput.pSODeclaration = nullptr;
				}
				
				{
					d.BlendState.AlphaToCoverageEnable = desc.blend_state.alpha_to_coverage_enable ? TRUE : FALSE;
					d.BlendState.IndependentBlendEnable = desc.blend_state.independent_blend_enable ? TRUE : FALSE;
					for (u32 i = 0; i < 8; ++i)
					{
						D3D12_RENDER_TARGET_BLEND_DESC& rt = d.BlendState.RenderTarget[i];
						const RenderTargetBlendDesc& srt = desc.blend_state.rt[i];
						rt.BlendEnable = srt.blend_enable ? TRUE : FALSE;
						rt.LogicOpEnable = srt.logic_op_enable ? TRUE : FALSE;
						rt.SrcBlend = encode_blend_factor(srt.src_blend);
						rt.DestBlend = encode_blend_factor(srt.dest_blend);
						rt.BlendOp = encode_blend_op(srt.blend_op);
						rt.SrcBlendAlpha = encode_blend_factor(srt.src_blend_alpha);
						rt.DestBlendAlpha = encode_blend_factor(srt.dest_blend_alpha);
						rt.BlendOpAlpha = encode_blend_op(srt.blend_op_alpha);
						rt.LogicOp = encode_logic_op(srt.logic_op);
						rt.RenderTargetWriteMask = 0;
						if ((srt.render_target_write_mask & EColorWriteMask::red) != EColorWriteMask::none)
						{
							rt.RenderTargetWriteMask = rt.RenderTargetWriteMask | D3D12_COLOR_WRITE_ENABLE_RED;
						}
						if ((srt.render_target_write_mask & EColorWriteMask::green) != EColorWriteMask::none)
						{
							rt.RenderTargetWriteMask = rt.RenderTargetWriteMask | D3D12_COLOR_WRITE_ENABLE_GREEN;
						}
						if ((srt.render_target_write_mask & EColorWriteMask::blue) != EColorWriteMask::none)
						{
							rt.RenderTargetWriteMask = rt.RenderTargetWriteMask | D3D12_COLOR_WRITE_ENABLE_BLUE;
						}
						if ((srt.render_target_write_mask & EColorWriteMask::alpha) != EColorWriteMask::none)
						{
							rt.RenderTargetWriteMask = rt.RenderTargetWriteMask | D3D12_COLOR_WRITE_ENABLE_ALPHA;
						}
					}
					d.SampleMask = desc.sample_mask;
				}

				{
					switch (desc.rasterizer_state.fill_mode)
					{
					case EFillMode::solid:
						d.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
						break;
					case EFillMode::wireframe:
						d.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
						break;
					}
					switch (desc.rasterizer_state.cull_mode)
					{
					case ECullMode::back:
						d.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
						break;
					case ECullMode::front:
						d.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
						break;
					case ECullMode::none:
						d.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
						break;
					}
					d.RasterizerState.ForcedSampleCount = desc.rasterizer_state.forced_sample_count;
					d.RasterizerState.FrontCounterClockwise = desc.rasterizer_state.front_counter_clockwise ? TRUE : FALSE;
					d.RasterizerState.DepthBias = desc.rasterizer_state.depth_bias;
					d.RasterizerState.DepthBiasClamp = desc.rasterizer_state.depth_bias_clamp;
					d.RasterizerState.SlopeScaledDepthBias = desc.rasterizer_state.slope_scaled_depth_bias;
					d.RasterizerState.DepthClipEnable = desc.rasterizer_state.depth_clip_enable ? TRUE : FALSE;
					d.RasterizerState.MultisampleEnable = desc.rasterizer_state.multisample_enable ? TRUE : FALSE;
					d.RasterizerState.AntialiasedLineEnable = desc.rasterizer_state.antialiased_line_enable ? TRUE : FALSE;
					d.RasterizerState.ConservativeRaster = desc.rasterizer_state.conservative_raster_enabled ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
				}

				{
					d.DepthStencilState.DepthEnable = desc.depth_stencil_state.depth_test_enable ? TRUE : FALSE;
					d.DepthStencilState.DepthWriteMask = desc.depth_stencil_state.depth_write_enable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
					d.DepthStencilState.DepthFunc = encode_comparison_func(desc.depth_stencil_state.depth_func);
					d.DepthStencilState.StencilEnable = desc.depth_stencil_state.stencil_enable ? TRUE : FALSE;
					d.DepthStencilState.StencilReadMask = desc.depth_stencil_state.stencil_read_mask;
					d.DepthStencilState.StencilWriteMask = desc.depth_stencil_state.stencil_write_mask;
					d.DepthStencilState.FrontFace.StencilFailOp = encode_stencil_op(desc.depth_stencil_state.front_face.stencil_fail_op);
					d.DepthStencilState.FrontFace.StencilDepthFailOp = encode_stencil_op(desc.depth_stencil_state.front_face.stencil_depth_fail_op);
					d.DepthStencilState.FrontFace.StencilPassOp = encode_stencil_op(desc.depth_stencil_state.front_face.stencil_pass_op);
					d.DepthStencilState.FrontFace.StencilFunc = encode_comparison_func(desc.depth_stencil_state.front_face.stencil_func);
					d.DepthStencilState.BackFace.StencilFailOp = encode_stencil_op(desc.depth_stencil_state.back_face.stencil_fail_op);
					d.DepthStencilState.BackFace.StencilDepthFailOp = encode_stencil_op(desc.depth_stencil_state.back_face.stencil_depth_fail_op);
					d.DepthStencilState.BackFace.StencilPassOp = encode_stencil_op(desc.depth_stencil_state.back_face.stencil_pass_op);
					d.DepthStencilState.BackFace.StencilFunc = encode_comparison_func(desc.depth_stencil_state.back_face.stencil_func);
				}

				Vector<D3D12_INPUT_ELEMENT_DESC> input_elements;
				if (desc.input_layout.num_elements)
				{
					u32 num_elements = desc.input_layout.num_elements;
					input_elements.resize(num_elements);
					d.InputLayout.NumElements = num_elements;
					d.InputLayout.pInputElementDescs = input_elements.data();
					for (u32 i = 0; i < num_elements; ++i)
					{
						D3D12_INPUT_ELEMENT_DESC& e = input_elements[i];
						const InputElementDesc& se = desc.input_layout.input_elements[i];
						e.SemanticName = se.semantic_name;
						e.SemanticIndex = se.semantic_index;
						e.Format = encode_resource_format(se.format);
						e.InputSlot = se.input_slot;
						e.AlignedByteOffset = se.aligned_byte_offset;
						switch (se.input_slot_class)
						{
						case EInputClassification::per_vertex:
							e.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
							break;
						case EInputClassification::per_instance:
							e.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
							break;
						}
						e.InstanceDataStepRate = se.instance_data_step_rate;
					}
				}
				else
				{
					d.InputLayout.NumElements = 0;
					d.InputLayout.pInputElementDescs = nullptr;
				}

				switch (desc.ib_strip_cut_value)
				{
				case EIndexBufferStripCutValue::disabled:
					d.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
					break;
				case EIndexBufferStripCutValue::value_0xffff:
					d.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF;
					break;
				case EIndexBufferStripCutValue::value_0xffffffff:
					d.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;
					break;
				}

				switch (desc.primitive_topology_type)
				{
				case EPrimitiveTopologyType::line:
					d.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
					break;
				case EPrimitiveTopologyType::patch:
					d.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
					break;
				case EPrimitiveTopologyType::point:
					d.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
					break;
				case EPrimitiveTopologyType::triangle:
					d.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
					break;
				case EPrimitiveTopologyType::undefined:
					d.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
					break;
				}

				d.NumRenderTargets = rp->m_desc.num_attachments;
				for (u32 i = 0; i < 8; ++i)
				{
					d.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
				}
				for (u32 i = 0; i < d.NumRenderTargets; ++i)
				{
					d.RTVFormats[i] = encode_resource_format(rp->m_desc.attachments[i].format);
				}
				d.DSVFormat = encode_resource_format(rp->m_desc.depth_stencil_format);
				d.SampleDesc.Count = rp->m_desc.sample_count;
				d.SampleDesc.Quality = desc.sample_quality;
				d.NodeMask = 0;
				d.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
				d.CachedPSO.CachedBlobSizeInBytes = 0;
				d.CachedPSO.pCachedBlob = nullptr;

				if (FAILED(m_device->m_device->CreateGraphicsPipelineState(&d, IID_PPV_ARGS(&m_pso))))
				{
					return false;
				}
				m_is_graphic = true;
				return true;
			}
			bool PipelineState::init_as_compute(const ComputePipelineStateDesc& desc, IShaderInputLayout* shader_input_layout)
			{
				ShaderInputLayout* slayout = static_cast<ShaderInputLayout*>(shader_input_layout);
				D3D12_COMPUTE_PIPELINE_STATE_DESC d;
				d.pRootSignature = slayout->m_rs.Get();
				d.CachedPSO.CachedBlobSizeInBytes = 0;
				d.CachedPSO.pCachedBlob = nullptr;
				d.CS.pShaderBytecode = desc.cs.code;
				d.CS.BytecodeLength = desc.cs.length;
				d.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
				d.NodeMask = 0;
				if (FAILED(m_device->m_device->CreateComputePipelineState(&d, IID_PPV_ARGS(&m_pso))))
				{
					return false;
				}
				m_is_graphic = false;
				return true;
			}
		}
	}
}

#endif