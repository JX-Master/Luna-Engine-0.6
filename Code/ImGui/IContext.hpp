// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IContext.hpp
* @author JXMaster
* @date 2020/4/3
*/
#pragma once
#include <Core/IObject.hpp>
#include <Gfx/Gfx.hpp>

namespace Luna
{
	namespace ImGui
	{
		//! Flags for ImGui::Begin()
		enum class EWindowFlag : u32
		{
			none = 0,
			no_title_bar = 1 << 0,   // Disable title-bar
			no_resize = 1 << 1,   // Disable user resizing with the lower-right grip
			no_move = 1 << 2,   // Disable user moving the window
			no_scrollbar = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
			no_scroll_with_mouse = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
			no_collapse = 1 << 5,   // Disable user collapsing window by f64-clicking on it
			always_auto_resize = 1 << 6,   // Resize every window to its content every frame
			no_background = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
			no_saved_settings = 1 << 8,   // Never load/save settings in .ini file
			no_mouse_inputs = 1 << 9,   // Disable catching mouse, hovering test with pass through.
			menu_bar = 1 << 10,  // Has a menu-bar
			horizontal_scrollbar = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(Float2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
			no_focus_on_appearing = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
			no_bring_to_front_on_focus = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
			always_vertical_scrollbar = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
			always_horizontal_scrollbar = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
			always_use_window_padding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
			no_nav_inputs = 1 << 18,  // No gamepad/keyboard navigation within the window
			no_nav_focus = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
			unsaved_document = 1 << 20,  // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. When used in a tab/docking context, tab is selected on closure and closure is deferred by one frame to allow code to cancel the closure (with a confirmation popup, etc.) without flicker.
			no_docking = 1 << 21,		// Disable docking of this window
			no_nav = no_nav_inputs | no_nav_focus,
			no_decoration = no_title_bar | no_resize | no_scrollbar | no_collapse,
			no_inputs = no_mouse_inputs | no_nav_inputs | no_nav_focus,
		};

		enum class EFocusedFlag : u32
		{
			none = 0,
			child_windows = 1 << 0,   // is_window_focused(): Return true if any children of the window is focused
			root_window = 1 << 1,   // is_window_focused(): Test from root window (top most parent of the current hierarchy)
			any_window = 1 << 2,   // is_window_focused(): Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use ImGui::GetIO().WantCaptureMouse instead.
			root_and_child_windows = root_window | child_windows
		};

		//! Flags for ImGui::TreeNodeEx(), ImGui::CollapsingHeader*()
		enum class TreeNodeFlag : u32
		{
			none = 0,
			selected = 1 << 0,   // Draw as selected
			framed = 1 << 1,   // Full colored frame (e.g. for CollapsingHeader)
			allow_item_overlap = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
			no_tree_push_on_open = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
			no_auto_open_on_log = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
			default_open = 1 << 5,   // Default node to be open
			open_on_float64_click = 1 << 6,   // Need f64-click to open node
			open_on_arrow = 1 << 7,   // Only open when clicking on the arrow part. If OpenOnDoubleClick is also set, single-click arrow or f64-click all box to open.
			leaf = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
			bullet = 1 << 9,   // Display a bullet instead of arrow
			frame_padding = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
			span_avail_width = 1 << 11,  // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
			span_full_width = 1 << 12,  // Extend hit box to the left-most and right-most edges (bypass the indented area).
			nav_left_jumps_back_here = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
			//NoScrollOnOpen     = 1 << 14,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
			collapsing_header = framed | no_tree_push_on_open | no_auto_open_on_log
		};

		// Flags for ImGui::Selectable()
		enum class ESelectableFlag : u32
		{
			none = 0,
			dont_close_popups = 1 << 0,   // Clicking this don't close parent popup window
			span_all_columns = 1 << 1,   // Selectable frame can span all columns (text will still fit in current column)
			allow_float64_click = 1 << 2,   // Generate press events on f64 clicks too
			disabled = 1 << 3,   // Cannot be selected, display grayed out text
			allow_item_overlap = 1 << 4    // (WIP) Hit testing to allow subsequent widgets to overlap this one
		};

		// Flags for ImGui::BeginCombo()
		enum class EComboFlag : u32
		{
			none = 0,
			popup_align_left = 1 << 0,   // Align the popup toward the left by default
			height_small = 1 << 1,   // Max ~4 items visible. Tip: If you want your combo popup to be a specific size you can use SetNextWindowSizeConstraints() prior to calling BeginCombo()
			height_regular = 1 << 2,   // Max ~8 items visible (default)
			height_large = 1 << 3,   // Max ~20 items visible
			height_largest = 1 << 4,   // As many fitting items as possible
			no_arrow_button = 1 << 5,   // Display on the preview box without the square arrow button
			no_preview = 1 << 6,   // Display only a square arrow button
		};

		// Flags for ImGui::BeginTabBar()
		enum class ETabBarFlag : u32
		{
			none = 0,
			reorderable = 1 << 0,   // Allow manually dragging tabs to re-order them + New tabs are appended at the end of list
			auto_select_new_tabs = 1 << 1,   // Automatically select new tabs when they appear
			tab_list_popup_button = 1 << 2,   // Disable buttons to open the tab list popup
			no_close_with_middle_mouse_button = 1 << 3,   // Disable behavior of closing tabs (that are submitted with p_open != nullptr) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
			no_tab_list_scrolling_buttons = 1 << 4,   // Disable scrolling buttons (apply when fitting policy is FittingPolicyScroll)
			no_tooltip = 1 << 5,   // Disable tooltips when hovering a tab
			fitting_policy_resize_down = 1 << 6,   // Resize tabs when they don't fit
			fitting_policy_scroll = 1 << 7,   // Add scroll buttons when tabs don't fit
		};

		// Flags for ImGui::BeginTabItem()
		enum class ETabItemFlag : u32
		{
			none = 0,
			unsaved_document = 1 << 0,   // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. Also: tab is selected on closure and closure is deferred by one frame to allow code to undo it without flicker.
			set_selected = 1 << 1,   // Trigger flag to programmatically make the tab selected when calling BeginTabItem()
			no_close_with_middle_mouse_button = 1 << 2,   // Disable behavior of closing tabs (that are submitted with p_open != nullptr) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
			no_push_id = 1 << 3    // Don't call PushID(tab->ID)/PopID() on BeginTabItem()/EndTabItem()
		};

		// Flags for ImGui::IsWindowFocused()
		enum class EFocusedFlags : u32
		{
			none = 0,
			child_windows = 1 << 0,   // IsWindowFocused(): Return true if any children of the window is focused
			root_window = 1 << 1,   // IsWindowFocused(): Test from root window (top most parent of the current hierarchy)
			any_window = 1 << 2,   // IsWindowFocused(): Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use ImGui::GetIO().WantCaptureMouse instead.
			root_and_child_windows = root_window | child_windows
		};

		// Flags for ImGui::IsItemHovered(), ImGui::IsWindowHovered()
		// Note: if you are trying to check whether your mouse should be dispatched to imgui or to your app, you should use the 'io.WantCaptureMouse' boolean for that. Please read the FAQ!
		// Note: windows with the ImGuiWindowFlags_NoInputs flag are ignored by IsWindowHovered() calls.
		enum class EHoveredFlag : u32
		{
			none = 0,        // Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
			child_windows = 1 << 0,   // IsWindowHovered() only: Return true if any children of the window is hovered
			root_window = 1 << 1,   // IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
			any_window = 1 << 2,   // IsWindowHovered() only: Return true if any window is hovered
			allow_when_blocked_by_popup = 1 << 3,   // Return true even if a popup window is normally blocking access to this item/window
			//AllowWhenBlockedByModal     = 1 << 4,   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
			allow_when_blocked_by_active_item = 1 << 5,   // Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
			allow_when_overlapped = 1 << 6,   // Return true even if the position is obstructed or overlapped by another window
			allow_when_disabled = 1 << 7,   // Return true even if the item is disabled
			rect_only = allow_when_blocked_by_popup | allow_when_blocked_by_active_item | allow_when_overlapped,
			root_and_child_windows = root_window | child_windows
		};

		// Flags for ImGui::BeginDragDropSource(), ImGui::AcceptDragDropPayload()
		enum class EDragDropFlag : u32
		{
			none = 0,
			// BeginDragDropSource() flags
			source_no_preview_tooltip = 1 << 0,   // By default, a successful call to BeginDragDropSource opens a tooltip so you can display a preview or description of the source contents. This flag disable this behavior.
			source_no_disable_hover = 1 << 1,   // By default, when dragging we clear data so that IsItemHovered() will return false, to avoid subsequent user code submitting tooltips. This flag disable this behavior so you can still call IsItemHovered() on the source item.
			source_no_hold_to_open_others = 1 << 2,   // Disable the behavior that allows to open tree nodes and collapsing header by holding over them while dragging a source item.
			source_allow_null_id = 1 << 3,   // Allow items such as Text(), Image() that have no unique identifier to be used as drag source, by manufacturing a temporary identifier based on their window-relative position. This is extremely unusual within the dear imgui ecosystem and so we made it explicit.
			source_extern = 1 << 4,   // External source (from outside of dear imgui), won't attempt to read current item/window info. Will always return true. Only one Extern source can be active simultaneously.
			source_auto_expire_payload = 1 << 5,   // Automatically expire the payload if the source cease to be submitted (otherwise payloads are persisting while being dragged)
			// AcceptDragDropPayload() flags
			accept_before_delivery = 1 << 10,  // AcceptDragDropPayload() will returns true even before the mouse button is released. You can then call IsDelivery() to test if the payload needs to be delivered.
			accept_no_draw_default_rect = 1 << 11,  // Do not draw the default highlight rectangle when hovering over target.
			accept_no_preview_tooltip = 1 << 12,  // Request hiding the BeginDragDropSource tooltip from the BeginDragDropTarget site.
			accept_peek_only = accept_before_delivery | accept_no_draw_default_rect  // For peeking ahead and inspecting the payload before delivery.
		};


