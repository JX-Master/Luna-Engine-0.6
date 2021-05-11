// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestTriangle.cpp
* @author JXMaster
* @date 2020/3/14
*/
#include "TestTriangle.hpp"
#include <Runtime/Math.hpp>

namespace Luna
{
	using namespace Gfx;

	struct VertexData
	{
		Float2U pos;
		Float2U texc;
		Float4U color;
	};

	RV TestTriangle::init()
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
							float2 uv  : TEXCOORD;\
							float4 col : COLOR0;\
							};\
							\
							struct PS_INPUT\
							{\
							float4 pos : SV_POSITION;\
							float4 uv  : TEXCOORD0;\
							float4 col  : COLOR0;\
							};\
							\
							PS_INPUT main(VS_INPUT input)\
							{\
							PS_INPUT output;\
							output.pos = float4(input.pos.x, input.pos.y, 0.0f, 1.0f);\
							output.uv = float4(input.uv.x, input.uv.y, 0.0f, 1.0f);	\
							output.col  = input.col;\
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
								float4 uv  : TEXCOORD0;\
								float4 col : COLOR0;\
								};\
								\
								float4 main(PS_INPUT input) : SV_Target\
								{\
								if ((input.uv.x * input.uv.x - input.uv.y) > 0.0f)	\
								{	\
									discard;	\
								}	\
								return input.col; \
								}";

				sdesc = ShaderCompileDesc(ps_shader_code, sizeof(ps_shader_code), "TestTrianglePS", "main",
					EShaderSourceType::hlsl, EShaderTargetType::platform_default, EShaderType::pixel,
					EShaderModel::sm_5_0, EShaderOptimizationLevel::full, EShaderCompileFlag::none);
				lulet(ps, compile_shader(sdesc));

				InputElementDesc input_elements[3] = {
					InputElementDesc("POSITION", 0, EResourceFormat::rg32_float),
					InputElementDesc("TEXCOORD", 0, EResourceFormat::rg32_float),
					InputElementDesc("COLOR", 0, EResourceFormat::rgba32_float)
				};

				luset(m_shader_input_layout, Renderer::device()->new_shader_input_layout(ShaderInputLayoutDesc(nullptr, 0, EShaderInputLayoutFlag::allow_input_assembler_input_layout |
					EShaderInputLayoutFlag::deny_domain_shader_access | EShaderInputLayoutFlag::deny_geometry_shader_access |
					EShaderInputLayoutFlag::deny_hull_shader_access | EShaderInputLayoutFlag::deny_pixel_shader_access |
					EShaderInputLayoutFlag::deny_vertex_shader_access)));

				GraphicsPipelineStateDesc desc(
					InputLayoutDesc(3, input_elements),
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

				// prepare draw buffer. POSITION : COLOR
				VertexData data[3]{
					{ { 0.0f,  0.7f},{0.0f, 0.0f},{1.0f, 0.0f, 0.0f, 1.0f} },
					{ { 0.7f, -0.7f},{0.5f, 0.0f},{0.0f, 1.0f, 0.0f, 1.0f} },
					{ {-0.7f, -0.7f},{1.0f, 1.0f},{0.0f, 0.0f, 1.0f, 0.0f} }
				};

				luset(m_vb, Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::vertex_buffer, sizeof(data))));
				lulet(mapped_data, m_vb->map_subresource(0, false, 1, 0));
				memcpy(mapped_data, data, sizeof(data));
				m_vb->unmap_subresource(0, false, 0, sizeof(data));
			}
		}
		lucatchret;
		return RV();
	}
	void TestTriangle::update()
	{
		Float4U clear_color = Color::black;
		g_cb->resource_barrier(ResourceBarrierDesc::as_transition(g_rt, EResourceState::render_target, 0));
		g_cb->begin_render_pass(m_render_pass, m_fbo, 1, &clear_color, 0.0f, 0);
		g_cb->set_pipeline_state(m_pso);
		g_cb->set_graphic_shader_input_layout(m_shader_input_layout);
		g_cb->set_primitive_topology(EPrimitiveTopology::triangle_list);
		g_cb->set_vertex_buffers(0, 1, &VertexBufferViewDesc(m_vb, 0, sizeof(VertexData) * 3, sizeof(VertexData)));
		auto sz = g_window->size();
		g_cb->set_scissor_rect(RectI(0, 0, (i32)sz.x, (i32)sz.y));
		g_cb->set_viewport(Viewport(0.0f, 0.0f, (f32)sz.x, (f32)sz.y, 0.0f, 1.0f));
		g_cb->draw(3, 0);
		g_cb->end_render_pass();

		g_cb->submit();
		g_cb->wait();
	}
	void TestTriangle::resize()
	{
		m_fbo = Renderer::device()->new_frame_buffer(m_render_pass, 1, g_rt.get_address_of(), nullptr, nullptr, nullptr).get();
	}
}