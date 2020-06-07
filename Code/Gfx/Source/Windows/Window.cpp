// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Window.cpp
* @author JXMaster
* @date 2019/7/11
*/
#include "Window.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

namespace luna
{
	namespace gfx
	{
		namespace win
		{
			void Window::set_mouse_cursor_shape_internal(EMouseCursorShape shape)
			{
				if (shape == EMouseCursorShape::none)
				{
					::SetCursor(NULL);
				}
				else
				{
					LPTSTR win32_cursor = IDC_ARROW;
					switch (shape)
					{
					case EMouseCursorShape::arrow:
						win32_cursor = IDC_ARROW; break;
					case EMouseCursorShape::text_input:
						win32_cursor = IDC_IBEAM; break;
					case EMouseCursorShape::resize_all:
						win32_cursor = IDC_SIZEALL; break;
					case EMouseCursorShape::resize_we:
						win32_cursor = IDC_SIZEWE; break;
					case EMouseCursorShape::resize_ns:
						win32_cursor = IDC_SIZENS; break;
					case EMouseCursorShape::resize_nesw:
						win32_cursor = IDC_SIZENESW; break;
					case EMouseCursorShape::resize_nwse:
						win32_cursor = IDC_SIZENWSE; break;
					case EMouseCursorShape::hand:
						win32_cursor = IDC_HAND; break;
					case EMouseCursorShape::not_allowed:
						win32_cursor = IDC_NO; break;
					default:
						lupanic();
					}
					::SetCursor(::LoadCursor(NULL, win32_cursor));
				}
			}

			RV Window::close()
			{
				if (!m_hwnd)
				{
					return e_bad_calling_time;
				}
				MutexGuard g(m_mtx);
				::DestroyWindow(m_hwnd);
				m_hwnd = nullptr;
				m_listeners.clear();
				return s_ok;
			}
			RV Window::set_display_mode(EWindowDisplayMode display_state)
			{
				if (!m_hwnd)
				{
					return e_bad_calling_time;
				}
				switch (display_state)
				{
				case luna::gfx::EWindowDisplayMode::windowed:
					::ShowWindow(m_hwnd, SW_RESTORE);
					break;
				case luna::gfx::EWindowDisplayMode::maximized:
					::ShowWindow(m_hwnd, SW_MAXIMIZE);
					break;
				case luna::gfx::EWindowDisplayMode::minimized:
					::ShowWindow(m_hwnd, SW_MINIMIZE);
					break;
				default:
					lupanic();
					break;
				}
				return s_ok;
			}
			EWindowDisplayMode Window::display_mode()
			{
				if (!m_hwnd)
				{
					return EWindowDisplayMode::unknown;
				}
				if (::IsIconic(m_hwnd))
				{
					return EWindowDisplayMode::minimized;
				}
				if (::IsZoomed(m_hwnd))
				{
					return EWindowDisplayMode::maximized;
				}
				return EWindowDisplayMode::windowed;
			}

			RV Window::set_visible(bool visibile)
			{
				if (!m_hwnd)
				{
					return e_bad_calling_time;
				}
				if (visibile)
				{
					if (m_first_show)
					{
						::ShowWindow(m_hwnd, SW_SHOWNORMAL);
						m_first_show = false;
					}
					else
					{
						::ShowWindow(m_hwnd, SW_SHOWNA);
					}
				}
				else
				{
					::ShowWindow(m_hwnd, SW_HIDE);
				}
				return s_ok;
			}

			bool Window::visible()
			{
				if (!m_hwnd)
				{
					return false;
				}
				return ::IsWindowVisible(m_hwnd) ? true : false;
			}
			UInt2U Window::size()
			{
				RECT r;
				::GetClientRect(m_hwnd, &r);
				UInt2U ret;
				ret.x = r.right - r.left;
				ret.y = r.bottom - r.top;
				return ret;
			}
			UInt2U Window::dpi_scaled_size()
			{
				RECT r;
				::GetClientRect(m_hwnd, &r);
				UInt2U ret;
				ret.x = r.right - r.left;
				ret.y = r.bottom - r.top;
				// Get system DPI.
				float32 scaled = dpi_scale_factor();
				ret.x = (uint32)(ret.x / scaled);
				ret.y = (uint32)(ret.y / scaled);
				return ret;
			}
			float32 Window::dpi_scale_factor()
			{
				//HWND hd = GetDesktopWindow();
				int zoom = GetDpiForWindow(m_hwnd);
				float32 scaled = (float32)zoom / 96.0f;
				return scaled;
			}
			RV Window::set_size(uint32 width, uint32 height)
			{
				RECT r;
				::GetClientRect(m_hwnd, &r);
				if (::MoveWindow(m_hwnd, r.left, r.top, width, height, TRUE))
				{
					return s_ok;
				}
				return e_failure;
			}
			Int2U Window::position()
			{
				POINT p = { 0, 0 };
				::ClientToScreen(m_hwnd, &p);
				Int2U ret;
				ret.x = p.x;
				ret.y = p.y;
				return ret;
			}
			RV Window::set_position(int32 x, int32 y)
			{
				RECT r;
				::GetClientRect(m_hwnd, &r);
				if (::MoveWindow(m_hwnd, x, y, r.right - r.left, r.bottom - r.top, FALSE))
				{
					return s_ok;
				}
				return e_failure;
			}
		}
	}
}

#endif