		enum class EStyle : u32
		{
			alpha,					// f32   Alpha
			window_padding,			// Float2    WindowPadding
			window_rounding,		// f32   WindowRounding
			window_border_size,		// f32   WindowBorderSize
			window_min_size,		// Float2    WindowMinSize
			window_title_align,		// Float2    WindowTitleAlign
			child_rounding,			// f32   ChildRounding
			child_border_size,		// f32   ChildBorderSize
			popup_rounding,			// f32   PopupRounding
			popup_border_size,		// f32   PopupBorderSize
			frame_padding,			// Float2    FramePadding
			frame_rounding,			// f32   FrameRounding
			frame_border_size,		// f32   FrameBorderSize
			item_spacing,			// Float2    ItemSpacing
			item_inner_spacing,		// Float2    ItemInnerSpacing
			indent_spacing,			// f32   IndentSpacing
			scrollbar_size,			// f32   ScrollbarSize
			scrollbar_rounding,		// f32   ScrollbarRounding
			grab_min_size,			// f32   GrabMinSize
			grab_rounding,			// f32   GrabRounding
			tab_rounding,			// f32   TabRounding
			button_text_align,		// Float2    ButtonTextAlign
			selectable_text_align,	// Float2    SelectableTextAlign
		};

		enum class EColor : u32
		{
			text,
			text_disabled,
			window_bg,              // Background of normal windows
			child_bg,               // Background of child windows
			popup_bg,               // Background of popups, menus, tooltips windows
			border,
			border_shadow,
			frame_bg,               // Background of checkbox, radio button, plot, slider, text input
			frame_bg_hovered,
			frame_bg_active,
			title_bg,
			title_bg_active,
			title_bg_collapsed,
			menu_bar_bg,
			scrollbar_bg,
			scrollbar_grab,
			scrollbar_grab_hovered,
			scrollbar_grab_active,
			check_mark,
			slider_grab,
			slider_grab_active,
			button,
			button_hovered,
			button_active,
			header,                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
			header_hovered,
			header_active,
			separator,
			separator_hovered,
			separator_active,
			resize_grip,
			resize_grip_hovered,
			resize_grip_active,
			tab,
			tab_hovered,
			tab_active,
			tab_unfocused,
			tab_unfocused_active,
			docking_preview,        // Preview overlay color when about to docking something
			docking_empty_bg,        // Background color for empty node (e.g. CentralNode with no window docked into it)
			plot_lines,
			plot_lines_hovered,
			plot_histogram,
			plot_histogram_hovered,
			text_selectet_bg,
			drag_drop_target,
			nav_highlight,            // Gamepad/keyboard: current highlighted item
			nav_windowing_highlight,  // Highlight window when using CTRL+TAB
			nav_windowing_dim_bg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
			modal_window_dim_bg,      // Darken/colorize entire screen behind a modal window, when one is active
			count
		};

		enum class ECondition : u32
		{
			none = 0,
			always = 1 << 0,   // Set the variable
			once = 1 << 1,   // Set the variable once per runtime session (only the first call with succeed)
			first_use_ever = 1 << 2,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
			appearing = 1 << 3    // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
		};

		enum class EDirection : i32
		{
			none = -1,
			left = 0,
			right = 1,
			up = 2,
			down = 3,
		};

		enum class EDataType : u32
		{
			s8,       
			u8,      
			s16,      
			u16,      
			s32,      
			u32,      
			s64,      
			u64,      
			f32,	
			f64,  
		};

		enum class EInputTextFlag : u32
		{
			none = 0,
			chars_decimal = 1 << 0,   // Allow 0123456789.+-*/
			chars_hexadecimal = 1 << 1,   // Allow 0123456789ABCDEFabcdef
			chars_uppercase = 1 << 2,   // Turn a..z into A..Z
			chars_no_blank = 1 << 3,   // Filter out spaces, tabs
			auto_select_all = 1 << 4,   // Select entire text when first taking mouse focus
			enter_returns_true = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
			callback_completion = 1 << 6,   // Callback on pressing TAB (for completion handling)
			callback_history = 1 << 7,   // Callback on pressing Up/Down arrows (for history handling)
			callback_always = 1 << 8,   // Callback on each iteration. User code may query cursor position, modify text buffer.
			callback_char_filter = 1 << 9,   // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
			allow_tab_input = 1 << 10,  // Pressing TAB input a '\t' character into the text field
			ctrl_enter_for_new_line = 1 << 11,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
			no_horizontal_scroll = 1 << 12,  // Disable following the cursor horizontally
			always_insert_mode = 1 << 13,  // Insert mode
			read_only = 1 << 14,  // Read-only mode
			password = 1 << 15,  // Password mode, display all characters as '*'
			no_undo_redo = 1 << 16,  // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
			chars_scientific = 1 << 17,  // Allow 0123456789.+-*/eE (Scientific notation input)
			callback_resize = 1 << 18,  // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/imgui_stdlib.h for an example of using this)
		};

		enum class EDrawCornerFlag : u32
		{
			none = 0,
			top_left = 1 << 0, // 0x1
			top_right = 1 << 1, // 0x2
			bot_left = 1 << 2, // 0x4
			bot_right = 1 << 3, // 0x8
			top = top_left | top_right,   // 0x3
			bot = bot_left | bot_right,   // 0xC
			left = top_left | bot_left,    // 0x5
			right = top_right | bot_right,  // 0xA
			all = 0xF     // In your function calls you may use ~0 (= all bits sets) instead of All, as a convenience
		};

		enum class EDockNodeFlag : u32
		{
			none							= 0,
			keep_alive_only					= 1 << 0,   // Shared       // Don't display the dockspace node but keep it alive. Windows docked into this dockspace node won't be undocked.
			no_docking_in_central_node		= 1 << 2,   // Shared       // Disable docking inside the Central Node, which will be always kept empty.
			passthru_central_node			= 1 << 3,   // Shared       // Enable passthru dockspace: 1) DockSpace() will render a ImGuiCol_WindowBg background covering everything excepted the Central Node when empty. Meaning the host window should probably use SetNextWindowBgAlpha(0.0f) prior to Begin() when using this. 2) When Central Node is empty: let inputs pass-through + won't display a DockingEmptyBg background. See demo for details.
			no_split						= 1 << 4,   // Shared/Local // Disable splitting the node into smaller nodes. Useful e.g. when embedding dockspaces into a main root one (the root one may have splitting disabled to reduce confusion). Note: when turned off, existing splits will be preserved.
			no_resize						= 1 << 5,   // Shared/Local // Disable resizing node using the splitter/separators. Useful with programatically setup dockspaces.
			auto_hide_tab_bar				= 1 << 6    // Shared/Local // Tab bar will automatically hide when there is a single window in the dock node.
		};

		enum class EGizmoOperation : u32
		{
			translate = 0,
			rotate = 1,
			scale = 2,
			bounds = 3,
		};

		enum class EGizmoMode : u32
		{
			local = 0,
			world = 1,
		};

		struct InputTextCallbackData
		{
			EInputTextFlag		event_flag;      // One ImGuiInputTextFlags_Callback*    // Read-only
			EInputTextFlag		flags;          // What user passed to InputText()      // Read-only
			void*               user_data;       // What user passed to InputText()      // Read-only

			// Arguments for the different callback events
			// - To modify the text buffer in a callback, prefer using the InsertChars() / DeleteChars() function. InsertChars() will take care of calling the resize callback if necessary.
			// - If you know your edits are not going to resize the underlying buffer allocation, you may modify the contents of 'Buf[]' directly. You need to update 'BufTextLen' accordingly (0 <= BufTextLen < BufSize) and set 'BufDirty'' to true so InputText can update its internal state.
			unsigned short      event_char;      // Character input                      // Read-write   // [CharFilter] Replace character with another one, or set to zero to drop. return 1 is equivalent to setting EventChar=0;
			int                 event_key;       // Key pressed (Up/Down/TAB)            // Read-only    // [Completion,History]
			char*               buf;            // Text buffer                          // Read-write   // [Resize] Can replace pointer / [Completion,History,Always] Only write to pointed data, don't replace the actual pointer!
			int                 buf_text_len;     // Text length (in bytes)               // Read-write   // [Resize,Completion,History,Always] Exclude zero-terminator storage. In C land: == strlen(some_text), in C++ land: string.length()
			int                 buf_size;        // Buffer size (in bytes) = capacity+1  // Read-only    // [Resize,Completion,History,Always] Include zero-terminator storage. In C land == ARRAYSIZE(my_char_array), in C++ land: string.capacity()+1
			bool                buf_dirty;       // Set if you modify Buf/BufTextLen!    // Write        // [Completion,History,Always]
			int                 cursor_pos;      //                                      // Read-write   // [Completion,History,Always]
			int                 selection_start; //                                      // Read-write   // [Completion,History,Always] == to SelectionEnd when no selection)
			int                 selection_end;   //                                      // Read-write   // [Completion,History,Always]

			// Helper functions for text manipulation.
			// Use those function to benefit from the CallbackResize behaviors. Calling those function reset the selection.
			InputTextCallbackData() { memset(this, 0, sizeof(*this)); }
			bool has_selection() const { return selection_start != selection_end; }
		};

		struct Style
		{
			f32       Alpha;                      // Global alpha applies to everything in Dear ImGui.
			Float2U      WindowPadding;              // Padding within a window.
			f32       WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows.
			f32       WindowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
			Float2U      WindowMinSize;              // Minimum window size. This is a global setting. If you want to constraint individual windows, use SetNextWindowSizeConstraints().
			Float2U      WindowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
			EDirection    WindowMenuButtonPosition;   // Side of the collapsing/docking button in the title bar (None/Left/Right). Defaults to ImGuiDir_Left.
			f32       ChildRounding;              // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
			f32       ChildBorderSize;            // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
			f32       PopupRounding;              // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
			f32       PopupBorderSize;            // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
			Float2U      FramePadding;               // Padding within a framed rectangle (used by most widgets).
			f32       FrameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
			f32       FrameBorderSize;            // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
			Float2U      ItemSpacing;                // Horizontal and vertical spacing between widgets/lines.
			Float2U      ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
			Float2U      TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
			f32       IndentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
			f32       ColumnsMinSpacing;          // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
			f32       ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
			f32       ScrollbarRounding;          // Radius of grab corners for scrollbar.
			f32       GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
			f32       GrabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
			f32       TabRounding;                // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
			f32       TabBorderSize;              // Thickness of border around tabs.
			EDirection    ColorButtonPosition;        // Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right.
			Float2U      ButtonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
			Float2U      SelectableTextAlign;        // Alignment of selectable text when selectable is larger than text. Defaults to (0.0f, 0.0f) (top-left aligned).
			Float2U      DisplayWindowPadding;       // Window position are clamped to be visible within the display area by at least this amount. Only applies to regular windows.
			Float2U      DisplaySafeAreaPadding;     // If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!
			f32       MouseCursorScale;           // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
			bool        AntiAliasedLines;           // Enable anti-aliasing on lines/borders. Disable if you are really tight on CPU/GPU.
			bool        AntiAliasedFill;            // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
			f32       CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
			f32       CircleSegmentMaxError;      // Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.
			Float4U      Colors[(u32)EColor::count];

