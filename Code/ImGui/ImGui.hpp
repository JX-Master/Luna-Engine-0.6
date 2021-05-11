// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ImGui.hpp
* @author JXMaster
* @date 2020/4/5
*/
#pragma once
#include "IContext.hpp"

#ifndef LUNA_IMGUI_API
#define LUNA_IMGUI_API
#endif

namespace Luna
{
	namespace ImGui
	{
		//! Creates a new context.
		LUNA_IMGUI_API RP<IContext> new_context(Gfx::IGraphicDevice* render_device, Gfx::ICommandBuffer* init_cmds, Gfx::EResourceFormat rt_format = Gfx::EResourceFormat::rgba8_unorm, f32 dpi_scale = 1.0f);

		// IO APIs. IO handle is not thread safe.
		LUNA_IMGUI_API Float2 io_get_mouse_pos(h_io_t io);
		LUNA_IMGUI_API void io_set_mouse_pos(h_io_t io, const Float2& pos);
		LUNA_IMGUI_API bool io_is_mouse_down(h_io_t io, Input::EMouseKey key);
		LUNA_IMGUI_API void io_set_mouse_down(h_io_t io, Input::EMouseKey key, bool down);
		LUNA_IMGUI_API f32 io_get_mouse_wheel(h_io_t io);
		LUNA_IMGUI_API void io_set_mouse_wheel(h_io_t io, f32 wheel_delta);
		LUNA_IMGUI_API f32 io_get_mouse_wheel_h(h_io_t io);
		LUNA_IMGUI_API void io_set_mouse_wheel_h(h_io_t io, f32 wheel_delta);
		LUNA_IMGUI_API bool io_is_key_down(h_io_t io, Input::EKeyCode key);
		LUNA_IMGUI_API void io_set_key_down(h_io_t io, Input::EKeyCode key, bool down);
		LUNA_IMGUI_API void io_add_input_character(h_io_t io, char32_t c);
		LUNA_IMGUI_API void io_add_input_characters(h_io_t io, const char* str);
		LUNA_IMGUI_API void io_clear_input_characters(h_io_t io);

		LUNA_IMGUI_API bool io_want_capture_mouse(h_io_t io);
		LUNA_IMGUI_API bool io_want_capture_keyboard(h_io_t io);
		LUNA_IMGUI_API bool io_want_text_input(h_io_t io);
		LUNA_IMGUI_API bool io_want_set_mouse_pos(h_io_t io);

		// Draw List APIs. Draw List handle is not thread safe.

