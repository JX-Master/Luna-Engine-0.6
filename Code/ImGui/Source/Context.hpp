// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Context.hpp
* @author JXMaster
* @date 2020/4/5
*/
#pragma once
#include "ImGuiInstance.hpp"
#include <Input/Input.hpp>
namespace Luna
{
	namespace ImGui
	{
		static_assert(sizeof(InputTextCallbackData) == sizeof(ImGuiInputTextCallbackData), "InputTextCallbackData mapped failed.");
		static_assert(sizeof(Style) == sizeof(ImGuiStyle), "Style mapping failed.");

		class Context;

		class ImGuiWindowEventListener : public Gfx::IWindowEventListener
		{
		public:
			lucid("{4435eb0d-f6a8-41dc-983b-3e15ba3007bb}");
			luiimpl(ImGuiWindowEventListener, Gfx::IWindowEventListener, IObject);

			Context* m_ctx;
			WP<Gfx::IWindow> m_window;

			ImGuiWindowEventListener() {}

			virtual bool on_window_event(Gfx::IWindow* window, Gfx::EWindowEvent e, void* data) override;
		};

		class Context : public IContext
		{
		public:
			lucid("{ed4bc5cb-fc95-4591-94e1-8680792228d9}");
			luiimpl(Context, IContext, IObject);

			ImGuiContext* m_ctx;

			P<ImGuiWindowEventListener> m_listener;

			// Render context.
			P<Gfx::IGraphicDevice> m_dev;
			P<Gfx::IResource> m_font;
			P<Gfx::IResource> m_vb;
			P<Gfx::IResource> m_ib;
			P<Gfx::IResource> m_cb;

			P<Gfx::IRenderPass> m_rp;
			P<Gfx::IShaderInputLayout> m_layout;
			P<Gfx::IPipelineState> m_pso;

			usize m_vb_size;
			usize m_ib_size;

			Context() :
				m_vb_size(0),
				m_ib_size(0) {}

			RV init(Gfx::IGraphicDevice* render_device, Gfx::ICommandBuffer* init_cmds, Gfx::EResourceFormat rt_format, f32 dpi_scale);

			virtual RV attach_system_window(Gfx::IWindow* window) override;
			virtual void detach_system_window() override
			{
				m_listener = nullptr;
			}

			virtual h_io_t get_io() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return (h_io_t)(&::ImGui::GetIO());
			}
			virtual Style* get_style() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return (Style*)(&::ImGui::GetStyle());
			}