			void ScaleAllSizes(f32 scale_factor)
			{
				WindowPadding.x = floorf(WindowPadding.x * scale_factor);
				WindowPadding.y = floorf(WindowPadding.y * scale_factor);
				WindowRounding = floorf(WindowRounding * scale_factor);
				WindowMinSize.x = floorf(WindowMinSize.x * scale_factor);
				WindowMinSize.y = floorf(WindowMinSize.y * scale_factor);
				ChildRounding = floorf(ChildRounding * scale_factor);
				PopupRounding = floorf(PopupRounding * scale_factor);
				FramePadding.x = floorf(FramePadding.x * scale_factor);
				FramePadding.y = floorf(FramePadding.y * scale_factor);
				FrameRounding = floorf(FrameRounding * scale_factor);
				ItemSpacing.x = floorf(ItemSpacing.x * scale_factor);
				ItemSpacing.y = floorf(ItemSpacing.y * scale_factor);
				ItemInnerSpacing.x = floorf(ItemInnerSpacing.x * scale_factor);
				ItemInnerSpacing.y = floorf(ItemInnerSpacing.y * scale_factor);
				TouchExtraPadding.x = floorf(TouchExtraPadding.x * scale_factor);
				TouchExtraPadding.y = floorf(TouchExtraPadding.y * scale_factor);
				IndentSpacing = floorf(IndentSpacing * scale_factor);
				ColumnsMinSpacing = floorf(ColumnsMinSpacing * scale_factor);
				ScrollbarSize = floorf(ScrollbarSize * scale_factor);
				ScrollbarRounding = floorf(ScrollbarRounding * scale_factor);
				GrabMinSize = floorf(GrabMinSize * scale_factor);
				GrabRounding = floorf(GrabRounding * scale_factor);
				TabRounding = floorf(TabRounding * scale_factor);
				DisplayWindowPadding.x = floorf(DisplayWindowPadding.x * scale_factor);
				DisplayWindowPadding.y = floorf(DisplayWindowPadding.y * scale_factor);
				DisplaySafeAreaPadding.x = floorf(DisplaySafeAreaPadding.x * scale_factor);
				DisplaySafeAreaPadding.y = floorf(DisplaySafeAreaPadding.y * scale_factor);
				MouseCursorScale = floorf(MouseCursorScale * scale_factor);
			}
		};

		using input_text_callback_t = int(InputTextCallbackData* data);

		enum class EColorEditFlag : u32
		{
			none = 0,
			no_alpha = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only read 3 components from the input pointer).
			no_picker = 1 << 2,   //              // ColorEdit: disable picker when clicking on colored square.
			no_options = 1 << 3,   //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
			no_small_preview = 1 << 4,   //              // ColorEdit, ColorPicker: disable colored square preview next to the inputs. (e.g. to show only the inputs)
			no_inputs = 1 << 5,   //              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview colored square).
			no_tooltip = 1 << 6,   //              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
			no_label = 1 << 7,   //              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the tooltip and picker).
			no_side_preview = 1 << 8,   //              // ColorPicker: disable bigger color preview on right side of the picker, use small colored square preview instead.
			no_drag_drop = 1 << 9,   //              // ColorEdit: disable drag and drop target. ColorButton: disable drag and drop source.

			// User Options (right-click on widget to change some of them).
			alpha_bar = 1 << 16,  //              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
			alpha_preview = 1 << 17,  //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
			alpha_preview_half = 1 << 18,  //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
			hdr = 1 << 19,  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to use Float flag as well).
			display_rgb = 1 << 20,  // [Display]    // ColorEdit: override _display_ type among RGB/HSV/Hex. ColorPicker: select any combination using one or more of RGB/HSV/Hex.
			display_hsv = 1 << 21,  // [Display]    // "
			display_hex = 1 << 22,  // [Display]    // "
			u8 = 1 << 23,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
			f32 = 1 << 24,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
			picker_hue_bar = 1 << 25,  // [Picker]     // ColorPicker: bar for Hue, rectangle for Sat/Value.
			picker_hue_wheel = 1 << 26,  // [Picker]     // ColorPicker: wheel for Hue, triangle for Sat/Value.
			input_rgb = 1 << 27,  // [Input]      // ColorEdit, ColorPicker: input and output data in RGB format.
			input_hsv = 1 << 28,  // [Input]      // ColorEdit, ColorPicker: input and output data in HSV format.
		};

		enum class ETreeNodeFlag : u32
		{
			none = 0,
			selected = 1 << 0,   // Draw as selected
			framed = 1 << 1,   // Full colored frame (e.g. for CollapsingHeader)
			allow_item_overlap = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
			no_tree_push_on_open = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
			no_auto_open_on_log = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
			default_open = 1 << 5,   // Default node to be open
			open_on_double_click = 1 << 6,   // Need double-click to open node
			open_on_arrow = 1 << 7,   // Only open when clicking on the arrow part. If OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
			leaf = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
			bullet = 1 << 9,   // Display a bullet instead of arrow
			frame_padding = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
			span_avail_width = 1 << 11,  // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
			span_full_width = 1 << 12,  // Extend hit box to the left-most and right-most edges (bypass the indented area).
			nav_left_jumps_back_here = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
			//NoScrollOnOpen     = 1 << 14,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
			collapsing_header = framed | no_tree_push_on_open | no_auto_open_on_log
		};

		// Data payload for Drag and Drop operations: AcceptDragDropPayload(), GetDragDropPayload()
		struct Payload
		{
			// Members
			void*           data;               // Data (copied and owned by dear imgui)
			int             data_size;           // Data size

			// [Internal]
			unsigned int    source_id;           // Source item id
			unsigned int    source_parent_id;     // Source parent id (if available)
			int             data_frame_count;     // Data timestamp
			char            data_type[32 + 1];     // Data type tag (short user-supplied string, 32 characters max)
			bool            preview;            // Set when AcceptDragDropPayload() was called and mouse has been hovering the target item (nb: handle overlapping drag targets)
			bool            delivery;           // Set when AcceptDragDropPayload() was called and mouse button is released over the target item.

			Payload() { clear(); }
			void clear() { source_id = source_parent_id = 0; data = nullptr; data_size = 0; memset(data_type, 0, sizeof(data_type)); data_frame_count = -1; preview = delivery = false; }
			bool is_data_type(const char* type) const { return data_frame_count != -1 && strcmp(type, data_type) == 0; }
			bool is_preview() const { return preview; }
			bool is_delivery() const { return delivery; }
		};

		using h_draw_list_t = handle_t;
		using h_io_t = handle_t;

		//! @interface IContext
		//! Represents a ImGui context. Every window and and in-game viewport should have its own context.
		struct IContext : public IObject
		{
			luiid("{51298ea0-14cc-40ae-8b45-585af5062652}");

			//! Attaches this context to a system window so that is interacts with the window.
			virtual RV attach_system_window(Gfx::IWindow* window) = 0;

			//! Detaches the attached system window.
			virtual void detach_system_window() = 0;

			virtual h_io_t get_io() = 0;                                    // access the IO structure (mouse/keyboard/gamepad inputs, time, various configuration options/flags)
			virtual Style* get_style() = 0;                              // access the Style structure (colors, sizes). Always use PushStyleCol(), PushStyleVar() to modify style mid-frame.

			//! Clears the previous render data and begins a new frame. This should be called exactly once in every frame for every 
			//! context.
			//! You should call this before appending any command.
			virtual void new_frame() = 0;

			//! Finishes one frame and generates draw calls. No command can be appended after this call.
			//! The draw call data is valid until next `begin_frame` call.
			virtual void end_frame() = 0;

			//! Renders commands into the provided command buffer to draw the content. This must be called after `end_frame` for every 
			//! frame.
			virtual RV render(Gfx::ICommandBuffer* cmd_buffer, Gfx::IResource* render_target) = 0;

			// Demo, Debug, Information
			virtual void          show_demo_window(bool* p_open = nullptr) = 0;        // create Demo window (previously called ShowTestWindow). demonstrate most ImGui features. call this to learn about the library! try to make it always available in your application!
			virtual void          show_about_window(bool* p_open = nullptr) = 0;       // create About window. display Dear ImGui version, credits and build/system information.
			virtual void          show_metrics_window(bool* p_open = nullptr) = 0;     // create Metrics/Debug window. display Dear ImGui internals: draw commands (with individual draw calls and vertices), window list, basic internal state, etc.
			virtual void          show_style_editor() = 0;							// add style editor block (not a window). you can pass in a reference ImGuiStyle structure to compare to, revert to and save to (else it uses the default style)
			virtual bool          show_style_selector(const char* label) = 0;       // add style selector block (not a window), essentially a combo listing the default styles.
			virtual void          show_font_selector(const char* label) = 0;        // add font selector block (not a window), essentially a combo listing the loaded fonts.
			virtual void          show_user_guide() = 0;                            // add basic help/info block (not a window): how to manipulate ImGui as a end-user (mouse/keyboard controls).
			virtual const char*   get_version() = 0;                               // get the compiled version string e.g. "1.23" (essentially the compiled value for IMGUI_VERSION)

			// Styles
			virtual void          style_colors_dark() = 0;    // new, recommended style (default)
			virtual void          style_colors_classic() = 0; // classic imgui style
			virtual void          style_colors_light() = 0;   // best used with borders and a custom, thicker font

			// Windows
			// - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
			// - You may append multiple times to the same window during the same frame.
			// - Passing 'bool* p_open != nullptr' shows a window-closing widget in the upper-right corner of the window,
			//   which clicking will set the boolean to false when clicked.
			// - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
			//   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
			//   [Important: due to legacy reason, this is inconsistent with most other functions such as BeginMenu/EndMenu,
			//    BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function
			//    returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
			// - Note that the bottom of window stack always contains a window called "Debug".
			virtual bool begin(const char* name, bool* open = nullptr, EWindowFlag flags = EWindowFlag::none) = 0;
			virtual void end() = 0;

