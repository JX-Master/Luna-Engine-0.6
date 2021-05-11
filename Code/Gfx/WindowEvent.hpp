// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file WindowEvent.hpp
* @author JXMaster
* @date 2019/11/26
*/
#pragma once
#include <Core/Core.hpp>
#include <Input/KeyCode.hpp>
#include <Runtime/Math.hpp>
namespace Luna
{
	namespace Gfx
	{
		enum class EWindowDisplayMode : u32
		{
			unknown = 0,	// The display mode of the window is unknown.
			windowed = 1,	// This window is currently displayed on the screen in windowed mode.
			maximized = 2,	// This window is currently displayed on the screen in maximized or full-screen mode.
			minimized = 3,	// This window is currently displayed on the screen in minimized mode.
		};

		enum class EWindowEvent : u32
		{
			//! Called when the close (x) button of the window is clicked.
			//! Applications can pop up messages to let users confirm the operation, and call `close` to close
			//! the window manually.
			//! 
			//! The default behavior is to call `close` to close the window internally.
			//! 
			//! No parameters, the `data` is always set to `nullptr`.
			close = 1,
			//! Called when the window is being destroyed, by calling the `close` function, or when the window object
			//! is no longer referenced.
			//!
			//! No parameters, the `data` is always set to `nullptr`.
			destroy,
			//! Called when the window size or display mode is changed. The window size may change because the window is 
			//! minimized, maximized, restored, changed by API calls, or the user is resizing the window by dragging
			//! the window border.
			//! If the window is dragged by user, the `resize` event will continuously spawn and dispatch to window.
			//! Use `on_sizemove_begin` and `on_sizemove_end` to determine when the user begins and finishes dragging 
			//! the window border.
			//! 
			//! Parameter: `const WindowResize*`
			resize,
			//! Called when the window has been moved, that is, the position of the window on screen is changed.
			//! 
			//! Parameter: `const WindowMove*`
			move,
			//! Called once when the user begin to drag the window border to move or resize the window.
			//! 
			//! No parameters, the `data` is always set to `nullptr`.
			begin_sizemove,
			//! Called once when the user finished dragging the window border.
			//!
			//! No parameters, the `data` is always set to `nullptr`.
			end_sizemove,
			//! Called when the window begins to be displayed to user from hidden.
			//! All updates to this window should resume to work.
			//!
			//! No parameters, the `data` is always set to `nullptr`.
			show,
			//! Called when the window is hidden from user.
			//! All updates to this window can be paused.
			//! 
			//! No parameters, the `data` is always set to `nullptr`.
			hide,
			//! Called when one of the mouse button is pressed within the window client area.
			//! 
			//! Parameter: `const WindowMouseDown*`
			mouse_down,
			//! Called when one of the mouse button is released within the window client area.
			//! 
			//! Parameter: `const WindowMouseUp*`
			mouse_up,
			//! Called when the mouse cursor is moved and is currently on this window.
			//! 
			//! Parameter: `const WindowMouseMove*`
			mouse_move,
			//! Called when the mouse's horizontal scroll wheel is tilted or rotated and this window is currently active.
			//! 
			//! Parameter: `const WindowMouseWheel*`
			mouse_hwheel,
			//! Called when the mouse's vertical scroll wheel is tilted or rotated and this window is currently active.
			//! 
			//! Parameter: `const WindowMouseWheel*`
			mouse_wheel,
			//! Called when a keyboard key is down without system capture, and while this window is active.
			//! 
			//! Parameter: `const EKeyCode*`
			key_down,
			//! Called when a keyboard key is up without system capture, and while this window is active.
			//! 
			//! Parameter: `const EKeyCode*`
			key_up,
			//! Called when a Unicode character is inputed by the user to this window. The Unicode character may from a keyboard input,
			//! from a copy-paste, from an IME or other methods.
			//�� 
			//! Parameter: `const char32_t*`
			input_char
		};

		struct WindowMove
		{
			Int2U pos;	// The new position of the window in screen space pixels.
			Input::EMouseKey down_keys;	// A combination of mouse keys that is currently pressed.
		};

		struct WindowResize
		{
			UInt2U size;			// The new size (width, height) of the window in pixels.
			EWindowDisplayMode mode;// The new display mode of the window.
		};

		struct WindowMouseDown
		{
			Int2U pos;					// The mouse position in window space pixels when this event is triggered.
			Input::EMouseKey key;		// The mouse key that triggers this event.
			Input::EMouseKey down_keys;	// A combination of mouse keys that is currently pressed.
			bool dblclk;				// If this mouse down event is a double click. The double click is determined by system.
		};

		struct WindowMouseUp
		{
			Int2U pos;					// The mouse position in window space pixels when this event is triggered.
			Input::EMouseKey key;		// The mouse key that triggers this event.
			Input::EMouseKey down_keys;	// A combination of mouse keys that is currently pressed.
		};

		struct WindowMouseMove
		{
			Int2U pos;					// The mouse position in window space pixels when this event is triggered.
			Input::EMouseKey down_keys;	// A combination of mouse keys that is currently pressed.
		};

		struct WindowMouseWheel
		{
			Int2U pos;					// The mouse position in window space pixels when this event is triggered.
			Input::EMouseKey down_keys;	// A combination of mouse keys that is currently pressed.
			f32 delta;				// The number of spans the wheel is rotated.
		};
	}
}