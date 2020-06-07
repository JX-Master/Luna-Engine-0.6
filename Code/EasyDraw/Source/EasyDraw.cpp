// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file EasyDraw.cpp
* @author JXMaster
* @date 2019/10/5
*/
#include "EasyDraw.hpp"
#include "DrawPath.hpp"
#include "DrawList.hpp"
#include "DrawListRenderer.hpp"
#include "FontTexture.hpp"

namespace luna
{
	namespace edraw
	{
		using namespace gfx;
		P<IBlob> g_vs;
		P<IBlob> g_ps;
		P<IBlob> g_ps_no_tex;

		void deinit()
		{
			g_vs = nullptr;
			g_ps = nullptr;
			g_ps_no_tex = nullptr;
		}

		RV init()
		{
			lutry
			{
				const char vs_shader_code[] =
				"cbuffer vertexBuffer : register(b0) \
				{\
					float4x4 ProjectionMatrix; \
				};\
				struct VS_INPUT\
				{\
					float2 pos : POSITION;\
					float2 uv  : TEXCOORD0;\
					float4 col : COLOR0;\
				};\
				\
				struct PS_INPUT\
				{\
					float4 pos : SV_POSITION;\
					float4 col : COLOR0;\
					float2 uv  : TEXCOORD0;\
				};\
				\
				PS_INPUT main(VS_INPUT input)\
				{\
					PS_INPUT output;\
					output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
					output.col = input.col;\
					output.uv  = input.uv;\
					return output;\
				}";

				const char ps_shader_code[] =
					"struct PS_INPUT\
					{\
						float4 pos : SV_POSITION;\
						float4 col : COLOR0;\
						float2 uv  : TEXCOORD0;\
					};\
					SamplerState sampler0 : register(s0);\
					Texture2D texture0 : register(t0);\
					\
					float4 main(PS_INPUT input) : SV_Target\
					{\
						float4 out_col = clamp(input.col * texture0.Sample(sampler0, input.uv), float4(0.0f, 0.0f, 0.0f, 0.0f), float4(1.0f, 1.0f, 1.0f, 1.0f)); \
						return out_col; \
					}";

				const char ps_shader_code_no_tex[] =
					"struct PS_INPUT\
					{\
						float4 pos : SV_POSITION;\
						float4 col : COLOR0;\
						float2 uv  : TEXCOORD0;\
					};\
					\
					float4 main(PS_INPUT input) : SV_Target\
					{\
						float4 out_col = clamp(input.col, float4(0.0f, 0.0f, 0.0f, 0.0f), float4(1.0f, 1.0f, 1.0f, 1.0f)); \
						return out_col; \
					}";

				luset(g_vs, gfx::compile_shader(ShaderCompileDesc(vs_shader_code, sizeof(vs_shader_code), "EasyDrawVS", "main", EShaderSourceType::hlsl, EShaderTargetType::dx_bytecode,
					EShaderType::vertex, EShaderModel::sm_5_0, EShaderOptimizationLevel::speed, EShaderCompileFlag::debug_info)));
				luset(g_ps, gfx::compile_shader(ShaderCompileDesc(ps_shader_code, sizeof(ps_shader_code), "EasyDrawPS", "main", EShaderSourceType::hlsl, EShaderTargetType::dx_bytecode,
					EShaderType::pixel, EShaderModel::sm_5_0, EShaderOptimizationLevel::speed, EShaderCompileFlag::debug_info)));
				luset(g_ps_no_tex, gfx::compile_shader(ShaderCompileDesc(ps_shader_code_no_tex, sizeof(ps_shader_code_no_tex), "EasyDrawPSNoTex", "main", EShaderSourceType::hlsl, EShaderTargetType::dx_bytecode,
					EShaderType::pixel, EShaderModel::sm_5_0, EShaderOptimizationLevel::speed, EShaderCompileFlag::debug_info)));
				add_module("EasyDraw", deinit);
			}
			lucatchret;
			return s_ok;
		}

		P<IDrawList> new_draw_list()
		{
			P<DrawList> o = box_ptr(new_obj<DrawList>(get_module_allocator()));
			o->init();
			return o;
		}

		RP<IDrawListRenderer> new_draw_list_renderer(gfx::IRenderPass* render_pass)
		{
			luassert_usr(render_pass);
			P<DrawListRenderer> o = box_ptr(new_obj<DrawListRenderer>(get_module_allocator()));
			lutry
			{
				o->m_rp = render_pass;
				luexp(o->init());
			}
			lucatchret;
			return o;
		}

		P<IDrawPath> new_draw_path()
		{
			P<DrawPath> o = box_ptr(new_obj_aligned<DrawPath>(get_module_allocator()));
			return o;
		}

		RP<IFontTexture> new_font_texture(font::IFontAtlas* atlas, gfx::ICommandBuffer* command_buffer)
		{
			luassert_usr(atlas && command_buffer);
			P<FontTexture> o = box_ptr(new_obj<FontTexture>(get_module_allocator()));
			lutry
			{	
				luexp(o->reset(atlas, command_buffer));
			}
			lucatchret;
			return o;
		}
	}
}