			// Child Windows
			// - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
			// - For each independent axis of 'size': ==0.0f: use remaining host window size / >0.0f: fixed size / <0.0f: use remaining window size minus abs(size) / Each axis can use a different mode, e.g. Float2(0,400).
			// - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting anything to the window.
			//   Always call a matching EndChild() for each BeginChild() call, regardless of its return value [as with Begin: this is due to legacy reason and inconsistent with most BeginXXX functions apart from the regular Begin() which behaves like BeginChild().]
			virtual bool begin_child(const char* str_id, const Float2& size = Float2(0.0f, 0.0f), bool border = false, EWindowFlag flags = EWindowFlag::none) = 0;
			virtual bool begin_child(i32 id, const Float2& size = Float2(0.0f, 0.0f), bool border = false, EWindowFlag flags = EWindowFlag::none) = 0;
			virtual void end_child() = 0;

			// Windows Utilities
			// - 'current window' = the window we are appending into while inside a Begin()/End() block. 'next window' = next window we will Begin() into.
			virtual bool          is_window_appearing() = 0;
			virtual bool          is_window_collapsed() = 0;
			virtual bool          is_window_focused(EFocusedFlag flags = EFocusedFlag::none) = 0; // is current window focused? or its root/child, depending on flags. see flags for options.
			virtual bool          is_window_hovered(EHoveredFlag flags = EHoveredFlag::none) = 0; // is current window hovered (and typically: not blocked by a popup/modal)? see flags for options. NB: If you are trying to check whether your mouse should be dispatched to imgui or to your app, you should use the 'io.WantCaptureMouse' boolean for that! Please read the FAQ!
			virtual h_draw_list_t	  get_window_draw_list() = 0;						// get draw list associated to the current window, to append your own drawing primitives. (Using draw list APIs to append).
			virtual Float2        get_window_pos() = 0;                             // get current window position in screen space (useful if you want to do your own drawing via the DrawList API)
			virtual Float2        get_window_size() = 0;                            // get current window size
			virtual f32       get_window_width() = 0;                           // get current window width (shortcut for GetWindowSize().x)
			virtual f32       get_window_height() = 0;                          // get current window height (shortcut for GetWindowSize().y)

			// Prefer using SetNextXXX functions (before Begin) rather that SetXXX functions (after Begin).
			virtual void          set_next_window_pos(const Float2& pos, ECondition cond = ECondition::none, const Float2& pivot = Float2(0, 0)) = 0; // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
			virtual void          set_next_window_size(const Float2& size, ECondition cond = ECondition::none) = 0;                  // set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin()
			virtual void          set_next_window_size_constraints(const Float2& size_min, const Float2& size_max) = 0; // set next window size limits. use -1,-1 on either X/Y axis to preserve the current size. Sizes will be rounded down. Use callback to apply non-trivial programmatic constraints.
			virtual void          set_next_window_content_size(const Float2& size) = 0;                               // set next window content size (~ scrollable client area, which enforce the range of scrollbars). Not including window decorations (title bar, menu bar, etc.) nor WindowPadding. set an axis to 0.0f to leave it automatic. call before Begin()
			virtual void          set_next_window_collapsed(bool collapsed, ECondition cond = ECondition::none) = 0;                 // set next window collapsed state. call before Begin()
			virtual void          set_next_window_focus() = 0;                                                       // set next window to be focused / top-most. call before Begin()
			virtual void          set_next_window_bg_alpha(f32 alpha) = 0;                                          // set next window background color alpha. helper to easily override the Alpha component of ImGuiCol_WindowBg/ChildBg/PopupBg. you may also use ImGuiWindowFlags_NoBackground.
			virtual void          set_window_pos(const Float2& pos, ECondition cond = ECondition::none) = 0;                        // (not recommended) set current window position - call within Begin()/End(). prefer using SetNextWindowPos(), as this may incur tearing and side-effects.
			virtual void          set_window_size(const Float2& size, ECondition cond = ECondition::none) = 0;                      // (not recommended) set current window size - call within Begin()/End(). set to Float2(0,0) to force an auto-fit. prefer using SetNextWindowSize(), as this may incur tearing and minor side-effects.
			virtual void          set_window_collapsed(bool collapsed, ECondition cond = ECondition::none) = 0;                     // (not recommended) set current window collapsed state. prefer using SetNextWindowCollapsed().
			virtual void          set_window_focus() = 0;                                                           // (not recommended) set current window to be focused / top-most. prefer using SetNextWindowFocus().
			virtual void          set_window_font_scale(f32 scale) = 0;                                            // set font scale. Adjust IO.FontGlobalScale if you want to scale all windows. This is an old API! For correct scaling, prefer to reload font + rebuild ImFontAtlas + call style.ScaleAllSizes().
			virtual void          set_window_pos(const char* name, const Float2& pos, ECondition cond = ECondition::none) = 0;      // set named window position.
			virtual void          set_window_size(const char* name, const Float2& size, ECondition cond = ECondition::none) = 0;    // set named window size. set axis to 0.0f to force an auto-fit on this axis.
			virtual void          set_window_collapsed(const char* name, bool collapsed, ECondition cond = ECondition::none) = 0;   // set named window collapsed state
			virtual void          set_window_focus(const char* name) = 0;                                           // set named window to be focused / top-most. use nullptr to remove focus.

			// Content region
			// - Those functions are bound to be redesigned soon (they are confusing, incomplete and return values in local window coordinates which increases confusion)
			virtual Float2        get_content_region_max() = 0;                                          // current content boundaries (typically window boundaries including scrolling, or current column boundaries), in windows coordinates
			virtual Float2        get_content_region_avail() = 0;                                        // == GetContentRegionMax() - GetCursorPos()
			virtual Float2        get_window_content_region_min() = 0;                                    // content boundaries min (roughly (0,0)-Scroll), in window coordinates
			virtual Float2        get_window_content_region_max() = 0;                                    // content boundaries max (roughly (0,0)+Size-Scroll) where Size can be override with SetNextWindowContentSize(), in window coordinates
			virtual f32       get_window_content_region_width() = 0;                                  //

			// Windows Scrolling
			virtual f32       get_scroll_x() = 0;                                                   // get scrolling amount [0..GetScrollMaxX()]
			virtual f32       get_scroll_y() = 0;                                                   // get scrolling amount [0..GetScrollMaxY()]
			virtual f32       get_scroll_max_x() = 0;                                                // get maximum scrolling amount ~~ ContentSize.X - WindowSize.X
			virtual f32       get_scroll_max_y() = 0;                                                // get maximum scrolling amount ~~ ContentSize.Y - WindowSize.Y
			virtual void          set_scroll_x(f32 scroll_x) = 0;                                     // set scrolling amount [0..GetScrollMaxX()]
			virtual void          set_scroll_y(f32 scroll_y) = 0;                                     // set scrolling amount [0..GetScrollMaxY()]
			virtual void          set_scroll_here_x(f32 center_x_ratio = 0.5f) = 0;                    // adjust scrolling amount to make current cursor position visible. center_x_ratio=0.0: left, 0.5: center, 1.0: right. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
			virtual void          set_scroll_here_y(f32 center_y_ratio = 0.5f) = 0;                    // adjust scrolling amount to make current cursor position visible. center_y_ratio=0.0: top, 0.5: center, 1.0: bottom. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
			virtual void          set_scroll_from_pos_x(f32 local_x, f32 center_x_ratio = 0.5f) = 0;  // adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position.
			virtual void          set_scroll_from_pos_y(f32 local_y, f32 center_y_ratio = 0.5f) = 0;  // adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position.

			// Parameters stacks (shared)
			virtual void          pop_font() = 0;
			virtual void          push_style_color(EColor idx, u32 col) = 0;
			virtual void          push_style_color(EColor idx, const Float4& col) = 0;
			virtual void          pop_style_color(i32 count = 1) = 0;
			virtual void          push_style_var(EStyle idx, f32 val) = 0;
			virtual void          push_style_var(EStyle idx, const Float2& val) = 0;
			virtual void          pop_style_var(i32 count = 1) = 0;
			virtual Float4		  get_style_color_vec4(EColor idx) = 0;                                // retrieve style color as stored in ImGuiStyle structure. use to feed back into PushStyleColor(), otherwise use GetColorU32() to get style color with style alpha baked in.
			virtual f32       get_font_size() = 0;                                                  // get current font size (= height in pixels) of current font with current scale applied
			virtual Float2        get_font_tex_uv_white_pixel() = 0;                                       // get UV coordinate for a while pixel, useful to draw custom shapes via the ImDrawList API
			virtual u32        get_color_u32(EColor idx, f32 alpha_mul = 1.0f) = 0;              // retrieve given style color with style alpha applied and optional extra alpha multiplier
			virtual u32        get_color_u32(const Float4& col) = 0;                                 // retrieve given color with style alpha applied
			virtual u32        get_color_u32(u32 col) = 0;                                         // retrieve given color with style alpha applied

			// Parameters stacks (current window)
			virtual void          push_item_width(f32 item_width) = 0;                                // set width of items for common large "item+label" widgets. >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side). 0.0f = default to ~2/3 of windows width,
			virtual void          pop_item_width() = 0;
			virtual void          set_next_item_width(f32 item_width) = 0;                             // set width of the _next_ common large "item+label" widget. >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side)
			virtual f32       calc_item_width() = 0;                                                // width of item given pushed settings and current cursor position. NOT necessarily the width of last item unlike most 'Item' functions.
			virtual void          push_text_wrap_pos(f32 wrap_local_pos_x = 0.0f) = 0;                 // word-wrapping for Text*() commands. < 0.0f: no wrapping; 0.0f: wrap to end of window (or column); > 0.0f: wrap at 'wrap_pos_x' position in window local space
			virtual void          pop_text_wrap_pos() = 0;
			virtual void          push_allow_keyboard_focus(bool allow_keyboard_focus) = 0;              // allow focusing using TAB/Shift-TAB, enabled by default but you can disable it for certain widgets
			virtual void          pop_allow_keyboard_focus() = 0;
			virtual void          push_button_repeat(bool repeat) = 0;                                  // in 'repeat' mode, Button*() functions return repeated true in a typematic manner (using io.KeyRepeatDelay/io.KeyRepeatRate setting). Note that you can call IsItemActive() after any Button() to tell if the button is held in the current frame.
			virtual void          pop_button_repeat() = 0;
		
