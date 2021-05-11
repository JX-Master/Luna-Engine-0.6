// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Window.cpp
* @author JXMaster
* @date 2019/7/11
*/
#include "Window.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include <Runtime/Unicode.hpp>

namespace Luna
{
	namespace Gfx
	{
		namespace Win
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
					return BasicError::bad_calling_time();
				}
				MutexGuard g(m_mtx);
				::DestroyWindow(m_hwnd);
				m_hwnd = nullptr;
				m_listeners.clear();
				return RV();
			}
			RV Window::set_display_mode(EWindowDisplayMode display_state)
			{
				if (!m_hwnd)
				{
					return BasicError::bad_calling_time();
				}
				switch (display_state)
				{
				case Luna::Gfx::EWindowDisplayMode::windowed:
					::ShowWindow(m_hwnd, SW_RESTORE);
					break;
				case Luna::Gfx::EWindowDisplayMode::maximized:
					::ShowWindow(m_hwnd, SW_MAXIMIZE);
					break;
				case Luna::Gfx::EWindowDisplayMode::minimized:
					::ShowWindow(m_hwnd, SW_MINIMIZE);
					break;
				default:
					lupanic();
					break;
				}
				return RV();
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
					return BasicError::bad_calling_time();
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
				return RV();
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
				f32 scaled = dpi_scale_factor();
				ret.x = (u32)(ret.x / scaled);
				ret.y = (u32)(ret.y / scaled);
				return ret;
			}
			f32 Window::dpi_scale_factor()
			{
				//HWND hd = GetDesktopWindow();
				int zoom = GetDpiForWindow(m_hwnd);
				f32 scaled = (f32)zoom / 96.0f;
				return scaled;
			}
			RV Window::set_size(u32 width, u32 height)
			{
				RECT r;
				::GetClientRect(m_hwnd, &r);
				if (::MoveWindow(m_hwnd, r.left, r.top, width, height, TRUE))
				{
					return RV();
				}
				return BasicError::failure();
			}
			String Window::title()
			{
				wchar_t buf[260];
				u32 buf_sz = ::GetWindowTextW(m_hwnd, buf, 260);
				buf[buf_sz] = 0;
				u32 u8_sz = (u32)utf16_to_utf8_len((c16*)buf);
				String r;
				r.resize(u8_sz, '\0');
				utf16_to_utf8(r.data(), r.size() + 1, (c16*)buf);
				return r;
			}
			RV Window::set_title(const c8* title_str)
			{
				wchar_t buf[260];
				utf8_to_utf16((c16*)buf, 260, title_str);
				BOOL r = ::SetWindowTextW(m_hwnd, buf);
				return r ? RV() : BasicError::bad_system_call();
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
			RV Window::set_position(i32 x, i32 y)
			{
				RECT r;
				::GetClientRect(m_hwnd, &r);
				if (::MoveWindow(m_hwnd, x, y, r.right - r.left, r.bottom - r.top, FALSE))
				{
					return RV();
				}
				return BasicError::failure();
			}
		}
	}
}

#endif