// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Window.hpp
* @author JXMaster
* @date 2019/7/11
*/
#pragma once

#include "../GfxHeader.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
#include <Base/Vector.hpp>
namespace luna
{
	namespace gfx
	{
		namespace win
		{
			//! The IWindow implementation on Windows.
			class Window : public IWindow
			{
			public:
				lucid("{2dc23fc9-ffc1-4aa9-9c05-72018da96e50}");
				luiimpl(Window, IWindow, IObject);

				HWND m_hwnd;
				Vector<WP<IWindowEventListener>> m_listeners;
				P<IMutex> m_mtx;
				EMouseCursorShape m_mouse_cursor_shape;
				bool m_first_show;

				Window() :
					luibind(get_module_allocator()),
					m_listeners(get_module_allocator()),
					m_hwnd(nullptr),
					m_mouse_cursor_shape(EMouseCursorShape::arrow),
					m_first_show(true) 
				{
					m_mtx = new_mutex().get();
				}

				~Window()
				{
					auto _ = close();
				}

				void set_mouse_cursor_shape_internal(EMouseCursorShape shape);

				virtual RV close() override;

				virtual bool closed() override
				{
					return m_hwnd ? false : true;
				}

				virtual RV set_display_mode(EWindowDisplayMode display_state) override;

				virtual EWindowDisplayMode display_mode() override;

				virtual RV set_visible(bool visibile) override;

				virtual bool visible() override;

				virtual UInt2U size() override;

				virtual UInt2U dpi_scaled_size() override;

				virtual float32 dpi_scale_factor() override;

				virtual RV set_size(uint32 width, uint32 height) override;

				virtual Int2U position() override;

				virtual RV set_position(int32 x, int32 y) override;

				virtual EMouseCursorShape mouse_cursor_shape() override
				{
					return m_mouse_cursor_shape;
				}

				virtual RV set_mouse_cursor_shape(EMouseCursorShape cursor_shape) override
				{
					if (m_mouse_cursor_shape != cursor_shape)
					{
						m_mouse_cursor_shape = cursor_shape;
						set_mouse_cursor_shape_internal(cursor_shape);
					}
					return s_ok;
				}
			};
		}
	}
}

#endif