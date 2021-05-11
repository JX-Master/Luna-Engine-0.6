// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ImGuiInstance.cpp
* @author JXMaster
* @date 2020/4/8
*/
#include "ImGuiInstance.hpp"
#include "Context.hpp"
#include <Runtime/Module.hpp>

namespace Luna
{
	namespace ImGui
	{
		P<IMutex> m_mtx;
		P<IMutex> m_gizmo_mtx;
		Blob m_vs_blob;
		Blob m_ps_blob;

		void deinit()
		{
			m_ps_blob.resize(0);
			m_vs_blob.resize(0);
			m_mtx = nullptr;
			m_gizmo_mtx = nullptr;
		}

		RV init()
		{
			lutry
			{
				m_mtx = new_mutex();
				m_gizmo_mtx = new_mutex();
				static const char* vertexShader =
					"cbuffer vertexBuffer : register(b0) \
					{\
					  float4x4 ProjectionMatrix; \
					};\
					struct VS_INPUT\
					{\
					  float2 pos : POSITION;\
					  float4 col : COLOR0;\
					  float2 uv  : TEXCOORD0;\
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
				luset(m_vs_blob, Gfx::compile_shader(Gfx::ShaderCompileDesc(vertexShader, strlen(vertexShader),
					"ImGuiVS", "main", Gfx::EShaderSourceType::hlsl, Gfx::EShaderTargetType::dx_bytecode,
					Gfx::EShaderType::vertex, Gfx::EShaderModel::sm_5_0, Gfx::EShaderOptimizationLevel::full, 
					Gfx::EShaderCompileFlag::none)));
				static const char* pixelShader =
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
					  float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
					  return out_col; \
					}";
				luset(m_ps_blob, Gfx::compile_shader(Gfx::ShaderCompileDesc(pixelShader, strlen(pixelShader),
					"ImGuiPS", "main", Gfx::EShaderSourceType::hlsl, Gfx::EShaderTargetType::dx_bytecode,
					Gfx::EShaderType::pixel, Gfx::EShaderModel::sm_5_0, Gfx::EShaderOptimizationLevel::full,
					Gfx::EShaderCompileFlag::none)));
			}
			lucatchret;
			return RV();
		}

		StaticRegisterModule m(u8"ImGui", u8"Core;Font;Gfx;Input", init, deinit);

		LUNA_IMGUI_API RP<IContext> new_context(Gfx::IGraphicDevice* render_device, Gfx::ICommandBuffer* init_cmds, Gfx::EResourceFormat rt_format, f32 dpi_scale)
		{
			P<Context> ctx = newobj<Context>();
			lutry
			{
				luexp(ctx->init(render_device, init_cmds, rt_format, dpi_scale));
			}
			lucatchret;
			return ctx;
		}

