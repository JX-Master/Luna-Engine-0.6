// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestImage.cpp
* @author JXMaster
* @date 2020/3/14
*/
#include "TestImage.hpp"
#include <Runtime/Math.hpp>

namespace Luna
{
	using namespace Gfx;

	struct VertexData
	{
		Float2U pos;
		Float2U texcoord;
	};

	RV TestImage::init()
	{
		lutry
		{
			// create render pass.
			RenderPassDesc render_pass({AttachmentDesc(EResourceFormat::rgba8_unorm, EAttachmentLoadOp::clear, EAttachmentStoreOp::store)},
			EResourceFormat::unknown, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care,1, false);
			luset(m_render_pass, Renderer::device()->new_render_pass(render_pass));

			// create fbo.
			luset(m_fbo, Renderer::device()->new_frame_buffer(m_render_pass, 1, g_rt.get_address_of(), nullptr, nullptr, nullptr));

			// create pso
			{
				const char vs_shader_code[] =
					"\
							struct VS_INPUT\
							{\
							float2 pos : POSITION;\
							float2 uv : TEXCOORD0;\
							};\
							\
							struct PS_INPUT\
							{\
							float4 pos : SV_POSITION;\
							float2 uv  : TEXCOORD0;\
							};\
							\
							PS_INPUT main(VS_INPUT input)\
							{\
							PS_INPUT output;\
							output.pos = float4(input.pos.x, input.pos.y, 0.0f, 1.0f);\
							output.uv  = input.uv;\
							return output;\
							}";

				ShaderCompileDesc sdesc(vs_shader_code, sizeof(vs_shader_code), "TestTriangleVS", "main",
					EShaderSourceType::hlsl, EShaderTargetType::platform_default, EShaderType::vertex,
					EShaderModel::sm_5_0, EShaderOptimizationLevel::full, EShaderCompileFlag::none);

				lulet(vs, compile_shader(sdesc));

				const char ps_shader_code[] =
					"struct PS_INPUT\
								{\
								float4 pos : SV_POSITION;\
								float2 uv  : TEXCOORD0;\
								};\
								SamplerState sampler0 : register(s0);\
								Texture2D texture0 : register(t0);\
								\
								float4 main(PS_INPUT input) : SV_Target\
								{\
								float4 out_col = clamp(texture0.Sample(sampler0, input.uv), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(1.0f, 1.0f, 1.0f, 1.0f)); \
								return out_col; \
								}";

				sdesc = ShaderCompileDesc(ps_shader_code, sizeof(ps_shader_code), "TestTrianglePS", "main",
					EShaderSourceType::hlsl, EShaderTargetType::platform_default, EShaderType::pixel,
					EShaderModel::sm_5_0, EShaderOptimizationLevel::full, EShaderCompileFlag::none);
				lulet(ps, compile_shader(sdesc));

				InputElementDesc input_elements[2] = {
					InputElementDesc("POSITION", 0, EResourceFormat::rg32_float),
					InputElementDesc("TEXCOORD", 0, EResourceFormat::rg32_float)
				};

				ShaderInputGroupDesc groups[2]{
					ShaderInputGroupDesc(EShaderInputGroupType::srv, 0, 1, EShaderVisibility::pixel),
					ShaderInputGroupDesc(EShaderInputGroupType::sampler, 0, 1, EShaderVisibility::pixel)
				};

				luset(m_shader_input_layout, Renderer::device()->new_shader_input_layout(ShaderInputLayoutDesc(groups, 2, EShaderInputLayoutFlag::allow_input_assembler_input_layout |
					EShaderInputLayoutFlag::deny_domain_shader_access | EShaderInputLayoutFlag::deny_geometry_shader_access |
					EShaderInputLayoutFlag::deny_hull_shader_access)));

				GraphicsPipelineStateDesc desc(
					InputLayoutDesc(2, input_elements),
					ShaderBytecode(vs.data(), vs.size()),
					ShaderBytecode(ps.data(), ps.size()),
					ShaderBytecode(Default()),
					ShaderBytecode(Default()),
					ShaderBytecode(Default()),
					StreamOutputDesc(Default()),
					BlendDesc(Default()),
					RasterizerDesc(Default()),
					DepthStencilDesc(Default()),
					EIndexBufferStripCutValue::disabled,
					EPrimitiveTopologyType::triangle,
					0xFFFFFFFF, 0);

				luset(m_pso, Renderer::device()->new_graphics_pipeline_state(m_shader_input_layout, m_render_pass, desc));

				luset(m_vb, Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::vertex_buffer, sizeof(VertexData) * 4)));
				u32 incides[] = { 0, 1, 2, 1, 3, 2 };
				luset(m_ib, Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::index_buffer, sizeof(incides))));
				lulet(mapped_data, m_ib->map_subresource(0, false, 1, 0));
				memcpy(mapped_data, incides, sizeof(incides));
				m_ib->unmap_subresource(0, false, 0, sizeof(incides));

				// prepare texture - 128x128 with only 1 mip level.
				luset(m_tex, Renderer::device()->new_resource(ResourceDesc::tex2d(EResourceFormat::rgba8_unorm, EAccessType::gpu_local, EResourceUsageFlag::shader_resource,
					128, 128, 1, 1)));

				lulet(tex_buffer, Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::none, sizeof(test_image_data_v))));
				lulet(tex_data_mapped, tex_buffer->map_subresource(0, false, 1, 0));
				memcpy(tex_data_mapped, test_image_data_v, sizeof(test_image_data_v));
				tex_buffer->unmap_subresource(0, true);

				TextureCopyLocation src, dest;
				src.resource = tex_buffer;
				src.type = ETextureCopyType::placed_footprint;
				src.placed_footprint.footprint.width = 128;
				src.placed_footprint.footprint.height = 128;
				src.placed_footprint.footprint.depth = 1;
				src.placed_footprint.footprint.row_pitch = 128 * 4;
				src.placed_footprint.footprint.format = EResourceFormat::rgba8_unorm;
				src.placed_footprint.offset = 0;
				dest.type = ETextureCopyType::subresource_index;
				dest.resource = m_tex;
				dest.subresource_index = 0;
				g_cb->resource_barrier(ResourceBarrierDesc::as_transition(m_tex, EResourceState::copy_dest, 0));
				g_cb->copy_texture_region(dest, 0, 0, 0, src);
				g_cb->submit();

				luset(m_view_set, Renderer::device()->new_view_set(m_shader_input_layout, ViewSetDesc(0, 1, 0, 1)));

				m_view_set->set_srv(0, m_tex, nullptr);

				f32 border_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
				m_view_set->set_sampler(0, SamplerDesc(EFilter::min_mag_mip_linear, ETextureAddressMode::clamp, 
					ETextureAddressMode::clamp, ETextureAddressMode::clamp, 0.0f, 1, EComparisonFunc::always, border_color, 0.0f, 0.0f));

				g_cb->wait();
				g_cb->reset();
			}
		}
		lucatchret;
		return RV();
	}
	void TestImage::update()
	{
		// prepare draw buffer. POSITION : TEXCOORD
		// 0----1
		// |    |
		// 2----3

		auto sz = g_window->size();
		auto w = sz.x;
		auto h = sz.y;

		VertexData data[4]{
			{ { -128.0f / w,  128.0f / h },{ 0.0f, 0.0f } },
			{ {  128.0f / w,  128.0f / h },{ 1.0f, 0.0f } },
			{ { -128.0f / w, -128.0f / h },{ 0.0f, 1.0f } },
			{ {  128.0f / w, -128.0f / h },{ 1.0f, 1.0f } },
		};

		void* mapped = m_vb->map_subresource(0, false, 1, 0).get();
		memcpy(mapped, data, sizeof(data));
		m_vb->unmap_subresource(0, false, 0, sizeof(data));

		Float4U clear_color = Color::black;
		g_cb->resource_barrier(ResourceBarrierDesc::as_transition(g_rt, EResourceState::render_target, 0));
		g_cb->begin_render_pass(m_render_pass, m_fbo, 1, &clear_color, 0.0f, 0);
		g_cb->set_pipeline_state(m_pso);
		g_cb->set_graphic_shader_input_layout(m_shader_input_layout);
		g_cb->set_graphic_view_set(m_view_set);
		g_cb->set_primitive_topology(EPrimitiveTopology::triangle_list);
		g_cb->set_vertex_buffers(0, 1, &VertexBufferViewDesc(m_vb, 0, sizeof(VertexData) * 4, sizeof(VertexData)));
		g_cb->set_index_buffer(m_ib, 0, sizeof(u32) * 6, EResourceFormat::r32_uint);
		sz = g_window->size();
		w = sz.x;
		h = sz.y;
		g_cb->set_scissor_rect(RectI(0, 0, (i32)w, (i32)h));
		g_cb->set_viewport(Viewport(0.0f, 0.0f, (f32)w, (f32)h, 0.0f, 1.0f));
		g_cb->draw_indexed(6, 0, 0);
		g_cb->end_render_pass();

		g_cb->submit();
		g_cb->wait();
	}
	void TestImage::resize()
	{
		m_fbo = Renderer::device()->new_frame_buffer(m_render_pass, 1, g_rt.get_address_of(), nullptr, nullptr, nullptr).get();
	}
}