			// Cursor / Layout
			// - By "cursor" we mean the current output position.
			// - The typical widget behavior is to output themselves at the current cursor position, then move the cursor one line down.
			// - You can call SameLine() between widgets to undo the last carriage return and output at the right of the preceeding widget.
			virtual void          separator() = 0;                                                    // separator, generally horizontal. inside a menu bar or in horizontal layout mode, this becomes a vertical separator.
			virtual void          same_line(f32 offset_from_start_x = 0.0f, f32 spacing = -1.0f) = 0;  // call between widgets or groups to layout them horizontally. X position given in window coordinates.
			virtual void          new_line() = 0;                                                      // undo a SameLine() or force a new line when in an horizontal-layout context.
			virtual void          spacing() = 0;                                                      // add vertical spacing.
			virtual void          dummy(const Float2& size) = 0;                                      // add a dummy item of given size. unlike InvisibleButton(), Dummy() won't take the mouse click or be navigable into.
			virtual void          indent(f32 indent_w = 0.0f) = 0;                                  // move content position toward the right, by style.IndentSpacing or indent_w if != 0
			virtual void          unindent(f32 indent_w = 0.0f) = 0;                                // move content position back to the left, by style.IndentSpacing or indent_w if != 0
			virtual void          begin_group() = 0;                                                   // lock horizontal starting position
			virtual void          end_group() = 0;                                                     // unlock horizontal starting position + capture the whole group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.)
			virtual Float2        get_cursor_pos() = 0;                                                 // cursor position in window coordinates (relative to window position)
			virtual f32       get_cursor_pos_x() = 0;                                                //   (some functions are using window-relative coordinates, such as: GetCursorPos, GetCursorStartPos, GetContentRegionMax, GetWindowContentRegion* etc.
			virtual f32       get_cursor_pos_y() = 0;                                                //    other functions such as GetCursorScreenPos or everything in ImDrawList::
			virtual void          set_cursor_pos(const Float2& local_pos) = 0;                          //    are using the main, absolute coordinate system.
			virtual void          set_cursor_pos_x(f32 local_x) = 0;                                   //    GetWindowPos() + GetCursorPos() == GetCursorScreenPos() etc.)
			virtual void          set_cursor_pos_y(f32 local_y) = 0;                                   //
			virtual Float2        get_cursor_start_pos() = 0;                                            // initial cursor position in window coordinates
			virtual Float2        get_cursor_screen_pos() = 0;                                           // cursor position in absolute screen coordinates [0..io.DisplaySize] (useful to work with ImDrawList API)
			virtual void          set_cursor_screen_pos(const Float2& pos) = 0;                          // cursor position in absolute screen coordinates [0..io.DisplaySize]
			virtual void          align_text_to_frame_padding() = 0;                                      // vertically align upcoming text baseline to FramePadding.y so that it will align properly to regularly framed items (call if you have text on a line before a framed item)
			virtual f32       get_text_line_height() = 0;                                            // ~ FontSize
			virtual f32       get_text_line_height_with_spacing() = 0;                                 // ~ FontSize + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of text)
			virtual f32       get_frame_height() = 0;                                               // ~ FontSize + style.FramePadding.y * 2
			virtual f32       get_frame_height_with_spacing() = 0;                                    // ~ FontSize + style.FramePadding.y * 2 + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of framed widgets)

			// ID stack/scopes
			// - Read the FAQ for more details about how ID are handled in dear imgui. If you are creating widgets in a loop you most
			//   likely want to push a unique identifier (e.g. object pointer, loop index) to uniquely differentiate them.
			// - The resulting ID are hashes of the entire stack.
			// - You can also use the "Label##foobar" syntax within widget label to distinguish them from each others.
			// - In this header file we use the "label"/"name" terminology to denote a string that will be displayed and used as an ID,
			//   whereas "str_id" denote a string that is only used as an ID and not normally displayed.
			virtual void          push_id(const char* str_id) = 0;                                     // push string into the ID stack (will hash string).
			virtual void          push_id(const char* str_id_begin, const char* str_id_end) = 0;       // push string into the ID stack (will hash string).
			virtual void          push_id(const void* ptr_id) = 0;                                     // push pointer into the ID stack (will hash pointer).
			virtual void          push_id(i32 int_id) = 0;                                             // push integer into the ID stack (will hash integer).
			virtual void          pop_id() = 0;                                                        // pop from the ID stack.
			virtual u32        get_id(const char* str_id) = 0;                                      // calculate unique ID (hash of whole ID stack + given parameter). e.g. if you want to query into ImGuiStorage yourself
			virtual u32        get_id(const char* str_id_begin, const char* str_id_end) = 0;
			virtual u32        get_id(const void* ptr_id) = 0;

			// Widgets: Text
			virtual void          text_unformatted(const char* text, const char* text_end = nullptr) = 0;	// raw text without formatting. Roughly equivalent to Text("%s", text) but: A) doesn't require null terminated string if 'text_end' is specified, B) it's faster, no memory copy is done, no buffer size limits, recommended for long chunks of text.
			virtual void          text(const char* fmt, ...) = 0;											// formatted text
			virtual void          text_v(const char* fmt, va_list args) = 0;
			virtual void          text_colored(const Float4& col, const char* fmt, ...) = 0;				// shortcut for PushStyleColor(ImGuiCol_Text, col); Text(fmt, ...); PopStyleColor();
			virtual void          text_colored_v(const Float4& col, const char* fmt, va_list args) = 0;
			virtual void          text_disabled(const char* fmt, ...) = 0;									// shortcut for PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]); Text(fmt, ...); PopStyleColor();
			virtual void          text_disabled_v(const char* fmt, va_list args) = 0;
			virtual void          text_wrapped(const char* fmt, ...) = 0;									// shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().
			virtual void          text_wrapped_v(const char* fmt, va_list args) = 0;
			virtual void          label_text(const char* label, const char* fmt, ...) = 0;					// display text+label aligned the same way as value+label widgets
			virtual void		  label_text_v(const char* label, const char* fmt, va_list args) = 0;
			virtual void          bullet_text(const char* fmt, ...) = 0;									// shortcut for Bullet()+Text()
			virtual void          bullet_text_v(const char* fmt, va_list args) = 0;

			// Widgets: Main
			// - Most widgets return true when the value has been changed or when pressed/selected
			// - You may also use one of the many IsItemXXX functions (e.g. IsItemActive, IsItemHovered, etc.) to query widget state.
			virtual bool          button(const char* label, const Float2& size = Float2(0, 0)) = 0;    // button
			virtual bool          small_button(const char* label) = 0;                                 // button with FramePadding=(0,0) to easily embed within text
			virtual bool          invisible_button(const char* str_id, const Float2& size) = 0;        // button behavior without the visuals, frequently useful to build custom behaviors using the public api (along with IsItemActive, IsItemHovered, etc.)
			virtual bool          arrow_button(const char* str_id, EDirection dir) = 0;                  // square button with an arrow shape
			virtual void          image(Gfx::IResource* user_texture_id, const Float2& size, const Float2& uv0 = Float2(0, 0), const Float2& uv1 = Float2(1, 1), const Float4& tint_col = Color::white, const Float4& border_col = Color::white) = 0;
			virtual bool          image_button(Gfx::IResource* user_texture_id, const Float2& size, const Float2& uv0 = Float2(0, 0), const Float2& uv1 = Float2(1, 1), i32 frame_padding = -1, const Float4& bg_col = Float4(0, 0, 0, 0), const Float4& tint_col = Color::white) = 0;    // <0 frame_padding uses default frame padding settings. 0 for no padding
			virtual bool          checkbox(const char* label, bool* v) = 0;
			virtual bool          checkbox_flags(const char* label, u32* flags, u32 flags_value) = 0;
			virtual bool          radio_button(const char* label, bool active) = 0;                    // use with e.g. if (RadioButton("one", my_value==1)) { my_value = 1; }
			virtual bool          radio_button(const char* label, i32* v, i32 v_button) = 0;           // shortcut to handle the above pattern when value is an integer
			virtual void          progress_bar(f32 fraction, const Float2& size_arg = Float2(-1, 0), const char* overlay = nullptr) = 0;
			virtual void          bullet() = 0;                                                       // draw a small circle and keep the cursor on the same line. advance cursor x position by GetTreeNodeToLabelSpacing(), same distance that TreeNode() uses

			// Widgets: Combo Box
			// - The BeginCombo()/EndCombo() api allows you to manage your contents and selection state however you want it, by creating e.g. Selectable() items.
			// - The old Combo() api are helpers over BeginCombo()/EndCombo() which are kept available for convenience purpose.
			virtual bool          begin_combo(const char* label, const char* preview_value, EComboFlag flags = EComboFlag::none) = 0;
			virtual void          end_combo() = 0; // only call EndCombo() if BeginCombo() returns true!
			virtual bool          combo(const char* label, i32* current_item, const char* const items[], i32 items_count, i32 popup_max_height_in_items = -1) = 0;
			virtual bool          combo(const char* label, i32* current_item, const char* items_separated_by_zeros, i32 popup_max_height_in_items = -1) = 0;      // Separate items with \0 within a string, end item-list with \0\0. e.g. "One\0Two\0Three\0"
			virtual bool          combo(const char* label, i32* current_item, bool(*items_getter)(void* data, i32 idx, const char** out_text), void* data, i32 items_count, i32 popup_max_height_in_items = -1) = 0;