		LUNA_IMGUI_API Float2 io_get_mouse_pos(h_io_t io)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			return Float2(o->MousePos.x, o->MousePos.y);
		}
		LUNA_IMGUI_API void io_set_mouse_pos(h_io_t io, const Float2& pos)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			o->MousePos = pos;
		}
		LUNA_IMGUI_API bool io_is_mouse_down(h_io_t io, Input::EMouseKey key)
		{
			ImGuiMouseButton btn = 0;
			switch (key)
			{
			case Input::EMouseKey::lb:
				btn = ImGuiMouseButton_Left;
				break;
			case Input::EMouseKey::rb:
				btn = ImGuiMouseButton_Right;
				break;
			case Input::EMouseKey::mb:
				btn = ImGuiMouseButton_Middle;
				break;
			case Input::EMouseKey::x1b:
				btn = 3;
				break;
			case Input::EMouseKey::x2b:
				btn = 4;
				break;
			default:
				break;
			}
			ImGuiIO* o = (ImGuiIO*)io;
			return o->MouseDown[btn];
		}
		LUNA_IMGUI_API void io_set_mouse_down(h_io_t io, Input::EMouseKey key, bool down)
		{
			ImGuiMouseButton btn = 0;
			switch (key)
			{
			case Input::EMouseKey::lb:
				btn = ImGuiMouseButton_Left;
				break;
			case Input::EMouseKey::rb:
				btn = ImGuiMouseButton_Right;
				break;
			case Input::EMouseKey::mb:
				btn = ImGuiMouseButton_Middle;
				break;
			case Input::EMouseKey::x1b:
				btn = 3;
				break;
			case Input::EMouseKey::x2b:
				btn = 4;
				break;
			default:
				break;
			}
			ImGuiIO* o = (ImGuiIO*)io;
			o->MouseDown[btn] = down;
		}
		LUNA_IMGUI_API f32 io_get_mouse_wheel(h_io_t io)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			return o->MouseWheel;
		}
		LUNA_IMGUI_API void io_set_mouse_wheel(h_io_t io, f32 wheel_delta)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			o->MouseWheel = wheel_delta;
		}
		LUNA_IMGUI_API f32 io_get_mouse_wheel_h(h_io_t io)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			return o->MouseWheelH;
		}
		LUNA_IMGUI_API void io_set_mouse_wheel_h(h_io_t io, f32 wheel_delta)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			o->MouseWheelH = wheel_delta;
		}
		LUNA_IMGUI_API bool io_is_key_down(h_io_t io, Input::EKeyCode key)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			return o->KeysDown[(u32)key];
		}
		LUNA_IMGUI_API void io_set_key_down(h_io_t io, Input::EKeyCode key, bool down)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			o->KeysDown[(u32)key] = down;
		}
		LUNA_IMGUI_API void io_add_input_character(h_io_t io, char32_t c)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			o->AddInputCharacter(c);
		}
		LUNA_IMGUI_API void io_add_input_characters(h_io_t io, const char* str)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			o->AddInputCharactersUTF8(str);
		}
		LUNA_IMGUI_API void io_clear_input_characters(h_io_t io)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			o->ClearInputCharacters();
		}

		LUNA_IMGUI_API bool io_want_capture_mouse(h_io_t io)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			return o->WantCaptureMouse;
		}
		LUNA_IMGUI_API bool io_want_capture_keyboard(h_io_t io)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			return o->WantCaptureKeyboard;
		}
		LUNA_IMGUI_API bool io_want_text_input(h_io_t io)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			return o->WantTextInput;
		}
		LUNA_IMGUI_API bool io_want_set_mouse_pos(h_io_t io)
		{
			ImGuiIO* o = (ImGuiIO*)io;
			return o->WantSetMousePos;
		}

		LUNA_IMGUI_API void dl_push_clip_rect(h_draw_list_t draw_list, const Float2& clip_rect_min, const Float2& clip_rect_max, bool intersect_with_current_clip_rect)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
		}
		LUNA_IMGUI_API void dl_push_clip_rect_full_screen(h_draw_list_t draw_list)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PushClipRectFullScreen();
		}
		LUNA_IMGUI_API void dl_pop_clip_rect(h_draw_list_t draw_list)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PopClipRect();
		}
		LUNA_IMGUI_API void dl_push_texture(h_draw_list_t draw_list, Gfx::IResource* texture)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PushTextureID(texture);
		}
		LUNA_IMGUI_API void dl_pop_texture(h_draw_list_t draw_list)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PopTextureID();
		}
		LUNA_IMGUI_API Float2 dl_get_clip_rect_min(h_draw_list_t draw_list)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			auto r = dl->GetClipRectMin();
			return Float2(r.x, r.y);
		}
		LUNA_IMGUI_API Float2 dl_get_clip_rect_max(h_draw_list_t draw_list)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			auto r = dl->GetClipRectMax();
			return Float2(r.x, r.y);
		}
		LUNA_IMGUI_API void dl_add_line(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, u32 col, f32 thickness)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddLine(p1, p2, col, thickness);
		}
		LUNA_IMGUI_API void dl_add_rect(h_draw_list_t draw_list, const Float2& p_min, const Float2& p_max, u32 col, f32 rounding, EDrawCornerFlag rounding_corners, f32 thickness)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddRect(p_min, p_max, col, rounding, (u32)rounding_corners, thickness);
		}
		LUNA_IMGUI_API void dl_add_rect_filled(h_draw_list_t draw_list, const Float2& p_min, const Float2& p_max, u32 col, f32 rounding, EDrawCornerFlag rounding_corners)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddRectFilled(p_min, p_max, col, rounding, (u32)rounding_corners);
		}
		LUNA_IMGUI_API void dl_add_rect_filled_multi_color(h_draw_list_t draw_list, const Float2& p_min, const Float2& p_max, u32 col_upr_left, u32 col_upr_right, u32 col_bot_right, u32 col_bot_left)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddRectFilledMultiColor(p_min, p_max, col_upr_left, col_upr_right, col_bot_right, col_bot_left);
		}
		LUNA_IMGUI_API void dl_add_quad(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, u32 col, f32 thickness)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddQuad(p1, p2, p3, p4, col, thickness);
		}
		LUNA_IMGUI_API void dl_add_quad_filled(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, u32 col)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddQuadFilled(p1, p2, p3, p4, col);
		}
		LUNA_IMGUI_API void dl_add_triangle(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, const Float2& p3, u32 col, f32 thickness)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddTriangle(p1, p2, p3, col, thickness);
		}
		LUNA_IMGUI_API void dl_add_triangle_filled(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, const Float2& p3, u32 col)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddTriangleFilled(p1, p2, p3, col);
		}
		LUNA_IMGUI_API void dl_add_circle(h_draw_list_t draw_list, const Float2& center, f32 radius, u32 col, i32 num_segments, f32 thickness)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddCircle(center, radius, col, num_segments, thickness);
		}
		LUNA_IMGUI_API void dl_add_circle_filled(h_draw_list_t draw_list, const Float2& center, f32 radius, u32 col, i32 num_segments)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddCircleFilled(center, radius, col, num_segments);
		}
		LUNA_IMGUI_API void dl_add_ngon(h_draw_list_t draw_list, const Float2& center, f32 radius, u32 col, i32 num_segments, f32 thickness)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddNgon(center, radius, col, num_segments, thickness);
		}
		LUNA_IMGUI_API void dl_add_ngon_filled(h_draw_list_t draw_list, const Float2& center, f32 radius, u32 col, i32 num_segments)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddNgonFilled(center, radius, col, num_segments);
		}
		LUNA_IMGUI_API void dl_add_text(h_draw_list_t draw_list, const Float2& pos, u32 col, const char* text_begin, const char* text_end)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddText(pos, col, text_begin, text_end);
		}
		LUNA_IMGUI_API void dl_add_polyline(h_draw_list_t draw_list, const Float2U* points, i32 num_points, u32 col, bool closed, f32 thickness)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddPolyline((ImVec2*)points, num_points, col, closed, thickness);
		}
		LUNA_IMGUI_API void dl_add_convex_poly_filled(h_draw_list_t draw_list, const Float2U* points, i32 num_points, u32 col)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddConvexPolyFilled((ImVec2*)points, num_points, col);
		}
		LUNA_IMGUI_API void dl_add_bezier_curve(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, u32 col, f32 thickness, i32 num_segments)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddBezierCurve(p1, p2, p3, p4, col, thickness, num_segments);
		}
		LUNA_IMGUI_API void dl_add_image(h_draw_list_t draw_list, Gfx::IResource* tex, const Float2& p_min, const Float2& p_max, const Float2& uv_min, const Float2& uv_max, u32 col)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddImage(tex, p_min, p_max, uv_min, uv_max, col);
		}
		LUNA_IMGUI_API void dl_add_image_quad(h_draw_list_t draw_list, Gfx::IResource* tex, const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, const Float2& uv1, const Float2& uv2, const Float2& uv3, const Float2& uv4, u32 col)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddImageQuad(tex, p1, p2, p3, p4, uv1, uv2, uv3, uv4, col);
		}
		LUNA_IMGUI_API void dl_add_image_rounded(h_draw_list_t draw_list, Gfx::IResource* tex, const Float2& p_min, const Float2& p_max, const Float2& uv_min, const Float2& uv_max, u32 col, f32 rounding, EDrawCornerFlag rounding_corners)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->AddImageRounded(tex, p_min, p_max, uv_min, uv_max, col, rounding, (u32)rounding_corners);
		}

		LUNA_IMGUI_API void dl_path_clear(h_draw_list_t draw_list)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PathClear();
		}
		LUNA_IMGUI_API void dl_path_line_to(h_draw_list_t draw_list, const Float2& pos)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PathLineTo(pos);
		}
		LUNA_IMGUI_API void dl_path_line_to_merge_duplicate(h_draw_list_t draw_list, const Float2& pos)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PathLineToMergeDuplicate(pos);
		}
		LUNA_IMGUI_API void dl_path_fill_convex(h_draw_list_t draw_list, u32 col)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PathFillConvex(col);
		}
		LUNA_IMGUI_API void dl_path_stroke(h_draw_list_t draw_list, u32 col, bool closed, f32 thickness)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PathStroke(col, closed, thickness);
		}
		LUNA_IMGUI_API void dl_path_arc_to(h_draw_list_t draw_list, const Float2& center, f32 radius, f32 a_min, f32 a_max, i32 num_segments)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PathArcTo(center, radius, a_min, a_max, num_segments);
		}
		LUNA_IMGUI_API void dl_path_arc_to_fast(h_draw_list_t draw_list, const Float2& center, f32 radius, i32 a_min_of_12, i32 a_max_of_12)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PathArcToFast(center, radius, a_min_of_12, a_max_of_12);
		}
		LUNA_IMGUI_API void dl_path_bezier_curve_to(h_draw_list_t draw_list, const Float2& p2, const Float2& p3, const Float2& p4, i32 num_segments)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PathBezierCurveTo(p2, p3, p4, num_segments);
		}
		LUNA_IMGUI_API void dl_path_rect(h_draw_list_t draw_list, const Float2& rect_min, const Float2& rect_max, f32 rounding, EDrawCornerFlag rounding_corners)
		{
			ImDrawList* dl = (ImDrawList*)draw_list;
			dl->PathRect(rect_min, rect_max, rounding, (u32)rounding_corners);
		}
	}
}