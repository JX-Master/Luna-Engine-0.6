// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file WindowInputDevice.cpp
* @author JXMaster
* @date 2020/2/10
*/
#include "WindowInputDevice.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include "Window.hpp"
#include <Windowsx.h>

namespace luna
{
	namespace gfx
	{
		namespace win
		{
			void WindowInputDevice::remove_invalid_listeners()
			{
				auto iter = m_listeners.begin();
				while (iter != m_listeners.end())
				{
					if (iter->empty())
					{
						iter = m_listeners.erase(iter);
					}
					else
					{
						++iter;
					}
				}
			}

			bool WindowInputDevice::dispatch_event(IWindow* window, EWindowEvent e, void* data)
			{
				bool ret = false;
				for (auto& i : m_listeners)
				{
					auto ptr = i.lock();
					if (ptr)
					{
						bool r = ptr->on_window_event(window, e, data);
						ret |= r;
					}
				}
				return ret;
			}

			void WindowInputDevice::update()
			{
				MutexGuard g(m_mtx);
				remove_invalid_listeners();
				MSG msg;
				bool r = false;
				while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
				{
					r = true;
					::TranslateMessage(&msg);
					::DispatchMessageW(&msg);
				}
			}

			void WindowInputDevice::add_listener(IWindowEventListener* listener)
			{
				MutexGuard g(m_mtx);
				m_listeners.push_back(makewp(listener));
			}

			RV WindowInputDevice::remove_listener(IWindowEventListener* listener)
			{
				MutexGuard g(m_mtx);
				for (auto iter = m_listeners.begin(); iter != m_listeners.end(); ++iter)
				{
					if (*iter == listener)
					{
						m_listeners.erase(iter);
						return s_ok;
					}
				}
				return e_item_not_exist;
			}

			Unconstructed<WindowInputDevice> g_window_input;
		}
	}
}

using namespace luna;
using namespace luna::gfx;
using namespace luna::gfx::win;
using namespace luna::input;

static EMouseKey encode_down_keys(WPARAM wParam)
{
	EMouseKey down_keys = EMouseKey::none;
	if (wParam & MK_LBUTTON)
	{
		down_keys |= EMouseKey::lb;
	}
	if (wParam & MK_RBUTTON)
	{
		down_keys |= EMouseKey::rb;
	}
	if (wParam & MK_MBUTTON)
	{
		down_keys |= EMouseKey::mb;
	}
	if (wParam & MK_XBUTTON1)
	{
		down_keys |= EMouseKey::x1b;
	}
	if (wParam & MK_XBUTTON2)
	{
		down_keys |= EMouseKey::x2b;
	}
	return down_keys;
}