			// Widgets: Drags
			// - CTRL+Click on any drag box to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
			// - For all the Float2/Float3/Float4/Int2/Int3/Int4 versions of every functions, note that a 'f32 v[X]' function argument is the same as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible. You can pass address of your first element out of a contiguous set, e.g. &myvector.x
			// - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
			// - Speed are per-pixel of mouse movement (v_speed=0.2f: mouse needs to move by 5 pixels to increase value by 1). For gamepad/keyboard navigation, minimum speed is Max(v_speed, minimum_step_at_given_precision).
			// - Use v_min < v_max to clamp edits to given limits. Note that CTRL+Click manual input can override those limits.
			// - Use v_max = FLT_MAX / INT_MAX etc to avoid clamping to a maximum, same with v_min = -FLT_MAX / INT_MIN to avoid clamping to a minimum.
			// - Use v_min > v_max to lock edits.
			virtual bool          drag_float(const char* label, f32* v, f32 v_speed = 1.0f, f32 v_min = 0.0f, f32 v_max = 0.0f, const char* format = "%.3f", f32 power = 1.0f) = 0;     // If v_min >= v_max we have no bound
			virtual bool          drag_float2(const char* label, f32 v[2], f32 v_speed = 1.0f, f32 v_min = 0.0f, f32 v_max = 0.0f, const char* format = "%.3f", f32 power = 1.0f) = 0;
			virtual bool          drag_float3(const char* label, f32 v[3], f32 v_speed = 1.0f, f32 v_min = 0.0f, f32 v_max = 0.0f, const char* format = "%.3f", f32 power = 1.0f) = 0;
			virtual bool          drag_float4(const char* label, f32 v[4], f32 v_speed = 1.0f, f32 v_min = 0.0f, f32 v_max = 0.0f, const char* format = "%.3f", f32 power = 1.0f) = 0;
			virtual bool          drag_float_range2(const char* label, f32* v_current_min, f32* v_current_max, f32 v_speed = 1.0f, f32 v_min = 0.0f, f32 v_max = 0.0f, const char* format = "%.3f", const char* format_max = nullptr, f32 power = 1.0f) = 0;
			virtual bool          drag_int(const char* label, i32* v, f32 v_speed = 1.0f, i32 v_min = 0, i32 v_max = 0, const char* format = "%d") = 0;                                       // If v_min >= v_max we have no bound
			virtual bool          drag_int2(const char* label, i32 v[2], f32 v_speed = 1.0f, i32 v_min = 0, i32 v_max = 0, const char* format = "%d") = 0;
			virtual bool          drag_int3(const char* label, i32 v[3], f32 v_speed = 1.0f, i32 v_min = 0, i32 v_max = 0, const char* format = "%d") = 0;
			virtual bool          drag_int4(const char* label, i32 v[4], f32 v_speed = 1.0f, i32 v_min = 0, i32 v_max = 0, const char* format = "%d") = 0;
			virtual bool          drag_int_range2(const char* label, i32* v_current_min, i32* v_current_max, f32 v_speed = 1.0f, i32 v_min = 0, i32 v_max = 0, const char* format = "%d", const char* format_max = nullptr) = 0;
			virtual bool          drag_scalar(const char* label, EDataType data_type, void* p_data, f32 v_speed, const void* p_min = nullptr, const void* p_max = nullptr, const char* format = nullptr, f32 power = 1.0f) = 0;
			virtual bool          drag_scalar_n(const char* label, EDataType data_type, void* p_data, i32 components, f32 v_speed, const void* p_min = nullptr, const void* p_max = nullptr, const char* format = nullptr, f32 power = 1.0f) = 0;

			// Widgets: Sliders
			// - CTRL+Click on any slider to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
			// - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
			virtual bool          slider_float(const char* label, f32* v, f32 v_min, f32 v_max, const char* format = "%.3f", f32 power = 1.0f) = 0;     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for power curve sliders
			virtual bool          slider_float2(const char* label, f32 v[2], f32 v_min, f32 v_max, const char* format = "%.3f", f32 power = 1.0f) = 0;
			virtual bool          slider_float3(const char* label, f32 v[3], f32 v_min, f32 v_max, const char* format = "%.3f", f32 power = 1.0f) = 0;
			virtual bool          slider_float4(const char* label, f32 v[4], f32 v_min, f32 v_max, const char* format = "%.3f", f32 power = 1.0f) = 0;
			virtual bool          slider_angle(const char* label, f32* v_rad, f32 v_degrees_min = -360.0f, f32 v_degrees_max = +360.0f, const char* format = "%.0f deg") = 0;
			virtual bool          slider_int(const char* label, i32* v, i32 v_min, i32 v_max, const char* format = "%d") = 0;
			virtual bool          slider_int2(const char* label, i32 v[2], i32 v_min, i32 v_max, const char* format = "%d") = 0;
			virtual bool          slider_int3(const char* label, i32 v[3], i32 v_min, i32 v_max, const char* format = "%d") = 0;
			virtual bool          slider_int4(const char* label, i32 v[4], i32 v_min, i32 v_max, const char* format = "%d") = 0;
			virtual bool          slider_scalar(const char* label, EDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = nullptr, f32 power = 1.0f) = 0;
			virtual bool          slider_scalar_n(const char* label, EDataType data_type, void* p_data, i32 components, const void* p_min, const void* p_max, const char* format = nullptr, f32 power = 1.0f) = 0;
			virtual bool          vslider_float(const char* label, const Float2& size, f32* v, f32 v_min, f32 v_max, const char* format = "%.3f", f32 power = 1.0f) = 0;
			virtual bool          vslider_int(const char* label, const Float2& size, i32* v, i32 v_min, i32 v_max, const char* format = "%d") = 0;
			virtual bool          vslider_scalar(const char* label, const Float2& size, EDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = nullptr, f32 power = 1.0f) = 0;

			// Widgets: Input with Keyboard
			// - If you want to use InputText() with std::string or any custom dynamic string type, see misc/cpp/imgui_stdlib.h and comments in imgui_demo.cpp.
			// - Most of the ImGuiInputTextFlags flags are only useful for InputText() and not for InputFloatX, InputIntX, InputDouble etc.
			virtual bool          input_text(const char* label, char* buf, usize buf_size, EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) = 0;
			virtual bool          input_text_multiline(const char* label, char* buf, usize buf_size, const Float2& size = Float2(0, 0), EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) = 0;
			virtual bool          input_text_with_hint(const char* label, const char* hint, char* buf, usize buf_size, EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) = 0;
			virtual bool          input_text(const char* label, String& buf, EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) = 0;
			virtual bool          input_text_multiline(const char* label, String& buf, const Float2& size = Float2(0, 0), EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) = 0;
			virtual bool          input_text_with_hint(const char* label, const char* hint, String& buf, EInputTextFlag flags = EInputTextFlag::none, input_text_callback_t* callback = nullptr, void* user_data = nullptr) = 0;
			virtual bool          input_float(const char* label, f32* v, f32 step = 0.0f, f32 step_fast = 0.0f, const char* format = "%.3f", EInputTextFlag flags = EInputTextFlag::none) = 0;
			virtual bool          input_float2(const char* label, f32 v[2], const char* format = "%.3f", EInputTextFlag flags = EInputTextFlag::none) = 0;
			virtual bool          input_float3(const char* label, f32 v[3], const char* format = "%.3f", EInputTextFlag flags = EInputTextFlag::none) = 0;
			virtual bool          input_float4(const char* label, f32 v[4], const char* format = "%.3f", EInputTextFlag flags = EInputTextFlag::none) = 0;
			virtual bool          input_int(const char* label, i32* v, i32 step = 1, i32 step_fast = 100, EInputTextFlag flags = EInputTextFlag::none) = 0;
			virtual bool          input_int2(const char* label, i32 v[2], EInputTextFlag flags = EInputTextFlag::none) = 0;
			virtual bool          input_int3(const char* label, i32 v[3], EInputTextFlag flags = EInputTextFlag::none) = 0;
			virtual bool          input_int4(const char* label, i32 v[4], EInputTextFlag flags = EInputTextFlag::none) = 0;
			virtual bool          input_double(const char* label, f64* v, f64 step = 0.0, f64 step_fast = 0.0, const char* format = "%.6f", EInputTextFlag flags = EInputTextFlag::none) = 0;
			virtual bool          input_scalar(const char* label, EDataType data_type, void* p_data, const void* p_step = nullptr, const void* p_step_fast = nullptr, const char* format = nullptr, EInputTextFlag flags = EInputTextFlag::none) = 0;
			virtual bool          input_scalar_n(const char* label, EDataType data_type, void* p_data, i32 components, const void* p_step = nullptr, const void* p_step_fast = nullptr, const char* format = nullptr, EInputTextFlag flags = EInputTextFlag::none) = 0;

			// Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little colored preview square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
			// - Note that in C++ a 'f32 v[X]' function argument is the _same_ as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible.
			// - You can pass the address of a first f32 element out of a contiguous structure, e.g. &myvector.x
			virtual bool          color_edit3(const char* label, f32 col[3], EColorEditFlag flags = EColorEditFlag::none) = 0;
			virtual bool          color_edit4(const char* label, f32 col[4], EColorEditFlag flags = EColorEditFlag::none) = 0;
			virtual bool          color_picker3(const char* label, f32 col[3], EColorEditFlag flags = EColorEditFlag::none) = 0;
			virtual bool          color_picker4(const char* label, f32 col[4], EColorEditFlag flags = EColorEditFlag::none, const f32* ref_col = nullptr) = 0;
			virtual bool          color_button(const char* desc_id, const Float4& col, EColorEditFlag flags = EColorEditFlag::none, const Float2& size = Float2(0, 0)) = 0;  // display a colored square/button, hover for details, return true when pressed.
			virtual void          set_color_edit_options(EColorEditFlag flags) = 0;                     // initialize current options (generally on application startup) if you want to select a default format, picker type, etc. User will be able to change many settings, unless you pass the _NoOptions flag to your calls.

			// Widgets: Trees
			// - TreeNode functions return true when the node is open, in which case you need to also call TreePop() when you are finished displaying the tree node contents.
			virtual bool          tree_node(const char* label) = 0;
			virtual bool          tree_node(const char* str_id, const char* fmt, ...) = 0;   // helper variation to easily decorelate the id from the displayed string. Read the FAQ about why and how to use ID. to align arbitrary text at the same level as a TreeNode() you can use Bullet().
			virtual bool          tree_node(const void* ptr_id, const char* fmt, ...) = 0;   // "
			virtual bool          tree_node_v(const char* str_id, const char* fmt, va_list args) = 0;
			virtual bool          tree_node_v(const void* ptr_id, const char* fmt, va_list args) = 0;
			virtual bool          tree_node_ex(const char* label, ETreeNodeFlag flags = ETreeNodeFlag::none) = 0;
			virtual bool          tree_node_ex(const char* str_id, ETreeNodeFlag flags, const char* fmt, ...) = 0;
			virtual bool          tree_node_ex(const void* ptr_id, ETreeNodeFlag flags, const char* fmt, ...) = 0;
			virtual bool          tree_node_ex_v(const char* str_id, ETreeNodeFlag flags, const char* fmt, va_list args) = 0;
			virtual bool          tree_node_ex_v(const void* ptr_id, ETreeNodeFlag flags, const char* fmt, va_list args) = 0;
			virtual void          tree_push(const char* str_id) = 0;                                       // ~ Indent()+PushId(). Already called by TreeNode() when returning true, but you can call TreePush/TreePop yourself if desired.
			virtual void          tree_push(const void* ptr_id = nullptr) = 0;                                // "
			virtual void          tree_pop() = 0;                                                          // ~ Unindent()+PopId()
			virtual f32       get_tree_node_to_label_spacing() = 0;                                        // horizontal distance preceding label when using TreeNode*() or Bullet() == (g.FontSize + style.FramePadding.x*2) for a regular unframed TreeNode
			virtual bool          collapsing_header(const char* label, ETreeNodeFlag flags = ETreeNodeFlag::none) = 0;  // if returning 'true' the header is open. doesn't indent nor push on ID stack. user doesn't have to call TreePop().
			virtual bool          collapsing_header(const char* label, bool* p_open, ETreeNodeFlag flags = ETreeNodeFlag::none) = 0; // when 'p_open' isn't nullptr, display an additional small close button on upper right of the header
			virtual void          set_next_item_open(bool is_open, ECondition cond = ECondition::none) = 0;                  // set next TreeNode/CollapsingHeader open state.

