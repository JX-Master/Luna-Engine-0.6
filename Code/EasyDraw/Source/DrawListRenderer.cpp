// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file DrawListRenderer.cpp
* @author JXMaster
* @date 2019/10/6
*/
#include "DrawListRenderer.hpp"
#include <math.h>

namespace Luna
{
	namespace EasyDraw
	{
		using namespace Gfx;
		RV DrawListRenderer::init()
		{
			lutry
			{
				// Create pipeline state.
				InputElementDesc input_elements[3] = {
				InputElementDesc("POSITION", 0, EResourceFormat::rg32_float),
				InputElementDesc("TEXCOORD", 0, EResourceFormat::rg32_float),
				InputElementDesc("COLOR", 0, EResourceFormat::rgba32_float)
				};

				InputLayoutDesc il(3, input_elements);

				ShaderInputGroupDesc ranges[3] =
				{
					ShaderInputGroupDesc(EShaderInputGroupType::cbv, 0, 1, EShaderVisibility::vertex),
					ShaderInputGroupDesc(EShaderInputGroupType::srv, 0, 1, EShaderVisibility::pixel),
					ShaderInputGroupDesc(EShaderInputGroupType::sampler, 0, 1, EShaderVisibility::pixel)
				};
				luset(m_slayout, m_rp->get_device()->new_shader_input_layout(ShaderInputLayoutDesc(ranges, 3,
					EShaderInputLayoutFlag::allow_input_assembler_input_layout | EShaderInputLayoutFlag::deny_domain_shader_access | 
					EShaderInputLayoutFlag::deny_geometry_shader_access | EShaderInputLayoutFlag::deny_hull_shader_access)));
				luset(m_slayout_no_tex, m_rp->get_device()->new_shader_input_layout(ShaderInputLayoutDesc(ranges, 1,
					EShaderInputLayoutFlag::allow_input_assembler_input_layout | EShaderInputLayoutFlag::deny_domain_shader_access |
					EShaderInputLayoutFlag::deny_geometry_shader_access | EShaderInputLayoutFlag::deny_hull_shader_access | EShaderInputLayoutFlag::deny_pixel_shader_access)));
				
				GraphicsPipelineStateDesc psd(
					il,
					ShaderBytecode(g_vs.data(), g_vs.size()),
					ShaderBytecode(g_ps.data(), g_ps.size()),
					ShaderBytecode(Default()),
					ShaderBytecode(Default()),
					ShaderBytecode(Default()),
					StreamOutputDesc(Default()),
					BlendDesc(false, false, { RenderTargetBlendDesc(true, false, EBlendFactor::src_alpha, EBlendFactor::inv_src_alpha, EBlendOp::add,
						EBlendFactor::src_alpha, EBlendFactor::src_alpha, EBlendOp::add, ELogicOp::noop, EColorWriteMask::all) }),
					RasterizerDesc(EFillMode::solid, ECullMode::none, 0, 0.0f, 0.0f, 0, false, false, false, false, false),
					DepthStencilDesc(false, false, EComparisonFunc::always, false, 0xff, 0xff, DepthStencilOpDesc(Default()), DepthStencilOpDesc(Default())),
					EIndexBufferStripCutValue::disabled,
					EPrimitiveTopologyType::triangle,
					0xffffffff, 0
				);
				luset(m_pso, m_rp->get_device()->new_graphics_pipeline_state(m_slayout, m_rp, psd));
				psd.ps.code = g_ps_no_tex.data();
				psd.ps.length = g_ps_no_tex.size();
				luset(m_pso_no_tex, m_rp->get_device()->new_graphics_pipeline_state(m_slayout_no_tex, m_rp, psd));

			}
			lucatchret;
			return RV();
		}
		RV DrawListRenderer::render(Gfx::ICommandBuffer* target_cmd_buffer, Gfx::IResource* rt, u32 width, u32 height)
		{
			lutry
			{
				u32 cb_align;
				m_rp->get_device()->check_feature_support(EDeviceFeature::buffer_data_alignment, &cb_align);
				usize cb_size = align_upper(sizeof(Float4x4), cb_align);
				// prepare buffer.
				{
					// collect sizes.
					usize vb_sz = 0;
					usize ib_sz = 0;
					usize cb_sz = 0;
					usize num_dcs = 0;
					for (auto& i : m_dcs)
					{
						for (u32 j = 0; j < i.m_count; ++j)
						{
							DrawCall dc;
							i.m_list->enum_draw_call(j, dc);
							vb_sz += dc.num_vertices * sizeof(PrimitiveVertex);
							ib_sz += dc.num_indices * sizeof(u32);
							cb_sz += cb_size;
							++num_dcs;
						}
					}
					m_vb_count = vb_sz / sizeof(PrimitiveVertex);
					m_ib_count = ib_sz / sizeof(u32);
					m_cb_count = num_dcs;
					if (!m_vb_res || m_vb_size < vb_sz)
					{
						luset(m_vb_res, m_rp->get_device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::vertex_buffer, vb_sz)));
						m_vb_size = vb_sz;
					}
					if (!m_ib_res || m_ib_size < ib_sz)
					{
						luset(m_ib_res, m_rp->get_device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::index_buffer, ib_sz)));
						m_ib_size = ib_sz;
					}
					if (!m_cb_res || m_cb_size < cb_sz)
					{
						luset(m_cb_res, m_rp->get_device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::constant_buffer, cb_sz)));
						m_cb_size = cb_sz;
					}
				}
				// Copy resources.
				{
					usize ib_off = 0;
					usize vb_off = 0;
					usize cb_off = 0;
					lulet(vb_data, m_vb_res->map_subresource(0, false, 1, 0));
					lulet(ib_data, m_ib_res->map_subresource(0, false, 1, 0));
					lulet(cb_data, m_cb_res->map_subresource(0, false, 1, 0));
					PrimitiveVertex* verts = (PrimitiveVertex*)vb_data;
					u32* indices = (u32*)ib_data;
					for (auto& i : m_dcs)
					{
						for (u32 j = 0; j < i.m_count; ++j)
						{
							DrawCall dc;
							i.m_list->enum_draw_call(j, dc);
							memcpy(verts + vb_off, dc.vertices, sizeof(PrimitiveVertex) * dc.num_vertices);
							memcpy(indices + ib_off, dc.indices, sizeof(u32) * dc.num_indices);
							f32 rad = deg_to_rad(dc.rotation);
							f32 sinr = sinf(rad);
							f32 cosr = cosf(rad);
							Float4x4 mvp(
								2.0f * cosr / width, -2.0f * sinr / height, 0.0f, 0.0f,
								-2.0f * sinr / width, -2.0f * cosr / height, 0.0f, 0.0f,
								0.0f, 0.0f, 1.0f, 0.0f,
								2.0f * dc.origin_x / width - 1.0f, 1.0f - 2.0f * dc.origin_y / height, 0.0f, 1.0f
							);
							Float4U* cb_dest = (Float4U*)((usize)cb_data + cb_off);
							memcpy(cb_dest, mvp.m, sizeof(Float4x4));
							vb_off += dc.num_vertices;
							ib_off += dc.num_indices;
							cb_off += cb_size;
						}
					}
					m_vb_res->unmap_subresource(0, true);
					m_ib_res->unmap_subresource(0, true);
					m_cb_res->unmap_subresource(0, true);
				}

				// Issue commands.
				{
					lulet(fbo, m_rp->get_device()->new_frame_buffer(m_rp, 1, &rt, nullptr, nullptr, nullptr));
					target_cmd_buffer->attach_graphic_object(fbo);
					ResourceBarrierDesc barriers[] =
					{
						ResourceBarrierDesc::as_transition(rt, EResourceState::render_target),
						ResourceBarrierDesc::as_transition(m_ib_res, EResourceState::index_buffer),
						ResourceBarrierDesc::as_transition(m_vb_res, EResourceState::vertex_and_constant_buffer),
						ResourceBarrierDesc::as_transition(m_cb_res, EResourceState::vertex_and_constant_buffer),
					};
					target_cmd_buffer->resource_barriers(4, barriers);
					target_cmd_buffer->begin_render_pass(m_rp, fbo, 0, nullptr, 1.0f, 0xff);
					target_cmd_buffer->set_primitive_topology(EPrimitiveTopology::triangle_list);
					target_cmd_buffer->set_vertex_buffers(0, 1, &VertexBufferViewDesc(m_vb_res, 0, (u32)m_vb_size, (u32)sizeof(PrimitiveVertex)));
					target_cmd_buffer->set_index_buffer(m_ib_res, 0, (u32)m_ib_size, EResourceFormat::r32_uint);
					ResourceDesc d = rt->desc();
					target_cmd_buffer->set_viewports(1, &Viewport(0.0f, 0.0f, (f32)d.width, (f32)d.height, 0.0f, 1.0f));

					// VB & IB offsets.
					usize vb_off = 0;
					usize ib_off = 0;
					usize cb_off = 0;
					for (auto& i : m_dcs)
					{
						for (u32 j = 0; j < i.m_count; ++j)
						{
							DrawCall dc;
							i.m_list->enum_draw_call(j, dc);
							// Create & set view sets.
							P<IViewSet> view_set;
							if (dc.texture)
							{
								target_cmd_buffer->set_pipeline_state(m_pso);
								target_cmd_buffer->set_graphic_shader_input_layout(m_slayout);
								luset(view_set, m_rp->get_device()->new_view_set(m_slayout, ViewSetDesc(1, 1, 0, 1)));
								view_set->set_cbv(0, m_cb_res, ConstantBufferViewDesc((u32)cb_off, (u32)cb_size));
								cb_off += cb_size;
								view_set->set_srv(0, dc.texture, nullptr);
								view_set->set_sampler(0, dc.sampler);
								target_cmd_buffer->resource_barrier(ResourceBarrierDesc::as_transition(dc.texture, EResourceState::shader_resource_pixel));
							}
							else
							{
								target_cmd_buffer->set_pipeline_state(m_pso_no_tex);
								target_cmd_buffer->set_graphic_shader_input_layout(m_slayout_no_tex);
								luset(view_set, m_rp->get_device()->new_view_set(m_slayout_no_tex, ViewSetDesc(1, 0, 0, 0)));
								view_set->set_cbv(0, m_cb_res, ConstantBufferViewDesc((u32)cb_off, (u32)cb_size));
								cb_off += cb_size;
							}
							target_cmd_buffer->attach_graphic_object(view_set);
							target_cmd_buffer->set_graphic_view_set(view_set);
							if (dc.clip_rect == RectI(0, 0, 0, 0))
							{
								target_cmd_buffer->set_scissor_rects(1, &RectI(0, 0, (i32)d.width, d.height));
							}
							else
							{
								f32 width_ratio = (f32)d.width / (f32)width;
								f32 height_ratio = (f32)d.height / (f32)height;
								RectI& r = dc.clip_rect;
								RectI scaled((i32)((r.left + (i32)dc.origin_x) * width_ratio), (i32)((r.top + (i32)dc.origin_y) * height_ratio),
									(i32)((r.right + (i32)dc.origin_x) * width_ratio), (i32)((r.bottom + (i32)dc.origin_y) * height_ratio));
								target_cmd_buffer->set_scissor_rects(1, &scaled);
							}
							target_cmd_buffer->draw_indexed((u32)dc.num_indices, (u32)ib_off, (u32)vb_off);
							vb_off += dc.num_vertices;
							ib_off += dc.num_indices;
						}
					}
					target_cmd_buffer->end_render_pass();
				}
				m_dcs.clear();
			}
			lucatchret;
			return RV();
		}
	}
}