LRESULT CALLBACK luna_gfx_win_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* pw = (Window*)::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	WindowInputDevice& in = g_window_input.get();
	switch (msg)
	{
	case WM_CLOSE:
		if (in.dispatch_event(pw, EWindowEvent::close, nullptr))
		{
			return 0;
		}
		break;
	case WM_DESTROY:
		in.dispatch_event(pw, EWindowEvent::destroy, nullptr);
		pw->m_hwnd = nullptr;
		break;
	case WM_SIZE:
		if (wParam == SIZE_MAXHIDE)
		{
			if (in.dispatch_event(pw, EWindowEvent::hide, nullptr))
			{
				return 0;
			}
		}
		else if (wParam == SIZE_MAXSHOW)
		{
			if (in.dispatch_event(pw, EWindowEvent::show, nullptr))
			{
				return 0;
			}
		}
		else
		{
			uint16 width = (uint16)LOWORD(lParam);
			uint16 height = (uint16)HIWORD(lParam);
			EWindowDisplayMode mode;
			switch (wParam)
			{
			case SIZE_MAXIMIZED:
				mode = EWindowDisplayMode::maximized;
				break;
			case SIZE_MINIMIZED:
				mode = EWindowDisplayMode::minimized;
				break;
			case SIZE_RESTORED:
				mode = EWindowDisplayMode::windowed;
				break;
			default:
				lupanic();
				break;
			}
			WindowResize resize;
			resize.mode = mode;
			resize.size.x = width;
			resize.size.y = height;
			if (in.dispatch_event(pw, EWindowEvent::resize, &resize))
			{
				return 0;
			}
		}
		break;
	case WM_MOVE:
	{
		int16 x = (int16)LOWORD(lParam);   // horizontal position 
		int16 y = (int16)HIWORD(lParam);   // vertical position 
		WindowMove move;
		move.pos.x = x;
		move.pos.y = y;
		if (in.dispatch_event(pw, EWindowEvent::move, &move))
		{
			return 0;
		}
	}
	break;
	case WM_ENTERSIZEMOVE:
		if (in.dispatch_event(pw, EWindowEvent::begin_sizemove, nullptr))
		{
			return 0;
		}
		break;
	case WM_EXITSIZEMOVE:
		if (in.dispatch_event(pw, EWindowEvent::end_sizemove, nullptr))
		{
			return 0;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	{
		bool dblclk;
		if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN)
		{
			dblclk = false;
		}
		else
		{
			dblclk = true;
		}
		EMouseKey down_keys = encode_down_keys(wParam);
		int32 x = GET_X_LPARAM(lParam);
		int32 y = GET_Y_LPARAM(lParam);
		EMouseKey dk;
		if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
		{
			dk = EMouseKey::lb;
		}
		else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK)
		{
			dk = EMouseKey::rb;
		}
		else
		{
			dk = EMouseKey::mb;
		}
		WindowMouseDown md;
		md.dblclk = dblclk;
		md.down_keys = down_keys;
		md.key = dk;
		md.pos.x = x;
		md.pos.y = y;
		if (in.dispatch_event(pw, EWindowEvent::mouse_down, &md))
		{
			return 0;
		}
	}
	break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONDBLCLK:
	{
		bool dblclk = (msg == WM_XBUTTONDOWN) ? false : true;
		uint32 button = (uint32)GET_XBUTTON_WPARAM(wParam);
		EMouseKey down_keys = encode_down_keys(GET_KEYSTATE_WPARAM(wParam));
		int32 x = GET_X_LPARAM(lParam);
		int32 y = GET_Y_LPARAM(lParam);
		EMouseKey dk;
		if (button & XBUTTON1)
		{
			dk = EMouseKey::x1b;
		}
		else if (button & XBUTTON2)
		{
			dk = EMouseKey::x2b;
		}
		else
		{
			lupanic_msg("Unknown button.");
		}
		WindowMouseDown md;
		md.dblclk = dblclk;
		md.down_keys = down_keys;
		md.key = dk;
		md.pos.x = x;
		md.pos.y = y;
		if (in.dispatch_event(pw, EWindowEvent::mouse_down, &md))
		{
			return 0;
		}
	}
	break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	{
		EMouseKey down_keys = encode_down_keys(wParam);
		int32 x = GET_X_LPARAM(lParam);
		int32 y = GET_Y_LPARAM(lParam);
		EMouseKey dk;
		if (msg == WM_LBUTTONUP)
		{
			dk = EMouseKey::lb;
		}
		else if (msg == WM_RBUTTONUP)
		{
			dk = EMouseKey::rb;
		}
		else
		{
			dk = EMouseKey::mb;
		}
		WindowMouseUp mu;
		mu.down_keys = down_keys;
		mu.key = dk;
		mu.pos.x = x;
		mu.pos.y = y;
		if (in.dispatch_event(pw, EWindowEvent::mouse_up, &mu))
		{
			return 0;
		}
	}
	break;
	case WM_XBUTTONUP:
	{
		EMouseKey down_keys = encode_down_keys(GET_KEYSTATE_WPARAM(wParam));
		uint32 button = (uint32)GET_XBUTTON_WPARAM(wParam);
		int32 x = GET_X_LPARAM(lParam);
		int32 y = GET_Y_LPARAM(lParam);
		EMouseKey dk;
		if (button & XBUTTON1)
		{
			dk = EMouseKey::x1b;
		}
		else if (button & XBUTTON2)
		{
			dk = EMouseKey::x2b;
		}
		else
		{
			lupanic_msg("Unknown button.");
		}
		WindowMouseUp mu;
		mu.down_keys = down_keys;
		mu.key = dk;
		mu.pos.x = x;
		mu.pos.y = y;
		if (in.dispatch_event(pw, EWindowEvent::mouse_up, &mu))
		{
			return 0;
		}
	}
	break;
	case WM_MOUSEMOVE:
	{
		EMouseKey down_keys = encode_down_keys(wParam);
		int32 x = GET_X_LPARAM(lParam);
		int32 y = GET_Y_LPARAM(lParam);
		WindowMove mv;
		mv.down_keys = down_keys;
		mv.pos.x = x;
		mv.pos.y = y;
		if (in.dispatch_event(pw, EWindowEvent::mouse_move, &mv))
		{
			return 0;
		}
	}
	break;
	case WM_MOUSEHWHEEL:
	case WM_MOUSEWHEEL:
	{
		EMouseKey down_keys = encode_down_keys(GET_KEYSTATE_WPARAM(wParam));
		int16 delta = GET_WHEEL_DELTA_WPARAM(wParam);
		int32 x = GET_X_LPARAM(lParam);
		int32 y = GET_Y_LPARAM(lParam);
		float32 fd = (float32)delta / (float32)WHEEL_DELTA;
		EWindowEvent e;
		if (msg == WM_MOUSEWHEEL)
		{
			e = EWindowEvent::mouse_wheel;
		}
		else
		{
			e = EWindowEvent::mouse_hwheel;
		}
		WindowMouseWheel wh;
		wh.pos.x = x;
		wh.pos.y = y;
		wh.down_keys = down_keys;
		wh.delta = fd;
		if (in.dispatch_event(pw, e, &wh))
		{
			return 0;
		}
	}
	break;
	case WM_KEYDOWN:
	{
		input::EKeyCode key = (input::EKeyCode)wParam;
		if (in.dispatch_event(pw, EWindowEvent::key_down, &key))
		{
			return 0;
		}
	}
	break;
	case WM_KEYUP:
	{
		input::EKeyCode key = (input::EKeyCode)wParam;
		if (in.dispatch_event(pw, EWindowEvent::key_up, &key))
		{
			return 0;
		}
	}
	break;
	case WM_CHAR:
	{
		char32_t c = (char32_t)wParam;
		if (in.dispatch_event(pw, EWindowEvent::input_char, &c))
		{
			return 0;
		}
	}
	break;
	case WM_SETCURSOR:
	{
		if (LOWORD(lParam) == HTCLIENT)
		{
			pw->set_mouse_cursor_shape_internal(pw->mouse_cursor_shape());
			return 1;
		}
	}
	break;
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

#endif