			// Widgets: Selectables
			// - A selectable highlights when hovered, and can display another color when selected.
			// - Neighbors selectable extend their highlight bounds in order to leave no gap between them. This is so a series of selected Selectable appear contiguous.
			virtual bool          selectable(const char* label, bool selected = false, ESelectableFlag flags = ESelectableFlag::none, const Float2& size = Float2(0, 0)) = 0;  // "bool selected" carry the selection state (read-only). Selectable() is clicked is returns true so you can modify your selection state. size.x==0.0: use remaining width, size.x>0.0: specify width. size.y==0.0: use label height, size.y>0.0: specify height
			virtual bool          selectable(const char* label, bool* p_selected, ESelectableFlag flags = ESelectableFlag::none, const Float2& size = Float2(0, 0)) = 0;       // "bool* p_selected" point32 to the selection state (read-write), as a convenient helper.

			// Widgets: List Boxes
			// - FIXME: To be consistent with all the newer API, ListBoxHeader/ListBoxFooter should in reality be called BeginListBox/EndListBox. Will rename them.
			virtual bool          list_box(const char* label, i32* current_item, const char* const items[], i32 items_count, i32 height_in_items = -1) = 0;
			virtual bool          list_box(const char* label, i32* current_item, bool(*items_getter)(void* data, i32 idx, const char** out_text), void* data, i32 items_count, i32 height_in_items = -1) = 0;
			virtual bool          list_box_header(const char* label, const Float2& size = Float2(0, 0)) = 0; // use if you want to reimplement ListBox() will custom data or interactions. if the function return true, you can output elements then call ListBoxFooter() afterwards.
			virtual bool          list_box_header(const char* label, i32 items_count, i32 height_in_items = -1) = 0; // "
			virtual void          list_box_footer() = 0;                                                    // terminate the scrolling region. only call ListBoxFooter() if ListBoxHeader() returned true!

																										// Widgets: Data Plotting
			virtual void          plot_lines(const char* label, const f32* values, i32 values_count, i32 values_offset = 0, const char* overlay_text = nullptr, f32 scale_min = FLT_MAX, f32 scale_max = FLT_MAX, const Float2& graph_size = Float2(0, 0), i32 stride = sizeof(f32)) = 0;
			virtual void          plot_lines(const char* label, f32(*values_getter)(void* data, i32 idx), void* data, i32 values_count, i32 values_offset = 0, const char* overlay_text = nullptr, f32 scale_min = FLT_MAX, f32 scale_max = FLT_MAX, const Float2& graph_size = Float2(0, 0)) = 0;
			virtual void          plot_histogram(const char* label, const f32* values, i32 values_count, i32 values_offset = 0, const char* overlay_text = nullptr, f32 scale_min = FLT_MAX, f32 scale_max = FLT_MAX, const Float2& graph_size = Float2(0, 0), i32 stride = sizeof(f32)) = 0;
			virtual void          plot_histogram(const char* label, f32(*values_getter)(void* data, i32 idx), void* data, i32 values_count, i32 values_offset = 0, const char* overlay_text = nullptr, f32 scale_min = FLT_MAX, f32 scale_max = FLT_MAX, const Float2& graph_size = Float2(0, 0)) = 0;

			// Widgets: Value() Helpers.
			// - Those are merely shortcut to calling Text() with a format string. Output single value in "name: value" format (tip: freely declare more in your code to handle your types. you can add functions to the ImGui namespace)
			virtual void          value(const char* prefix, bool b) = 0;
			virtual void          value(const char* prefix, i32 v) = 0;
			virtual void          value(const char* prefix, u32 v) = 0;
			virtual void          value(const char* prefix, f32 v, const char* float_format = nullptr) = 0;

			// Widgets: Menus
			// - Use BeginMenuBar() on a window ImGuiWindowFlags_MenuBar to append to its menu bar.
			// - Use BeginMainMenuBar() to create a menu bar at the top of the screen.
			virtual bool          begin_menu_bar() = 0;                                                     // append to menu-bar of current window (requires ImGuiWindowFlags_MenuBar flag set on parent window).
			virtual void          end_menu_bar() = 0;                                                       // only call EndMenuBar() if BeginMenuBar() returns true!
			virtual bool          begin_main_menu_bar() = 0;                                                 // create and append to a full screen menu-bar.
			virtual void          end_main_menu_bar() = 0;                                                   // only call EndMainMenuBar() if BeginMainMenuBar() returns true!
			virtual bool          begin_menu(const char* label, bool enabled = true) = 0;                  // create a sub-menu entry. only call EndMenu() if this returns true!
			virtual void          end_menu() = 0;                                                          // only call EndMenu() if BeginMenu() returns true!
			virtual bool          menu_item(const char* label, const char* shortcut = nullptr, bool selected = false, bool enabled = true) = 0;  // return true when activated. shortcuts are displayed for convenience but not processed by ImGui at the moment
			virtual bool          menu_item(const char* label, const char* shortcut, bool* p_selected, bool enabled = true) = 0;              // return true when activated + toggle (*p_selected) if p_selected != nullptr

			// Tooltips
			// - Tooltip are windows following the mouse which do not take focus away.
			virtual void          begin_tooltip() = 0;                                                     // begin/append a tooltip window. to create full-featured tooltip (with any kind of items).
			virtual void          end_tooltip() = 0;
			virtual void          set_tooltip(const char* fmt, ...) = 0;                     // set a text-only tooltip, typically use with ImGui::IsItemHovered(). override any previous call to SetTooltip().
			virtual void          set_tooltip_v(const char* fmt, va_list args) = 0;

			// Popups, Modals
			// The properties of popups windows are:
			// - They block normal mouse hovering detection outside them. (*)
			// - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
			// - Their visibility state (~bool) is held internally by imgui instead of being held by the programmer as we are used to with regular Begin() calls.
			//   User can manipulate the visibility state by calling OpenPopup().
			// - We default to use the right mouse (ImGuiMouseButton_Right=1) for the Popup Context functions.
			// (*) You can use IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even when normally blocked by a popup.
			// Those three properties are connected. The library needs to hold their visibility state because it can close popups at any time.
			virtual void          open_popup(const char* str_id) = 0;                                      // call to mark popup as open (don't call every frame!). popups are closed when user click outside, or if CloseCurrentPopup() is called within a BeginPopup()/EndPopup() block. By default, Selectable()/MenuItem() are calling CloseCurrentPopup(). Popup identifiers are relative to the current ID-stack (so OpenPopup and BeginPopup needs to be at the same level).
			virtual bool          begin_popup(const char* str_id, EWindowFlag flags = EWindowFlag::none) = 0;                                             // return true if the popup is open, and you can start outputting to it. only call EndPopup() if BeginPopup() returns true!
			virtual bool          begin_popup_context_item(const char* str_id = nullptr, Input::EMouseKey mouse_button = Input::EMouseKey::rb) = 0;                    // helper to open and begin popup when clicked on last item. if you can pass a nullptr str_id only if the previous item had an id. If you want to use that on a non-interactive item such as Text() you need to pass in an explicit ID here. read comments in .cpp!
			virtual bool          begin_popup_context_window(const char* str_id = nullptr, Input::EMouseKey mouse_button = Input::EMouseKey::rb, bool also_over_items = true) = 0;  // helper to open and begin popup when clicked on current window.
			virtual bool          begin_popup_context_void(const char* str_id = nullptr, Input::EMouseKey mouse_button = Input::EMouseKey::rb) = 0;                    // helper to open and begin popup when clicked in void (where there are no imgui windows).
			virtual bool          begin_popup_modal(const char* name, bool* p_open = nullptr, EWindowFlag flags = EWindowFlag::none) = 0;                     // modal dialog (regular window with title bar, block interactions behind the modal window, can't close the modal window by clicking outside)
			virtual void          end_popup() = 0;                                                                                             // only call EndPopup() if BeginPopupXXX() returns true!
			virtual bool          open_popup_on_item_click(const char* str_id = nullptr, Input::EMouseKey mouse_button = Input::EMouseKey::rb) = 0;                     // helper to open popup when clicked on last item (note: actually triggers on the mouse _released_ event to be consistent with popup behaviors). return true when just opened.
			virtual bool          is_popup_open(const char* str_id) = 0;                                    // return true if the popup is open at the current begin-ed level of the popup stack.
			virtual void          close_current_popup() = 0;                                                // close the popup we have begin-ed into. clicking on a MenuItem or Selectable automatically close the current popup.

			// Columns
			// - You can also use SameLine(pos_x) to mimic simplified columns.
			// - The columns API is work-in-progress and rather lacking (columns are arguably the worst part of dear imgui at the moment!)
			// - There is a maximum of 64 columns.
			// - Currently working on new 'Tables' api which will replace columns (see GitHub #2957)
			virtual void          columns(i32 count = 1, const char* id = nullptr, bool border = true) = 0;
			virtual void          next_column() = 0;                                                       // next column, defaults to current row or next row if the current row is finished
			virtual i32         get_column_index() = 0;                                                   // get current column index
			virtual f32       get_column_width(i32 column_index = -1) = 0;                              // get column width (in pixels). pass -1 to use current column
			virtual void          set_column_width(i32 column_index, f32 width) = 0;                      // set column width (in pixels). pass -1 to use current column
			virtual f32       get_column_offset(i32 column_index = -1) = 0;                             // get position of column line (in pixels, from the left side of the contents region). pass -1 to use current column, otherwise 0..GetColumnsCount() inclusive. column 0 is typically 0.0f
			virtual void          set_column_offset(i32 column_index, f32 offset_x) = 0;                  // set position of column line (in pixels, from the left side of the contents region). pass -1 to use current column
			virtual i32         get_columns_count() = 0;