			virtual void new_frame() override;
			virtual void end_frame() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Render();
			}

			virtual RV render(Gfx::ICommandBuffer* cmd_buffer, Gfx::IResource* render_target) override;

			virtual void show_demo_window(bool* p_open = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::ShowDemoWindow(p_open);
			}
			virtual void show_about_window(bool* p_open = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::ShowAboutWindow(p_open);
			}
			virtual void show_metrics_window(bool* p_open = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::ShowMetricsWindow(p_open);
			}
			virtual void show_style_editor() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::ShowStyleEditor();
			}
			virtual bool show_style_selector(const char* label) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ShowStyleSelector(label);
			}
			virtual void show_font_selector(const char* label) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::ShowFontSelector(label);
			}
			virtual void show_user_guide() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::ShowUserGuide();
			}
			virtual const char*   get_version() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetVersion();
			}

			virtual void style_colors_dark() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::StyleColorsDark();
			}
			virtual void style_colors_classic() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::StyleColorsClassic();
			}
			virtual void style_colors_light() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::StyleColorsLight();
			}
															 
			virtual bool begin(const char* name, bool* open = nullptr, EWindowFlag flags = EWindowFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::Begin(name, open, (u32)flags);
			}
			virtual void end() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::End();
			}

			virtual bool begin_child(const char* str_id, const Float2& size = Float2(0.0f, 0.0f), bool border = false, EWindowFlag flags = EWindowFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginChild(str_id, size, border, (u32)flags);
			}
			virtual bool begin_child(i32 id, const Float2& size = Float2(0.0f, 0.0f), bool border = false, EWindowFlag flags = EWindowFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginChild(id, size, border, (u32)flags);
			}
			virtual void end_child() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndChild();
			}

			virtual bool is_window_appearing() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsWindowAppearing();
			}
			virtual bool is_window_collapsed() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsWindowCollapsed();
			}
			virtual bool is_window_focused(EFocusedFlag flags = EFocusedFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsWindowFocused((u32)flags);
			}
			virtual bool is_window_hovered(EHoveredFlag flags = EHoveredFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsWindowHovered((u32)flags);
			}
			virtual h_draw_list_t get_window_draw_list() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetWindowDrawList();
			}
			virtual Float2 get_window_pos() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto ret = ::ImGui::GetWindowPos();
				return Float2(ret.x, ret.y);
			}
			virtual Float2 get_window_size() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto ret = ::ImGui::GetWindowSize();
				return Float2(ret.x, ret.y);
			}
			virtual f32 get_window_width() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetWindowWidth();
			}
			virtual f32 get_window_height() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetWindowHeight();
			}

			virtual void set_next_window_pos(const Float2& pos, ECondition cond = ECondition::none, const Float2& pivot = Float2(0, 0)) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetNextWindowPos(pos, (u32)cond, pivot);
			}
			virtual void set_next_window_size(const Float2& size, ECondition cond = ECondition::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetNextWindowSize(size, (u32)cond);
			}
			virtual void set_next_window_size_constraints(const Float2& size_min, const Float2& size_max) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetNextWindowSizeConstraints(size_min, size_max);
			}
			virtual void set_next_window_content_size(const Float2& size) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetNextWindowContentSize(size);
			}
			virtual void set_next_window_collapsed(bool collapsed, ECondition cond = ECondition::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetNextWindowCollapsed(collapsed, (u32)cond);
			}
			virtual void set_next_window_focus() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetNextWindowFocus();
			}
			virtual void set_next_window_bg_alpha(f32 alpha) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetNextWindowBgAlpha(alpha);
			}
			virtual void set_window_pos(const Float2& pos, ECondition cond = ECondition::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetWindowPos(pos, (u32)cond);
			}
			virtual void set_window_size(const Float2& size, ECondition cond = ECondition::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetWindowSize(size, (u32)cond);
			}
			virtual void set_window_collapsed(bool collapsed, ECondition cond = ECondition::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetWindowCollapsed(collapsed, (u32)cond);
			}
			virtual void set_window_focus() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetWindowFocus();
			}
			virtual void set_window_font_scale(f32 scale) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetWindowFontScale(scale);
			}
			virtual void set_window_pos(const char* name, const Float2& pos, ECondition cond = ECondition::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetWindowPos(name, pos, (u32)cond);
			}
			virtual void set_window_size(const char* name, const Float2& size, ECondition cond = ECondition::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetWindowSize(name, size, (u32)cond);
			}
			virtual void set_window_collapsed(const char* name, bool collapsed, ECondition cond = ECondition::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetWindowCollapsed(name, collapsed, (u32)cond);
			}
			virtual void set_window_focus(const char* name) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetWindowFocus(name);
			}
			
			virtual Float2 get_content_region_max() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetContentRegionMax();
				return Float2(r.x, r.y);
			}
			virtual Float2 get_content_region_avail() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetContentRegionAvail();
				return Float2(r.x, r.y);
			}
			virtual Float2 get_window_content_region_min() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetWindowContentRegionMin();
				return Float2(r.x, r.y);
			}
			virtual Float2 get_window_content_region_max() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetWindowContentRegionMax();
				return Float2(r.x, r.y);
			}
			virtual f32 get_window_content_region_width() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetWindowContentRegionWidth();
			}

			virtual f32 get_scroll_x() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetScrollX();
			}
			virtual f32 get_scroll_y() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetScrollY();
			}
			virtual f32 get_scroll_max_x() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetScrollMaxX();
			}
			virtual f32 get_scroll_max_y() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetScrollMaxY();
			}
			virtual void set_scroll_x(f32 scroll_x) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetScrollX(scroll_x);
			}
			virtual void set_scroll_y(f32 scroll_y) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetScrollY(scroll_y);
			}
			virtual void set_scroll_here_x(f32 center_x_ratio = 0.5f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetScrollHereX(center_x_ratio);
			}
			virtual void set_scroll_here_y(f32 center_y_ratio = 0.5f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetScrollHereY(center_y_ratio);
			}
			virtual void set_scroll_from_pos_x(f32 local_x, f32 center_x_ratio = 0.5f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetScrollFromPosX(local_x, center_x_ratio);
			}
			virtual void set_scroll_from_pos_y(f32 local_y, f32 center_y_ratio = 0.5f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetScrollFromPosY(local_y, center_y_ratio);
			}

			virtual void pop_font() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PopFont();
			}
			virtual void push_style_color(EColor idx, u32 col) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushStyleColor((u32)idx, col);
			}
			virtual void push_style_color(EColor idx, const Float4& col) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushStyleColor((u32)idx, col);
			}
			virtual void pop_style_color(i32 count = 1) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PopStyleColor(count);
			}
			virtual void push_style_var(EStyle idx, f32 val) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushStyleVar((u32)idx, val);
			}
			virtual void push_style_var(EStyle idx, const Float2& val) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushStyleVar((u32)idx, val);
			}
			virtual void pop_style_var(i32 count = 1) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PopStyleVar(count);
			}
			virtual Float4 get_style_color_vec4(EColor idx) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetStyleColorVec4((u32)idx);
				return Float4(r.x, r.y, r.z, r.w);
			}
			virtual f32 get_font_size() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetFontSize();
			}
			virtual Float2 get_font_tex_uv_white_pixel() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetFontTexUvWhitePixel();
				return Float2(r.x, r.y);
			}
			virtual u32 get_color_u32(EColor idx, f32 alpha_mul = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetColorU32((u32)idx, alpha_mul);
			}
			virtual u32 get_color_u32(const Float4& col) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetColorU32(col);
			}
			virtual u32 get_color_u32(u32 col) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetColorU32(col);
			}

			virtual void push_item_width(f32 item_width) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushItemWidth(item_width);
			}
			virtual void pop_item_width() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PopItemWidth();
			}
			virtual void set_next_item_width(f32 item_width) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetNextItemWidth(item_width);
			}
			virtual f32 calc_item_width() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::CalcItemWidth();
			}
			virtual void push_text_wrap_pos(f32 wrap_local_pos_x = 0.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushTextWrapPos(wrap_local_pos_x);
			}
			virtual void pop_text_wrap_pos() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PopTextWrapPos();
			}
			virtual void push_allow_keyboard_focus(bool allow_keyboard_focus) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushAllowKeyboardFocus(allow_keyboard_focus);
			}
			virtual void pop_allow_keyboard_focus() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PopAllowKeyboardFocus();
			}
			virtual void push_button_repeat(bool repeat) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushButtonRepeat(repeat);
			}
			virtual void pop_button_repeat() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PopButtonRepeat();
			}

			virtual void separator() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Separator();
			}
			virtual void same_line(f32 offset_from_start_x = 0.0f, f32 spacing = -1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SameLine(offset_from_start_x, spacing);
			}
			virtual void new_line() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::NewLine();
			}
			virtual void spacing() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Spacing();
			}
			virtual void dummy(const Float2& size) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Dummy(size);
			}
			virtual void indent(f32 indent_w = 0.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Indent(indent_w);
			}
			virtual void unindent(f32 indent_w = 0.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Unindent(indent_w);
			}
			virtual void begin_group() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::BeginGroup();
			}
			virtual void end_group() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndGroup();
			}
			virtual Float2 get_cursor_pos() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetCursorPos();
				return Float2(r.x, r.y);
			}
			virtual f32 get_cursor_pos_x() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetCursorPosX();
			}
			virtual f32 get_cursor_pos_y() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetCursorPosY();
			}
			virtual void set_cursor_pos(const Float2& local_pos) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetCursorPos(local_pos);
			}
			virtual void set_cursor_pos_x(f32 local_x) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetCursorPosX(local_x);
			}
			virtual void set_cursor_pos_y(f32 local_y) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetCursorPosY(local_y);
			}
			virtual Float2 get_cursor_start_pos() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetCursorStartPos();
				return Float2(r.x, r.y);
			}
			virtual Float2 get_cursor_screen_pos() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetCursorScreenPos();
				return Float2(r.x, r.y);
			}
			virtual void set_cursor_screen_pos(const Float2& pos) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetCursorScreenPos(pos);
			}
			virtual void align_text_to_frame_padding() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::AlignTextToFramePadding();
			}
			virtual f32 get_text_line_height() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetTextLineHeight();
			}
			virtual f32 get_text_line_height_with_spacing() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetTextLineHeightWithSpacing();
			}
			virtual f32 get_frame_height() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetFrameHeight();
			}
			virtual f32 get_frame_height_with_spacing() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetFrameHeightWithSpacing();
			}
			
			virtual void push_id(const char* str_id) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushID(str_id);
			}
			virtual void push_id(const char* str_id_begin, const char* str_id_end) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushID(str_id_begin, str_id_end);
			}
			virtual void push_id(const void* ptr_id) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushID(ptr_id);
			}
			virtual void push_id(i32 int_id) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushID(int_id);
			}
			virtual void pop_id() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PopID();
			}
			virtual u32 get_id(const char* str_id) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetID(str_id);
			}
			virtual u32 get_id(const char* str_id_begin, const char* str_id_end) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetID(str_id_begin, str_id_end);
			}
			virtual u32 get_id(const void* ptr_id) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetID(ptr_id);
			}

			virtual void text_unformatted(const char* text, const char* text_end = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::TextUnformatted(text, text_end);
			}
			virtual void text(const char* fmt, ...) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				va_list args;
				va_start(args, fmt);
				text_v(fmt, args);
				va_end(args);
			}
			virtual void text_v(const char* fmt, va_list args) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::TextV(fmt, args);
			}
			virtual void text_colored(const Float4& col, const char* fmt, ...) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				va_list args;
				va_start(args, fmt);
				text_colored_v(col, fmt, args);
				va_end(args);
			}
			virtual void text_colored_v(const Float4& col, const char* fmt, va_list args) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::TextColoredV(col, fmt, args);
			}
			virtual void text_disabled(const char* fmt, ...) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				va_list args;
				va_start(args, fmt);
				text_disabled_v(fmt, args);
				va_end(args);
			}
			virtual void text_disabled_v(const char* fmt, va_list args) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::TextDisabledV(fmt, args);
			}
			virtual void text_wrapped(const char* fmt, ...) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				va_list args;
				va_start(args, fmt);
				text_wrapped_v(fmt, args);
				va_end(args);
			}
			virtual void text_wrapped_v(const char* fmt, va_list args) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::TextWrappedV(fmt, args);
			}
			virtual void label_text(const char* label, const char* fmt, ...) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				va_list args;
				va_start(args, fmt);
				label_text_v(label, fmt, args);
				va_end(args);
			}
			virtual void label_text_v(const char* label, const char* fmt, va_list args) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::LabelTextV(label, fmt, args);
			}
			virtual void bullet_text(const char* fmt, ...) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				va_list args;
				va_start(args, fmt);
				bullet_text_v(fmt, args);
				va_end(args);
			}
			virtual void bullet_text_v(const char* fmt, va_list args) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::BulletTextV(fmt, args);
			}

			virtual bool button(const char* label, const Float2& size = Float2(0, 0)) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::Button(label, size);
			}
			virtual bool small_button(const char* label) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SmallButton(label);
			}
			virtual bool invisible_button(const char* str_id, const Float2& size) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InvisibleButton(str_id, size);
			}
			virtual bool arrow_button(const char* str_id, EDirection dir) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ArrowButton(str_id, (u32)dir);
			}
			virtual void image(Gfx::IResource* user_texture_id, const Float2& size, const Float2& uv0 = Float2(0, 0), const Float2& uv1 = Float2(1, 1), const Float4& tint_col = Color::white, const Float4& border_col = Color::white) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Image(user_texture_id, size, uv0, uv1, tint_col, border_col);
			}
			virtual bool image_button(Gfx::IResource* user_texture_id, const Float2& size, const Float2& uv0 = Float2(0, 0), const Float2& uv1 = Float2(1, 1), i32 frame_padding = -1, const Float4& bg_col = Float4(0, 0, 0, 0), const Float4& tint_col = Color::white) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ImageButton(user_texture_id, size, uv0, uv1, frame_padding, bg_col, tint_col);
			}
			virtual bool checkbox(const char* label, bool* v) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::Checkbox(label, v);
			}
			virtual bool checkbox_flags(const char* label, u32* flags, u32 flags_value) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::CheckboxFlags(label, flags, flags_value);
			}
			virtual bool radio_button(const char* label, bool active) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::RadioButton(label, active);
			}
			virtual bool radio_button(const char* label, i32* v, i32 v_button) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::RadioButton(label, v, v_button);
			}
			virtual void progress_bar(f32 fraction, const Float2& size_arg = Float2(-1, 0), const char* overlay = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::ProgressBar(fraction, size_arg, overlay);
			}
			virtual void bullet() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Bullet();
			}
			
			virtual bool begin_combo(const char* label, const char* preview_value, EComboFlag flags = EComboFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginCombo(label, preview_value, (u32)flags);
			}
			virtual void end_combo() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndCombo();
			}
			virtual bool combo(const char* label, i32* current_item, const char* const items[], i32 items_count, i32 popup_max_height_in_items = -1) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::Combo(label, current_item, items, items_count, popup_max_height_in_items);
			}
			virtual bool combo(const char* label, i32* current_item, const char* items_separated_by_zeros, i32 popup_max_height_in_items = -1) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::Combo(label, current_item, items_separated_by_zeros, popup_max_height_in_items);
			}
			virtual bool combo(const char* label, i32* current_item, bool(*items_getter)(void* data, i32 idx, const char** out_text), void* data, i32 items_count, i32 popup_max_height_in_items = -1) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::Combo(label, current_item, items_getter, data, items_count, popup_max_height_in_items);
			}

			virtual bool drag_float(const char* label, f32* v, f32 v_speed = 1.0f, f32 v_min = 0.0f, f32 v_max = 0.0f, const char* format = "%.3f", f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragFloat(label, v, v_speed, v_min, v_max, format, power);
			}
			virtual bool drag_float2(const char* label, f32 v[2], f32 v_speed = 1.0f, f32 v_min = 0.0f, f32 v_max = 0.0f, const char* format = "%.3f", f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragFloat2(label, v, v_speed, v_min, v_max, format, power);
			}
			virtual bool drag_float3(const char* label, f32 v[3], f32 v_speed = 1.0f, f32 v_min = 0.0f, f32 v_max = 0.0f, const char* format = "%.3f", f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragFloat3(label, v, v_speed, v_min, v_max, format, power);
			}
			virtual bool drag_float4(const char* label, f32 v[4], f32 v_speed = 1.0f, f32 v_min = 0.0f, f32 v_max = 0.0f, const char* format = "%.3f", f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragFloat4(label, v, v_speed, v_min, v_max, format, power);
			}
			virtual bool drag_float_range2(const char* label, f32* v_current_min, f32* v_current_max, f32 v_speed = 1.0f, f32 v_min = 0.0f, f32 v_max = 0.0f, const char* format = "%.3f", const char* format_max = nullptr, f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragFloatRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, format, format_max, power);
			}
			virtual bool drag_int(const char* label, i32* v, f32 v_speed = 1.0f, i32 v_min = 0, i32 v_max = 0, const char* format = "%d") override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragInt(label, v, v_speed, v_min, v_max, format);
			}
			virtual bool drag_int2(const char* label, i32 v[2], f32 v_speed = 1.0f, i32 v_min = 0, i32 v_max = 0, const char* format = "%d") override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragInt2(label, v, v_speed, v_min, v_max, format);
			}
			virtual bool drag_int3(const char* label, i32 v[3], f32 v_speed = 1.0f, i32 v_min = 0, i32 v_max = 0, const char* format = "%d") override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragInt3(label, v, v_speed, v_min, v_max, format);
			}
			virtual bool drag_int4(const char* label, i32 v[4], f32 v_speed = 1.0f, i32 v_min = 0, i32 v_max = 0, const char* format = "%d") override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragInt4(label, v, v_speed, v_min, v_max, format);
			}
			virtual bool drag_int_range2(const char* label, i32* v_current_min, i32* v_current_max, f32 v_speed = 1.0f, i32 v_min = 0, i32 v_max = 0, const char* format = "%d", const char* format_max = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragIntRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, format, format_max);
			}
			virtual bool drag_scalar(const char* label, EDataType data_type, void* p_data, f32 v_speed, const void* p_min = nullptr, const void* p_max = nullptr, const char* format = nullptr, f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragScalar(label, (u32)data_type, p_data, v_speed, p_min, p_max, format, power);
			}
			virtual bool drag_scalar_n(const char* label, EDataType data_type, void* p_data, i32 components, f32 v_speed, const void* p_min = nullptr, const void* p_max = nullptr, const char* format = nullptr, f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::DragScalarN(label, (u32)data_type, p_data, components, v_speed, p_min, p_max, format, power);
			}

			virtual bool slider_float(const char* label, f32* v, f32 v_min, f32 v_max, const char* format = "%.3f", f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SliderFloat(label, v, v_min, v_max, format, power);
			}
			virtual bool slider_float2(const char* label, f32 v[2], f32 v_min, f32 v_max, const char* format = "%.3f", f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SliderFloat2(label, v, v_min, v_max, format, power);
			}
			virtual bool slider_float3(const char* label, f32 v[3], f32 v_min, f32 v_max, const char* format = "%.3f", f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SliderFloat3(label, v, v_min, v_max, format, power);
			}
			virtual bool slider_float4(const char* label, f32 v[4], f32 v_min, f32 v_max, const char* format = "%.3f", f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SliderFloat4(label, v, v_min, v_max, format, power);
			}
			virtual bool slider_angle(const char* label, f32* v_rad, f32 v_degrees_min = -360.0f, f32 v_degrees_max = +360.0f, const char* format = "%.0f deg") override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max, format);
			}
			virtual bool slider_int(const char* label, i32* v, i32 v_min, i32 v_max, const char* format = "%d") override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SliderInt(label, v, v_min, v_max, format);
			}
			virtual bool slider_int2(const char* label, i32 v[2], i32 v_min, i32 v_max, const char* format = "%d") override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SliderInt2(label, v, v_min, v_max, format);
			}
			virtual bool slider_int3(const char* label, i32 v[3], i32 v_min, i32 v_max, const char* format = "%d") override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SliderInt3(label, v, v_min, v_max, format);
			}
			virtual bool slider_int4(const char* label, i32 v[4], i32 v_min, i32 v_max, const char* format = "%d") override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SliderInt4(label, v, v_min, v_max, format);
			}
			virtual bool slider_scalar(const char* label, EDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = nullptr, f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SliderScalar(label, (u32)data_type, p_data, p_min, p_max, format, power);
			}
			virtual bool slider_scalar_n(const char* label, EDataType data_type, void* p_data, i32 components, const void* p_min, const void* p_max, const char* format = nullptr, f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SliderScalarN(label, (u32)data_type, p_data, components, p_min, p_max, format, power);
			}
			virtual bool vslider_float(const char* label, const Float2& size, f32* v, f32 v_min, f32 v_max, const char* format = "%.3f", f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::VSliderFloat(label, size, v, v_min, v_max, format, power);
			}
			virtual bool vslider_int(const char* label, const Float2& size, i32* v, i32 v_min, i32 v_max, const char* format = "%d") override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::VSliderInt(label, size, v, v_min, v_max, format);
			}
			virtual bool vslider_scalar(const char* label, const Float2& size, EDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = nullptr, f32 power = 1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::VSliderScalar(label, size, (u32)data_type, p_data, p_min, p_max, format, power);
			}

			virtual bool input_text(const char* label, char* buf, usize buf_size, EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputText(label, buf, buf_size, (u32)flags);
			}
			virtual bool input_text_multiline(const char* label, char* buf, usize buf_size, const Float2& size = Float2(0, 0), EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputTextMultiline(label, buf, buf_size, size, (u32)flags);
			}
			virtual bool input_text_with_hint(const char* label, const char* hint, char* buf, usize buf_size, EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputTextWithHint(label, hint, buf, buf_size, (u32)flags, (ImGuiInputTextCallback)callback, user_data);
			}
			virtual bool input_text(const char* label, String& buf, EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) override;
			virtual bool input_text_multiline(const char* label, String& buf, const Float2& size = Float2(0, 0), EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) override;
			virtual bool input_text_with_hint(const char* label, const char* hint, String& buf, EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) override;
			virtual bool input_float(const char* label, f32* v, f32 step = 0.0f, f32 step_fast = 0.0f, const char* format = "%.3f", EInputTextFlag flags = EInputTextFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputFloat(label, v, step, step_fast, format, (u32)flags);
			}
			virtual bool input_float2(const char* label, f32 v[2], const char* format = "%.3f", EInputTextFlag flags = EInputTextFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputFloat2(label, v, format, (u32)flags);
			}
			virtual bool input_float3(const char* label, f32 v[3], const char* format = "%.3f", EInputTextFlag flags = EInputTextFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputFloat3(label, v, format, (u32)flags);
			}
			virtual bool input_float4(const char* label, f32 v[4], const char* format = "%.3f", EInputTextFlag flags = EInputTextFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputFloat4(label, v, format, (u32)flags);
			}
			virtual bool input_int(const char* label, i32* v, i32 step = 1, i32 step_fast = 100, EInputTextFlag flags = EInputTextFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputInt(label, v, step, step_fast, (u32)flags);
			}
			virtual bool input_int2(const char* label, i32 v[2], EInputTextFlag flags = EInputTextFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputInt2(label, v, (u32)flags);
			}
			virtual bool input_int3(const char* label, i32 v[3], EInputTextFlag flags = EInputTextFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputInt3(label, v, (u32)flags);
			}
			virtual bool input_int4(const char* label, i32 v[4], EInputTextFlag flags = EInputTextFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputInt4(label, v, (u32)flags);
			}
			virtual bool input_double(const char* label, f64* v, f64 step = 0.0, f64 step_fast = 0.0, const char* format = "%.6f", EInputTextFlag flags = EInputTextFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputDouble(label, v, step, step_fast, format, (u32)flags);
			}
			virtual bool input_scalar(const char* label, EDataType data_type, void* p_data, const void* p_step = nullptr, const void* p_step_fast = nullptr, const char* format = nullptr, EInputTextFlag flags = EInputTextFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputScalar(label, (u32)data_type, p_data, p_step, p_step_fast, format, (u32)flags);
			}
			virtual bool input_scalar_n(const char* label, EDataType data_type, void* p_data, i32 components, const void* p_step = nullptr, const void* p_step_fast = nullptr, const char* format = nullptr, EInputTextFlag flags = EInputTextFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::InputScalarN(label, (u32)data_type, p_data, components, p_step, p_step_fast, format, (u32)flags);
			}

			virtual bool color_edit3(const char* label, f32 col[3], EColorEditFlag flags = EColorEditFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ColorEdit3(label, col, (u32)flags);
			}
			virtual bool color_edit4(const char* label, f32 col[4], EColorEditFlag flags = EColorEditFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ColorEdit4(label, col, (u32)flags);
			}
			virtual bool color_picker3(const char* label, f32 col[3], EColorEditFlag flags = EColorEditFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ColorPicker3(label, col, (u32)flags);
			}
			virtual bool color_picker4(const char* label, f32 col[4], EColorEditFlag flags = EColorEditFlag::none, const f32* ref_col = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ColorPicker4(label, col, (u32)flags, ref_col);
			}	
			virtual bool color_button(const char* desc_id, const Float4& col, EColorEditFlag flags = EColorEditFlag::none, const Float2& size = Float2(0, 0)) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ColorButton(desc_id, col, (u32)flags, size);
			}
			virtual void set_color_edit_options(EColorEditFlag flags) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetColorEditOptions((u32)flags);
			}

			virtual bool tree_node(const char* label) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::TreeNode(label);
			}
			virtual bool tree_node(const char* str_id, const char* fmt, ...) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				va_list args;
				va_start(args, fmt);
				bool r = tree_node_v(str_id, fmt, args);
				va_end(args);
				return r;
			}
			virtual bool tree_node(const void* ptr_id, const char* fmt, ...) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				va_list args;
				va_start(args, fmt);
				bool r = tree_node_v(ptr_id, fmt, args);
				va_end(args);
				return r;
			}
			virtual bool tree_node_v(const char* str_id, const char* fmt, va_list args) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::TreeNodeV(str_id, fmt, args);
			}
			virtual bool tree_node_v(const void* ptr_id, const char* fmt, va_list args) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::TreeNodeV(ptr_id, fmt, args);
			}
			virtual bool tree_node_ex(const char* label, ETreeNodeFlag flags = ETreeNodeFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::TreeNodeEx(label, (u32)flags);
			}
			virtual bool tree_node_ex(const char* str_id, ETreeNodeFlag flags, const char* fmt, ...) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				va_list args;
				va_start(args, fmt);
				bool r = tree_node_ex_v(str_id, flags, fmt, args);
				va_end(args);
				return r;
			}
			virtual bool tree_node_ex(const void* ptr_id, ETreeNodeFlag flags, const char* fmt, ...) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				va_list args;
				va_start(args, fmt);
				bool r = tree_node_ex_v(ptr_id, flags, fmt, args);
				va_end(args);
				return r;
			}
			virtual bool tree_node_ex_v(const char* str_id, ETreeNodeFlag flags, const char* fmt, va_list args) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::TreeNodeExV(str_id, (u32)flags, fmt, args);
			}
			virtual bool tree_node_ex_v(const void* ptr_id, ETreeNodeFlag flags, const char* fmt, va_list args) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::TreeNodeExV(ptr_id, (u32)flags, fmt, args);
			}
			virtual void tree_push(const char* str_id) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::TreePush(str_id);
			}
			virtual void tree_push(const void* ptr_id = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::TreePush(ptr_id);
			}
			virtual void tree_pop() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::TreePop();
			}
			virtual f32 get_tree_node_to_label_spacing() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetTreeNodeToLabelSpacing();
			}
			virtual bool collapsing_header(const char* label, ETreeNodeFlag flags = ETreeNodeFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::CollapsingHeader(label, (u32)flags);
			}
			virtual bool collapsing_header(const char* label, bool* p_open, ETreeNodeFlag flags = ETreeNodeFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::CollapsingHeader(label, p_open, (u32)flags);
			}
			virtual void set_next_item_open(bool is_open, ECondition cond = ECondition::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetNextItemOpen(is_open, (u32)cond);
			}

			virtual bool selectable(const char* label, bool selected = false, ESelectableFlag flags = ESelectableFlag::none, const Float2& size = Float2(0, 0)) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::Selectable(label, selected, (u32)flags, size);
			}
			virtual bool selectable(const char* label, bool* p_selected, ESelectableFlag flags = ESelectableFlag::none, const Float2& size = Float2(0, 0)) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::Selectable(label, p_selected, (u32)flags, size);
			}

			virtual bool list_box(const char* label, i32* current_item, const char* const items[], i32 items_count, i32 height_in_items = -1) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ListBox(label, current_item, items, items_count, height_in_items);
			}
			virtual bool list_box(const char* label, i32* current_item, bool(*items_getter)(void* data, i32 idx, const char** out_text), void* data, i32 items_count, i32 height_in_items = -1) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ListBox(label, current_item, items_getter, data, height_in_items, height_in_items);
			}
			virtual bool list_box_header(const char* label, const Float2& size = Float2(0, 0)) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ListBoxHeader(label, size);
			}
			virtual bool list_box_header(const char* label, i32 items_count, i32 height_in_items = -1) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::ListBoxHeader(label, items_count, height_in_items);
			}
			virtual void list_box_footer() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::ListBoxFooter();
			}

			virtual void plot_lines(const char* label, const f32* values, i32 values_count, i32 values_offset = 0, const char* overlay_text = nullptr, f32 scale_min = FLT_MAX, f32 scale_max = FLT_MAX, const Float2& graph_size = Float2(0, 0), i32 stride = sizeof(f32)) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
			}
			virtual void plot_lines(const char* label, f32(*values_getter)(void* data, i32 idx), void* data, i32 values_count, i32 values_offset = 0, const char* overlay_text = nullptr, f32 scale_min = FLT_MAX, f32 scale_max = FLT_MAX, const Float2& graph_size = Float2(0, 0)) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PlotLines(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
			}
			virtual void plot_histogram(const char* label, const f32* values, i32 values_count, i32 values_offset = 0, const char* overlay_text = nullptr, f32 scale_min = FLT_MAX, f32 scale_max = FLT_MAX, const Float2& graph_size = Float2(0, 0), i32 stride = sizeof(f32)) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PlotHistogram(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
			}
			virtual void plot_histogram(const char* label, f32(*values_getter)(void* data, i32 idx), void* data, i32 values_count, i32 values_offset = 0, const char* overlay_text = nullptr, f32 scale_min = FLT_MAX, f32 scale_max = FLT_MAX, const Float2& graph_size = Float2(0, 0)) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PlotHistogram(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
			}

			virtual void value(const char* prefix, bool b) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Value(prefix, b);
			}
			virtual void value(const char* prefix, i32 v) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Value(prefix, v);
			}
			virtual void value(const char* prefix, u32 v) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Value(prefix, v);
			}
			virtual void value(const char* prefix, f32 v, const char* float_format = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Value(prefix, v, float_format);
			}

			virtual bool begin_menu_bar() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginMenuBar();
			}
			virtual void end_menu_bar() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndMenuBar();
			}
			virtual bool begin_main_menu_bar() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginMainMenuBar();
			}
			virtual void end_main_menu_bar() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndMainMenuBar();
			}
			virtual bool begin_menu(const char* label, bool enabled = true) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginMenu(label, enabled);
			}
			virtual void end_menu() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndMenu();
			}
			virtual bool menu_item(const char* label, const char* shortcut = nullptr, bool selected = false, bool enabled = true) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::MenuItem(label, shortcut, selected, enabled);
			}
			virtual bool menu_item(const char* label, const char* shortcut, bool* p_selected, bool enabled = true) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::MenuItem(label, shortcut, p_selected, enabled);
			}

			virtual void begin_tooltip() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::BeginTooltip();
			}
			virtual void end_tooltip() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndTooltip();
			}
			virtual void set_tooltip(const char* fmt, ...) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				va_list args;
				va_start(args, fmt);
				set_tooltip_v(fmt, args);
				va_end(args);
			}
			virtual void set_tooltip_v(const char* fmt, va_list args) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetTooltipV(fmt, args);
			}

			virtual void open_popup(const char* str_id) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::OpenPopup(str_id);
			}
			virtual bool begin_popup(const char* str_id, EWindowFlag flags = EWindowFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginPopup(str_id, (u32)flags);
			}
			virtual bool begin_popup_context_item(const char* str_id = nullptr, Input::EMouseKey mouse_button = Input::EMouseKey::rb) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				ImGuiMouseButton btn = 0;
				switch (mouse_button)
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
				return ::ImGui::BeginPopupContextItem(str_id, btn);
			}
			virtual bool begin_popup_context_window(const char* str_id = nullptr, Input::EMouseKey mouse_button = Input::EMouseKey::rb, bool also_over_items = true) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				ImGuiMouseButton btn = 0;
				switch (mouse_button)
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
				return ::ImGui::BeginPopupContextWindow(str_id, btn, also_over_items);
			}
			virtual bool begin_popup_context_void(const char* str_id = nullptr, Input::EMouseKey mouse_button = Input::EMouseKey::rb) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				ImGuiMouseButton btn = 0;
				switch (mouse_button)
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
				return ::ImGui::BeginPopupContextVoid(str_id, btn);
			}
			virtual bool begin_popup_modal(const char* name, bool* p_open = nullptr, EWindowFlag flags = EWindowFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginPopupModal(name, p_open, (u32)flags);
			}
			virtual void end_popup() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndPopup();
			}
			virtual bool open_popup_on_item_click(const char* str_id = nullptr, Input::EMouseKey mouse_button = Input::EMouseKey::rb) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				ImGuiMouseButton btn = 0;
				switch (mouse_button)
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
				return ::ImGui::OpenPopupOnItemClick(str_id, btn);
			}
			virtual bool is_popup_open(const char* str_id) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsPopupOpen(str_id);
			}
			virtual void close_current_popup() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::CloseCurrentPopup();
			}

			virtual void columns(i32 count = 1, const char* id = nullptr, bool border = true) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::Columns(count, id, border);
			}
			virtual void next_column() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::NextColumn();
			}
			virtual i32  get_column_index() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetColumnIndex();
			}
			virtual f32 get_column_width(i32 column_index = -1) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetColumnWidth(column_index);
			}
			virtual void set_column_width(i32 column_index, f32 width) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetColumnWidth(column_index, width);
			}
			virtual f32 get_column_offset(i32 column_index = -1) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetColumnOffset(column_index);
			}
			virtual void set_column_offset(i32 column_index, f32 offset_x) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetColumnOffset(column_index, offset_x);
			}
			virtual i32  get_columns_count() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetColumnsCount();
			}

			virtual bool begin_tab_bar(const char* str_id, ETabBarFlag flags = ETabBarFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginTabBar(str_id, (u32)flags);
			}
			virtual void end_tab_bar() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndTabBar();
			}
			virtual bool begin_tab_item(const char* label, bool* p_open = nullptr, ETabItemFlag flags = ETabItemFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginTabItem(label, p_open, (u32)flags);
			}
			virtual void end_tab_item() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndTabItem();
			}
			virtual void set_tab_item_closed(const char* tab_or_docked_window_label) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetTabItemClosed(tab_or_docked_window_label);
			}

			virtual void dock_space(u32 id, const Float2& size = Float2(0, 0), EDockNodeFlag flags = EDockNodeFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::DockSpace(id, size, (u32)flags);
			}
			virtual void set_next_window_dock_id(u32 dock_id, ECondition cond = ECondition::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetNextWindowDockID(dock_id, (u32)cond);
			}
			virtual u32 get_window_dock_id() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetWindowDockID();
			}
			virtual bool is_window_docked() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsWindowDocked();
			}

			virtual bool begin_drag_drop_source(EDragDropFlag flags = EDragDropFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginDragDropSource((u32)flags);
			}
			virtual bool set_drag_drop_payload(const char* type, const void* data, usize sz, ECondition cond = ECondition::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::SetDragDropPayload(type, data, sz, (u32)cond);
			}
			virtual void end_drag_drop_source() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndDragDropSource();
			}
			virtual bool    begin_drag_drop_target() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginDragDropTarget();
			}
			virtual const Payload*   accept_drag_drop_payload(const char* type, EDragDropFlag flags = EDragDropFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return (const Payload*)::ImGui::AcceptDragDropPayload(type, (u32)flags);
			}
			virtual void    end_drag_drop_target() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndDragDropTarget();
			}
			virtual const Payload*   get_drag_drop_payload() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return (const Payload*)::ImGui::GetDragDropPayload();
			}

			virtual void push_clip_rect(const Float2& clip_rect_min, const Float2& clip_rect_max, bool intersect_with_current_clip_rect) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
			}
			virtual void pop_clip_rect() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::PopClipRect();
			}

			virtual void set_item_default_focus() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetItemDefaultFocus();
			}
			virtual void set_keyboard_focus_here(i32 offset = 0) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetKeyboardFocusHere(offset);
			}

			virtual bool is_item_hovered(EHoveredFlag flags = EHoveredFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsItemHovered((u32)flags);
			}
			virtual bool is_item_active() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsItemActive();
			}
			virtual bool is_item_focused() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsItemFocused();
			}
			virtual bool is_item_clicked(Input::EMouseKey mouse_button = Input::EMouseKey::lb) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				ImGuiMouseButton btn = 0;
				switch (mouse_button)
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
				return ::ImGui::IsItemClicked(btn);
			}
			virtual bool is_item_visible() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsItemVisible();
			}
			virtual bool is_item_edited() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsItemEdited();
			}
			virtual bool is_item_activated() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsItemActivated();
			}
			virtual bool is_item_deactivated() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsItemDeactivated();
			}
			virtual bool is_item_deactivated_after_edit() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsItemDeactivatedAfterEdit();
			}
			virtual bool is_item_toggled_open() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsItemToggledOpen();
			}
			virtual bool is_any_item_hovered() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsAnyItemHovered();
			}
			virtual bool is_any_item_active() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsAnyItemActive();
			}
			virtual bool is_any_item_focused() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsAnyItemFocused();
			}
			virtual Float2 get_item_rect_min() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetItemRectMin();
				return Float2(r.x, r.y);
			}
			virtual Float2 get_item_rect_max() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetItemRectMax();
				return Float2(r.x, r.y);
			}
			virtual Float2 get_item_rect_size() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::GetItemRectSize();
				return Float2(r.x, r.y);
			}
			virtual void set_item_allow_overlap() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::SetItemAllowOverlap();
			}

			virtual bool is_rect_visible(const Float2& size) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsRectVisible(size);
			}
			virtual bool is_rect_visible(const Float2& rect_min, const Float2& rect_max) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::IsRectVisible(rect_min, rect_max);
			}
			virtual h_draw_list_t get_background_draw_list() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetBackgroundDrawList();
			}
			virtual h_draw_list_t get_foreground_draw_list() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetForegroundDrawList();
			}
			virtual const char*  get_style_color_name(EColor idx) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetStyleColorName((u32)idx);
			}
			virtual Float2 calc_text_size(const char* text, const char* text_end = nullptr, bool hide_text_after_double_hash = false, f32 wrap_width = -1.0f) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				auto r = ::ImGui::CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width);
				return Float2(r.x, r.y);
			}
			virtual void calc_list_clipping(i32 items_count, f32 items_height, int* out_items_display_start, int* out_items_display_end) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::CalcListClipping(items_count, items_height, out_items_display_start, out_items_display_end);
			}
			virtual bool begin_child_frame(u32 id, const Float2& size, EWindowFlag flags = EWindowFlag::none) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::BeginChildFrame(id, size, (u32)flags);
			}
			virtual void end_child_frame() override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				::ImGui::EndChildFrame();
			}
			virtual bool is_mouse_clicked(Input::EMouseKey key) override
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
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetIO().MouseClicked[btn];
			}
			virtual bool is_mouse_double_clicked(Input::EMouseKey key) override
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
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetIO().MouseDoubleClicked[btn];
			}
			virtual bool is_mouse_released(Input::EMouseKey key) override
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
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetIO().MouseReleased[btn];
			}
			virtual f32 mouse_down_duration(Input::EMouseKey key) override
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
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				return ::ImGui::GetIO().MouseDownDuration[btn];
			}
			virtual void input_text_callback_data_delete_chars(InputTextCallbackData* data, i32 pos, i32 bytes_count) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				ImGuiInputTextCallbackData* d = (ImGuiInputTextCallbackData*)data;
				d->DeleteChars(pos, bytes_count);
			}
			virtual void input_text_callback_data_insert_chars(InputTextCallbackData* data, int pos, const char* text, const char* text_end = nullptr) override
			{
				MutexGuard _lock(m_mtx);
				::ImGui::SetCurrentContext(m_ctx);
				ImGuiInputTextCallbackData* d = (ImGuiInputTextCallbackData*)data;
				d->InsertChars(pos, text, text_end);
			}
			virtual void gizmo(
				Float4x4& world_matrix,
				const Float4x4& view,
				const Float4x4& projection,
				const RectF& viewport_rect,
				EGizmoOperation operation,
				EGizmoMode mode,
				f32 snap = 0.0f,
				bool enabled = true,
				bool orthographic = false,
				Float4x4* delta_matrix = nullptr,
				bool* is_mouse_hover = nullptr,
				bool* is_mouse_moving = nullptr
			) override;
		};
	}
}