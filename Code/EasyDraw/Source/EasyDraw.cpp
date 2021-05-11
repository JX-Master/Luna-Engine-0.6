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
#include <Runtime/Module.hpp>

namespace Luna
{
	namespace EasyDraw
	{
		using namespace Gfx;
		Blob g_vs;
		Blob g_ps;
		Blob g_ps_no_tex;

		void deinit()
		{
			g_vs.resize(0);
			g_ps.resize(0);
			g_ps_no_tex.resize(0);
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

				luset(g_vs, Gfx::compile_shader(ShaderCompileDesc(vs_shader_code, sizeof(vs_shader_code), "EasyDrawVS", "main", EShaderSourceType::hlsl, EShaderTargetType::dx_bytecode,
					EShaderType::vertex, EShaderModel::sm_5_0, EShaderOptimizationLevel::speed, EShaderCompileFlag::debug_info)));
				luset(g_ps, Gfx::compile_shader(ShaderCompileDesc(ps_shader_code, sizeof(ps_shader_code), "EasyDrawPS", "main", EShaderSourceType::hlsl, EShaderTargetType::dx_bytecode,
					EShaderType::pixel, EShaderModel::sm_5_0, EShaderOptimizationLevel::speed, EShaderCompileFlag::debug_info)));
				luset(g_ps_no_tex, Gfx::compile_shader(ShaderCompileDesc(ps_shader_code_no_tex, sizeof(ps_shader_code_no_tex), "EasyDrawPSNoTex", "main", EShaderSourceType::hlsl, EShaderTargetType::dx_bytecode,
					EShaderType::pixel, EShaderModel::sm_5_0, EShaderOptimizationLevel::speed, EShaderCompileFlag::debug_info)));
			}
			lucatchret;
			return RV();
		}

		StaticRegisterModule m("EasyDraw", "Core;Font;Gfx;Image;Input;RectPack", init, deinit);

		P<IDrawList> new_draw_list()
		{
			P<DrawList> o = newobj<DrawList>();
			o->init();
			return o;
		}

		RP<IDrawListRenderer> new_draw_list_renderer(Gfx::IRenderPass* render_pass)
		{
			lucheck(render_pass);
			P<DrawListRenderer> o = newobj<DrawListRenderer>();
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
			P<DrawPath> o = newobj<DrawPath>();
			return o;
		}

		RP<IFontTexture> new_font_texture(Font::IFontAtlas* atlas, Gfx::ICommandBuffer* command_buffer)
		{
			lucheck(atlas && command_buffer);
			P<FontTexture> o = newobj<FontTexture>();
			lutry
			{	
				luexp(o->reset(atlas, command_buffer));
			}
			lucatchret;
			return o;
		}
	}
}