			// Tab Bars, Tabs
			virtual bool          begin_tab_bar(const char* str_id, ETabBarFlag flags = ETabBarFlag::none) = 0;        // create and append into a TabBar
			virtual void          end_tab_bar() = 0;                                                        // only call EndTabBar() if BeginTabBar() returns true!
			virtual bool          begin_tab_item(const char* label, bool* p_open = nullptr, ETabItemFlag flags = ETabItemFlag::none) = 0;// create a Tab. Returns true if the Tab is selected.
			virtual void          end_tab_item() = 0;                                                       // only call EndTabItem() if BeginTabItem() returns true!
			virtual void          set_tab_item_closed(const char* tab_or_docked_window_label) = 0;           // notify TabBar or Docking system of a closed tab/window ahead (useful to reduce visual flicker on reorderable tab bars). For tab-bar: call after BeginTabBar() and before Tab submissions. Otherwise call with a window name.

			// Docking
			// [BETA API] Enable with io.ConfigFlags |= ImGuiConfigFlags_DockingEnable.
			// Note: You can use most Docking facilities without calling any API. You DO NOT need to call DockSpace() to use Docking!
			// - To dock windows: if io.ConfigDockingWithShift == false (default) drag window from their title bar.
			// - To dock windows: if io.ConfigDockingWithShift == true: hold SHIFT anywhere while moving windows.
			// About DockSpace:
			// - Use DockSpace() to create an explicit dock node _within_ an existing window. See Docking demo for details.
			// - DockSpace() needs to be submitted _before_ any window they can host. If you use a dockspace, submit it early in your app.
			virtual void          dock_space(u32 id, const Float2& size = Float2(0, 0), EDockNodeFlag flags = EDockNodeFlag::none) = 0;
			virtual void          set_next_window_dock_id(u32 dock_id, ECondition cond = ECondition::none) = 0;           // set next window dock id (FIXME-DOCK)
			virtual u32        get_window_dock_id() = 0;
			virtual bool          is_window_docked() = 0;                                                   // is current window docked into another window?

			// Drag and Drop
			// [BETA API] API may evolve!
			virtual bool          begin_drag_drop_source(EDragDropFlag flags = EDragDropFlag::none) = 0;                         // call when the current item is active. If this return true, you can call SetDragDropPayload() + EndDragDropSource()
			virtual bool          set_drag_drop_payload(const char* type, const void* data, usize sz, ECondition cond = ECondition::none) = 0;  // type is a user defined string of maximum 32 characters. Strings starting with '_' are reserved for dear imgui internal types. Data is copied and held by imgui.
			virtual void          end_drag_drop_source() = 0;                                                                    // only call EndDragDropSource() if BeginDragDropSource() returns true!
			virtual bool             begin_drag_drop_target() = 0;                                                          // call after submitting an item that may receive a payload. If this returns true, you can call AcceptDragDropPayload() + EndDragDropTarget()
			virtual const Payload*   accept_drag_drop_payload(const char* type, EDragDropFlag flags = EDragDropFlag::none) = 0;          // accept contents of a given type. If ImGuiDragDropFlags_AcceptBeforeDelivery is set you can peek into the payload before the mouse button is released.
			virtual void             end_drag_drop_target() = 0;                                                            // only call EndDragDropTarget() if BeginDragDropTarget() returns true!
			virtual const Payload*   get_drag_drop_payload() = 0;                                                           // peek directly into the current payload from anywhere. may return nullptr. use ImGuiPayload::IsDataType() to test for the payload type.

			// Clipping
			virtual void          push_clip_rect(const Float2& clip_rect_min, const Float2& clip_rect_max, bool intersect_with_current_clip_rect) = 0;
			virtual void          pop_clip_rect() = 0;

			// Focus, Activation
			// - Prefer using "SetItemDefaultFocus()" over "if (IsWindowAppearing()) SetScrollHereY()" when applicable to signify "this is the default item"
			virtual void          set_item_default_focus() = 0;                                              // make last item the default focused item of a window.
			virtual void          set_keyboard_focus_here(i32 offset = 0) = 0;                               // focus keyboard on the next widget. Use positive 'offset' to access sub components of a multiple component widget. Use -1 to access previous widget.

			// Item/Widgets Utilities
			// - Most of the functions are referring to the last/previous item we submitted.
			// - See Demo Window under "Widgets->Querying Status" for an interactive visualization of most of those functions.
			virtual bool          is_item_hovered(EHoveredFlag flags = EHoveredFlag::none) = 0;             // is the last item hovered? (and usable, aka not blocked by a popup, etc.). See ImGuiHoveredFlags for more options.
			virtual bool          is_item_active() = 0;                                                     // is the last item active? (e.g. button being held, text field being edited. This will continuously return true while holding mouse button on an item. Items that don't interact will always return false)
			virtual bool          is_item_focused() = 0;                                                    // is the last item focused for keyboard/gamepad navigation?
			virtual bool          is_item_clicked(Input::EMouseKey mouse_button = Input::EMouseKey::lb) = 0;// is the last item clicked? (e.g. button/node just clicked on) == IsMouseClicked(mouse_button) && IsItemHovered()
			virtual bool          is_item_visible() = 0;                                                    // is the last item visible? (items may be out of sight because of clipping/scrolling)
			virtual bool          is_item_edited() = 0;                                                     // did the last item modify its underlying value this frame? or was pressed? This is generally the same as the "bool" return value of many widgets.
			virtual bool          is_item_activated() = 0;                                                  // was the last item just made active (item was previously inactive).
			virtual bool          is_item_deactivated() = 0;                                                // was the last item just made inactive (item was previously active). Useful for Undo/Redo patterns with widgets that requires continuous editing.
			virtual bool          is_item_deactivated_after_edit() = 0;                                       // was the last item just made inactive and made a value change when it was active? (e.g. Slider/Drag moved). Useful for Undo/Redo patterns with widgets that requires continuous editing. Note that you may get false positives (some widgets such as Combo()/ListBox()/Selectable() will return true even when clicking an already selected item).
			virtual bool          is_item_toggled_open() = 0;                                                // was the last item open state toggled? set by TreeNode().
			virtual bool          is_any_item_hovered() = 0;                                                 // is any item hovered?
			virtual bool          is_any_item_active() = 0;                                                  // is any item active?
			virtual bool          is_any_item_focused() = 0;                                                 // is any item focused?
			virtual Float2        get_item_rect_min() = 0;                                                   // get upper-left bounding rectangle of the last item (screen space)
			virtual Float2        get_item_rect_max() = 0;                                                   // get lower-right bounding rectangle of the last item (screen space)
			virtual Float2        get_item_rect_size() = 0;                                                  // get size of last item
			virtual void          set_item_allow_overlap() = 0;                                              // allow last item to be overlapped by a subsequent item. sometimes useful with invisible buttons, selectables, etc. to catch unused area.

			// Miscellaneous Utilities
			virtual bool          is_rect_visible(const Float2& size) = 0;                                  // test if rectangle (of given size, starting from cursor position) is visible / not clipped.
			virtual bool          is_rect_visible(const Float2& rect_min, const Float2& rect_max) = 0;      // test if rectangle (in screen space) is visible / not clipped. to perform coarse clipping on user's side.
			virtual h_draw_list_t      get_background_draw_list() = 0;                                            // this draw list will be the first rendering one. Useful to quickly draw shapes/text behind dear imgui contents.
			virtual h_draw_list_t      get_foreground_draw_list() = 0;                                            // this draw list will be the last rendered one. Useful to quickly draw shapes/text over dear imgui contents.
			virtual const char*   get_style_color_name(EColor idx) = 0;                                    // get a string corresponding to the enum value (for display, saving, etc.).
			virtual Float2        calc_text_size(const char* text, const char* text_end = nullptr, bool hide_text_after_double_hash = false, f32 wrap_width = -1.0f) = 0;
			virtual void          calc_list_clipping(i32 items_count, f32 items_height, int* out_items_display_start, int* out_items_display_end) = 0;    // calculate coarse clipping for large list of evenly sized items. Prefer using the ImGuiListClipper higher-level helper if you can.
			virtual bool          begin_child_frame(u32 id, const Float2& size, EWindowFlag flags = EWindowFlag::none) = 0; // helper to create a child window / scrolling region that looks like a normal widget frame
			virtual void          end_child_frame() = 0;                                                    // always call EndChildFrame() regardless of BeginChildFrame() return values (which indicates a collapsed/clipped window)
		
			virtual bool		  is_mouse_clicked(Input::EMouseKey key) = 0;
			virtual bool		  is_mouse_double_clicked(Input::EMouseKey key) = 0;
			virtual bool		  is_mouse_released(Input::EMouseKey key) = 0;
			virtual f32		  mouse_down_duration(Input::EMouseKey key) = 0;

			// Input Callback Data.
			virtual void input_text_callback_data_delete_chars(InputTextCallbackData* data, i32 pos, i32 bytes_count) = 0;
			virtual void input_text_callback_data_insert_chars(InputTextCallbackData* data, int pos, const char* text, const char* text_end = nullptr) = 0;

			// Extension: ImGuizmo

			//! Draws a gizmo widget. This call manipulates the draw list directly and does not change the cursor position.
			//! @param[in|out] world_matrix The transform matrix to manipulate in world space.
			//! @param[in] view The view matrix (world to camera matrix).
			//! @param[in] projection The camera projection matrix.
			//! @param[in] viewport_rect The viewport rect to render the gizmo to. Typically this should match the viewport of your scene.
			//! @param[in] operation The operation to display.
			//! @param[in] mode The mode to display.
			//! @param[in] snap If not `0.0f`, then the gizmo is manipulated in a fixed gap.
			//! @param[in] enabled If `false`, then the rendered gizmo is disabled (gray color) and cannot be manipulated.
			//! @param[in] orthographic If the camera is in orthographic mode.
			//! @param[out] delta_matrix Returns the matrix used to apply to the original matrix in world space.
			//! @param[out] is_mouse_hover Returns `true` if mouse is hovering on any part of the gizmo.
			//! @param[out] is_mouse_moving Returns `true` if mouse is dragging any part of the gizmo.
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
			) = 0;

		};
	}
}