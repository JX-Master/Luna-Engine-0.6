// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IWindow.hpp
* @author JXMaster
* @date 2019/7/10
*/
#pragma once
#include "IWindowEventListener.hpp"
namespace luna
{
	namespace gfx
	{
		struct IWindow;

		enum class EMouseCursorShape : uint32
		{
			none = 0,					// Do not display and cursor shape in this window.
			arrow,
			text_input,
			resize_all,
			resize_we,
			resize_ns,
			resize_nesw,
			resize_nwse,
			hand,
			not_allowed
		};

		//! @interface IWindow
		//! @threadsafe
		//! Represents a system window that can be used to display user interface and can be drawn as surface.
		struct IWindow : public IObject
		{
			luiid("{234f4d10-340a-4633-9acc-d70d61f44d23}");

			//! Close the window and release all underlying resources for the window.
			//! All methods of the window will be invalid after the window is closed.
			//! The registered callbacks will be cleared after the window is closed.
			//! @return `s_ok` if the window is successfully close, `e_bad_calling_time`
			//! if the window is already closed before this call.
			virtual RV close() = 0;

			//! Check if the window has been closed.
			//! @return `true` if the window has been closed, `false` otherwise.
			virtual bool closed() = 0;

			//! set the window display mode.
			//! @return `s_ok` if the window's display mode is set to the specified value
			//! after the call. `e_bad_calling_time` if the window is already closed before this call.
			virtual RV set_display_mode(EWindowDisplayMode display_state) = 0;

			//! Get the display mode of the window.
			//! @return The display mode of the window. If the window is closed, returns 
			//! `EWindowDisplayMode::unknown` 
			virtual EWindowDisplayMode display_mode() = 0;

			//! Show or hide the window.
			//! @param[in] `true` if you want to display the window, or `hide` if you want to 
			//! hide the window.
			//! @return `s_ok` if the window's visibility is set to the specified value
			//! after the call. `e_bad_calling_time` if the window is already closed before this call.
			virtual RV set_visible(bool visible) = 0;

			//! Get the visibility of the window.
			//! @return `true` if the window is currently showing on the screen, `false` otherwise.
			virtual bool visible() = 0;

			//! Gets the native (unscaled) size of the window.
			virtual UInt2U size() = 0;

			//! Gets the dpi-scaled size of the window. This size is usually used for positioning widgets on screen.
			//! @remark In a windowing system that supports high-DPI scaling, there will be two sizing system
			//! for representing the window size and position. The native size is the number of pixels that a window actually
			//! covers on the screen, and the scaled size is the size that is used to draw items in the window so their size 
			//! can be consistent across multiple monitors with different DPI. For example, if a 29-inch 4K monitor uses scale
			//! factor 150%, then the native size for a full-screen window is 3840x2160, and the scaled size is 2560x1440.
			//! 
			//! Generally, you should stick to the dpi-scaled size if you need to draw some window size related items. The only exception
			//! is when creating window size buffers like swap chain back-buffers and full-screen buffers, in such cases you should use
			//! native screen size to archive pixel perfect results.
			//! 
			//! If the system does not support DPI scaling, the native size and the scaled size will always be equal.
			virtual UInt2U dpi_scaled_size() = 0;

			//! Get the scale factor of the window, which is native_size / size.
			//! Using this value instead of calculating native_size / size manually, since the size is represented in integers and may
			//! result in inaccuracy.
			virtual float32 dpi_scale_factor() = 0;

			//! Set scaled the size of the window.
			virtual RV set_size(uint32 width, uint32 height) = 0;

			//! Get the scaled position of the window relative to screen origin.
			virtual Int2U position() = 0;

			//! Set the scaled position of the window relative to screen origin.
			virtual RV set_position(int32 x, int32 y) = 0;

			//! Gets the mouse cursor shape in this window. The default cursor shape is arrow.
			//! Always return `EMouseCursorShape::none` on platforms that do not have the mouse cursor.
			virtual EMouseCursorShape mouse_cursor_shape() = 0;

			//! Sets the mouse cursor shape in this window. 
			//! This function returns `e_not_available` on platforms that do not have mouse cursor. 
			virtual RV set_mouse_cursor_shape(EMouseCursorShape cursor_shape) = 0;
		};
	}
}