		LUNA_IMGUI_API void dl_push_clip_rect(h_draw_list_t draw_list, const Float2& clip_rect_min, const Float2& clip_rect_max, bool intersect_with_current_clip_rect = false);
		LUNA_IMGUI_API void dl_push_clip_rect_full_screen(h_draw_list_t draw_list);
		LUNA_IMGUI_API void dl_pop_clip_rect(h_draw_list_t draw_list);
		LUNA_IMGUI_API void dl_push_texture(h_draw_list_t draw_list, Gfx::IResource* texture);
		LUNA_IMGUI_API void dl_pop_texture(h_draw_list_t draw_list);
		LUNA_IMGUI_API Float2 dl_get_clip_rect_min(h_draw_list_t draw_list);
		LUNA_IMGUI_API Float2 dl_get_clip_rect_max(h_draw_list_t draw_list);
		// Primitives
		// - For rectangular primitives, "p_min" and "p_max" represent the upper-left and lower-right corners.
		// - For circle primitives, use "num_segments == 0" to automatically calculate tessellation (preferred). 
		//   In future versions we will use textures to provide cheaper and higher-quality circles. 
		//   Use AddNgon() and AddNgonFilled() functions if you need to guaranteed a specific number of sides.
		LUNA_IMGUI_API void dl_add_line(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, u32 col, f32 thickness = 1.0f);
		LUNA_IMGUI_API void dl_add_rect(h_draw_list_t draw_list, const Float2& p_min, const Float2& p_max, u32 col, f32 rounding = 0.0f, EDrawCornerFlag rounding_corners = EDrawCornerFlag::all, f32 thickness = 1.0f);
		LUNA_IMGUI_API void dl_add_rect_filled(h_draw_list_t draw_list, const Float2& p_min, const Float2& p_max, u32 col, f32 rounding = 0.0f, EDrawCornerFlag rounding_corners = EDrawCornerFlag::all);
		LUNA_IMGUI_API void dl_add_rect_filled_multi_color(h_draw_list_t draw_list, const Float2& p_min, const Float2& p_max, u32 col_upr_left, u32 col_upr_right, u32 col_bot_right, u32 col_bot_left);
		LUNA_IMGUI_API void dl_add_quad(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, u32 col, f32 thickness = 1.0f);
		LUNA_IMGUI_API void dl_add_quad_filled(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, u32 col);
		LUNA_IMGUI_API void dl_add_triangle(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, const Float2& p3, u32 col, f32 thickness = 1.0f);
		LUNA_IMGUI_API void dl_add_triangle_filled(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, const Float2& p3, u32 col);
		LUNA_IMGUI_API void dl_add_circle(h_draw_list_t draw_list, const Float2& center, f32 radius, u32 col, i32 num_segments = 12, f32 thickness = 1.0f);
		LUNA_IMGUI_API void dl_add_circle_filled(h_draw_list_t draw_list, const Float2& center, f32 radius, u32 col, i32 num_segments = 12);
		LUNA_IMGUI_API void dl_add_ngon(h_draw_list_t draw_list, const Float2& center, f32 radius, u32 col, i32 num_segments, f32 thickness = 1.0f);
		LUNA_IMGUI_API void dl_add_ngon_filled(h_draw_list_t draw_list, const Float2& center, f32 radius, u32 col, i32 num_segments);
		LUNA_IMGUI_API void dl_add_text(h_draw_list_t draw_list, const Float2& pos, u32 col, const char* text_begin, const char* text_end = nullptr);
		//LUNA_IMGUI_API void dl_add_text(h_draw_list_t draw_list, const ImFont* font, f32 font_size, const Float2& pos, u32 col, const char* text_begin, const char* text_end = nullptr, f32 wrap_width = 0.0f, const ImVec4* cpu_fine_clip_rect = nullptr);
		LUNA_IMGUI_API void dl_add_polyline(h_draw_list_t draw_list, const Float2U* points, i32 num_points, u32 col, bool closed, f32 thickness);
		LUNA_IMGUI_API void dl_add_convex_poly_filled(h_draw_list_t draw_list, const Float2U* points, i32 num_points, u32 col); // Note: Anti-aliased filling requires points to be in clockwise order.
		LUNA_IMGUI_API void dl_add_bezier_curve(h_draw_list_t draw_list, const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, u32 col, f32 thickness, i32 num_segments = 0);
		// Image primitives
		// - Read FAQ to understand what ImTextureID is.
		// - "p_min" and "p_max" represent the upper-left and lower-right corners of the rectangle.
		// - "uv_min" and "uv_max" represent the normalized texture coordinates to use for those corners. Using (0,0)->(1,1) texture coordinates will generally display the entire texture.
		LUNA_IMGUI_API void dl_add_image(h_draw_list_t draw_list, Gfx::IResource* tex, const Float2& p_min, const Float2& p_max, const Float2& uv_min = Float2(0, 0), const Float2& uv_max = Float2(1, 1), u32 col = 0xFFFFFFFF);
		LUNA_IMGUI_API void dl_add_image_quad(h_draw_list_t draw_list, Gfx::IResource* tex, const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, const Float2& uv1 = Float2(0, 0), const Float2& uv2 = Float2(1, 0), const Float2& uv3 = Float2(1, 1), const Float2& uv4 = Float2(0, 1), u32 col = 0xFFFFFFFF);
		LUNA_IMGUI_API void dl_add_image_rounded(h_draw_list_t draw_list, Gfx::IResource* tex, const Float2& p_min, const Float2& p_max, const Float2& uv_min, const Float2& uv_max, u32 col, f32 rounding, EDrawCornerFlag rounding_corners = EDrawCornerFlag::all);

		// Stateful path API, add points then finish with PathFillConvex() or PathStroke()
		LUNA_IMGUI_API void dl_path_clear(h_draw_list_t draw_list);
		LUNA_IMGUI_API void dl_path_line_to(h_draw_list_t draw_list, const Float2& pos);
		LUNA_IMGUI_API void dl_path_line_to_merge_duplicate(h_draw_list_t draw_list, const Float2& pos);
		LUNA_IMGUI_API void dl_path_fill_convex(h_draw_list_t draw_list, u32 col);
		LUNA_IMGUI_API void dl_path_stroke(h_draw_list_t draw_list, u32 col, bool closed, f32 thickness = 1.0f);
		LUNA_IMGUI_API void dl_path_arc_to(h_draw_list_t draw_list, const Float2& center, f32 radius, f32 a_min, f32 a_max, i32 num_segments = 10);
		LUNA_IMGUI_API void dl_path_arc_to_fast(h_draw_list_t draw_list, const Float2& center, f32 radius, i32 a_min_of_12, i32 a_max_of_12);
		LUNA_IMGUI_API void dl_path_bezier_curve_to(h_draw_list_t draw_list, const Float2& p2, const Float2& p3, const Float2& p4, i32 num_segments = 0);
		LUNA_IMGUI_API void dl_path_rect(h_draw_list_t draw_list, const Float2& rect_min, const Float2& rect_max, f32 rounding = 0.0f, EDrawCornerFlag rounding_corners = EDrawCornerFlag::all);
	}
}