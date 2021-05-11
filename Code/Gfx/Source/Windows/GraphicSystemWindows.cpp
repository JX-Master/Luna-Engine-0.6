// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file GraphicSystemWindows.cpp
* @author JXMaster
* @date 2019/7/17
*/

#include "GraphicSystemWindows.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment (lib, "Shcore.lib")

#include "GraphicAdapter.hpp"

#include "Window.hpp"
#include <Windowsx.h>
#include <ShellScalingApi.h>

#include <commdlg.h>

#include <ShlObj.h>

#include <d3dcompiler.h>

#include <Runtime/Unicode.hpp>

LRESULT CALLBACK luna_gfx_win_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Luna
{
	namespace Gfx
	{
		namespace Win
		{
			ComPtr<IDXGIFactory1> m_dxgi;
			Unconstructed<Vector<P<IGraphicAdapter>>> m_adapters;
			P<WindowInputDevice> m_window_input;
			HINSTANCE m_instance;

			RV init_graphic_system_windows()
			{
				// register window class.
				::SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
				m_instance = GetModuleHandleW(NULL);
				WNDCLASSEXW wcex;
				wcex.cbSize = sizeof(WNDCLASSEXW);
				wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
				wcex.lpfnWndProc = luna_gfx_win_proc;
				wcex.cbClsExtra = 0;
				wcex.cbWndExtra = sizeof(void*);
				wcex.hInstance = m_instance;
				wcex.hIcon = NULL;
				wcex.hCursor = NULL; //::LoadCursor(NULL, IDC_ARROW); We need to set the mouse cursor manually, so this must be NULL.
				wcex.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
				wcex.lpszMenuName = NULL;
				wcex.lpszClassName = L"LunaGfxWindow";
				wcex.hIconSm = NULL;
				if (!::RegisterClassExW(&wcex))
				{
					return BasicError::bad_system_call();
				}

				g_window_input.construct();
				m_window_input = box_ptr(&(g_window_input.get()));

				lutry
				{
					m_window_input->m_mtx = new_mutex();
					auto input_name = Name("window");
					luexp(Input::mount_device(input_name, m_window_input));
				}
				lucatchret;
				
				if (FAILED(::CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgi))))
				{
					return BasicError::bad_system_call();
				}

				ComPtr<IDXGIAdapter1> adapter;

				u32 i = 0;
				m_adapters.construct();
				while (SUCCEEDED(m_dxgi->EnumAdapters1(i, adapter.ReleaseAndGetAddressOf())))
				{
					P<GraphicAdapter> n = newobj<GraphicAdapter>();
					n->init(adapter.Get());
					IGraphicAdapter* ig = n.get();
					m_adapters.get().push_back(ig);
					++i;
				}
				return RV();
			}

			void deinit_graphic_system_windows()
			{
				::UnregisterClassW(L"LunaGfxWindow", m_instance);
				m_dxgi = nullptr;
				m_adapters.destruct();
				auto _ = Input::unmount_device(Name("window"));
				m_window_input = nullptr;
				m_instance = nullptr;
			}
		}

		LUNA_GFX_API RP<IWindow> new_window(const c8* title, i32 x, i32 y, i32 w, i32 h, EWindowCreationFlag window_creation_flags)
		{
			wchar_t titlew[260];
			utf8_to_utf16((char16_t*)titlew, 260, title);
			u32 style = 0;
			if ((window_creation_flags & EWindowCreationFlag::borderless) == EWindowCreationFlag::none)
			{
				// not borderless.
				style |= (WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
			}
			if ((window_creation_flags & EWindowCreationFlag::no_resize) == EWindowCreationFlag::none)
			{
				// resizable.
				style |= WS_THICKFRAME;
			}
			HWND hwnd = ::CreateWindowExW(NULL, L"LunaGfxWindow", titlew, style,
				x, y,
				w, h,
				NULL, NULL, Win::m_instance, NULL);
			if (!hwnd)
			{
				return BasicError::bad_system_call();
			}
			P<Win::Window> window = newobj<Win::Window>();
			window->m_hwnd = hwnd;
			// set user data part.
			::SetWindowLongPtrW(window->m_hwnd, GWLP_USERDATA, (LONG_PTR)window.get());
			if ((window_creation_flags & EWindowCreationFlag::hidden) == EWindowCreationFlag::none)
			{
				::ShowWindow(hwnd, SW_SHOWNORMAL);
				::UpdateWindow(hwnd);
			}
			return window;
		}
		LUNA_GFX_API RP<IGraphicAdapter> enum_adapter(u32 index)
		{
			if (index >= Win::m_adapters.get().size())
			{
				return BasicError::not_found();
			}
			return Win::m_adapters.get()[index];
		}
		LUNA_GFX_API R<Blob> compile_shader(const ShaderCompileDesc& desc)
		{
			// Validation.
			if (desc.src_type != EShaderSourceType::hlsl && desc.src_type != EShaderSourceType::platform_default)
			{
				return BasicError::not_supported();
			}
			if (desc.target_type != EShaderTargetType::dx_bytecode && desc.target_type != EShaderTargetType::platform_default)
			{
				return BasicError::not_supported();
			}
			const char* sm;
			// Create shader type.
			switch (desc.shader_model)
			{
			case EShaderModel::sm_5_0:
				switch (desc.shader_type)
				{
				case EShaderType::vertex:
					sm = "vs_5_0";
					break;
				case EShaderType::pixel:
					sm = "ps_5_0";
					break;
				case EShaderType::compute:
					sm = "cs_5_0";
					break;
				case EShaderType::geometry:
					sm = "gs_5_0";
					break;
				case EShaderType::hull:
					sm = "hs_5_0";
					break;
				case EShaderType::domain:
					sm = "ds_5_0";
					break;
				default:
					lupanic();
					break;
				}
				break;
			default:
				lupanic();
				break;
			}
			// Check flags.
			u32 flags = 0;
			if ((desc.flags & EShaderCompileFlag::debug_info) != EShaderCompileFlag::none)
			{
				flags |= D3DCOMPILE_DEBUG;
			}
			if ((desc.flags & EShaderCompileFlag::skip_validation) != EShaderCompileFlag::none)
			{
				flags |= D3DCOMPILE_SKIP_VALIDATION;
			}
			switch (desc.optimization_level)
			{
			case EShaderOptimizationLevel::none:
				flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
				break;
			case EShaderOptimizationLevel::speed:
				flags |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
				break;
			case EShaderOptimizationLevel::full:
				flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
				break;
			default:
				lupanic();
				break;
			}
			ComPtr<ID3DBlob> shader;
			ComPtr<ID3DBlob> err;
			D3D_SHADER_MACRO macros[] = { NULL, NULL };
			if (FAILED(::D3DCompile(desc.src_data, desc.src_size, desc.src_name, macros, nullptr,
				desc.entry_point, sm, flags, 0, shader.GetAddressOf(), err.GetAddressOf())))
			{
				get_error_object() = Error(BasicError::failure(), "Failed to compile shader %s: %s", desc.src_name, (const char*)err->GetBufferPointer());
				return BasicError::error_object();
			}

			Blob b;
			b.resize(shader->GetBufferSize());
			memcpy(b.data(), shader->GetBufferPointer(), b.size());
			return b;
		}

		LUNA_GFX_API R<EMessageBoxButton> message_box(const c8* text, const c8* caption, EMessageBoxType type, EMessageBoxIcon icon)
		{
			wchar_t* wtext;
			wchar_t* wcap;
			usize text_size, caption_size;
			text_size = utf8_to_utf16_len(text);
			caption_size = utf8_to_utf16_len(caption);
			wtext = (wchar_t*)alloca((text_size + 1) * sizeof(wchar_t));
			wcap = (wchar_t*)alloca((caption_size + 1) * sizeof(wchar_t));
			utf8_to_utf16((char16_t*)wtext, text_size + 1, text);
			utf8_to_utf16((char16_t*)wcap, caption_size + 1, caption);
			UINT f = 0;
			switch (type)
			{
			case EMessageBoxType::ok:
				f = MB_OK;
				break;
			case EMessageBoxType::ok_cancel:
				f = MB_OKCANCEL;
				break;
			case EMessageBoxType::retry_cancel:
				f = MB_RETRYCANCEL;
				break;
			case EMessageBoxType::yes_no:
				f = MB_YESNO;
				break;
			case EMessageBoxType::yes_no_cancel:
				f = MB_YESNOCANCEL;
				break;
			default:
				lupanic();
				break;
			}
			switch (icon)
			{
			case EMessageBoxIcon::none:
				break;
			case EMessageBoxIcon::information:
				f |= MB_ICONINFORMATION;
				break;
			case EMessageBoxIcon::warning:
				f |= MB_ICONWARNING;
				break;
			case EMessageBoxIcon::question:
				f |= MB_ICONQUESTION;
				break;
			case EMessageBoxIcon::error:
				f |= MB_ICONERROR;
				break;
			default:
				lupanic();
				break;
			}
			int ret = ::MessageBoxW(NULL, wtext, wcap, f);
			if (!ret)
			{
				return BasicError::bad_system_call();
			}
			switch (ret)
			{
			case IDOK:
				return EMessageBoxButton::ok;
			case IDNO:
				return EMessageBoxButton::no;
			case IDYES:
				return EMessageBoxButton::yes;
			case IDCANCEL:
				return EMessageBoxButton::cancel;
			case IDRETRY:
				return EMessageBoxButton::retry;
			default:
				lupanic();
			}
			return EMessageBoxButton::ok;
		}

		LUNA_GFX_API R<Vector<Path>> open_file_dialog(const c8* filter, const c8* title, const Path& initial_dir, EFileOpenDialogFlag flags)
		{
			Vector<Path> paths;
			OPENFILENAMEW ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			wchar_t out[2048];
			if (initial_dir != Path())
			{
				auto path_str = initial_dir.encode(EPathSeparator::back_slash, true);
				utf8_to_utf16((char16_t*)out, 2048, path_str.c_str());
			}
			else
			{
				out[0] = '\0';
			}

			wchar_t* wtitle = nullptr;
			if (title)
			{
				usize wt_size = utf8_to_utf16_len(title);
				wtitle = (wchar_t*)alloca(sizeof(wchar_t) * (wt_size + 1));
				utf8_to_utf16((char16_t*)wtitle, wt_size + 1, title);
			}

			const char* cur_filter = filter;
			usize filter_wsize = 0;
			while (*cur_filter)
			{
				usize len = strlen(cur_filter);
				usize wlen = utf8_to_utf16_len(cur_filter);
				filter_wsize += wlen + 1;
				cur_filter += len + 1;
			}
			wchar_t* wfilter = (wchar_t*)alloca((filter_wsize + 1) * sizeof(wchar_t));
			cur_filter = filter;
			usize wfilter_cur = 0;
			while (*cur_filter)
			{
				usize len = strlen(cur_filter);
				usize outputted = utf8_to_utf16((char16_t*)wfilter + wfilter_cur, filter_wsize - wfilter_cur + 1, cur_filter);
				cur_filter += len + 1;
				wfilter_cur += outputted + 1;
			}
			wfilter[filter_wsize] = '\0';
			ofn.lpstrFile = out;
			ofn.nMaxFile = 2048 * sizeof(wchar_t);
			ofn.lpstrFilter = wfilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.lpstrTitle = wtitle;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON;
			if ((flags & EFileOpenDialogFlag::multi_select) != EFileOpenDialogFlag::none)
			{
				ofn.Flags |= OFN_ALLOWMULTISELECT;
			}
			bool res = ::GetOpenFileNameW(&ofn) != FALSE;
			if (res)
			{
				// Checks if multiple files are selected.
				if (ofn.lpstrFile[ofn.nFileOffset - 1] == '\0')
				{
					// Multiple file.
					char* ret_buf = (char*)alloca(sizeof(char) * 1024);
					usize dir_sz = utf16_to_utf8(ret_buf, 1024, (char16_t*)ofn.lpstrFile);
					char16_t* wdir_cur = (char16_t*)ofn.lpstrFile + strlen16((char16_t*)ofn.lpstrFile) + 1;
					while (wdir_cur)
					{
						utf16_to_utf8(ret_buf + dir_sz, 1024 - dir_sz, wdir_cur);
						wdir_cur += strlen16(wdir_cur) + 1;
						auto ret_path = Path(ret_buf);
						paths.push_back(ret_path);
					}
				}
				else
				{
					// Single file.
					usize ret_sz = utf16_to_utf8_len((char16_t*)ofn.lpstrFile);
					char* ret_buf = (char*)alloca(sizeof(char) * (ret_sz + 1));
					utf16_to_utf8(ret_buf, (ret_sz + 1), (char16_t*)ofn.lpstrFile);
					auto ret_path = Path(ret_buf);
					paths.push_back(ret_path);
				}
			}
			else
			{
				DWORD err_code = CommDlgExtendedError();
				if (err_code == 0)
				{
					return BasicError::user_canceled();
				}
				get_error_object() = Error(BasicError::bad_system_call(), "Open File Dialog (common dialog box) reports failure, error code: %u", err_code);
				return BasicError::error_object();
			}

			return paths;
		}

		LUNA_GFX_API R<Path> save_file_dialog(const c8* filter, const c8* title, const Path& initial_file_path)
		{
			Path ret_path;
			// Translate filter.
			const char* cur_filter = filter;
			usize filter_wsize = 0;
			while (*cur_filter)
			{
				usize len = strlen(cur_filter);
				usize wlen = utf8_to_utf16_len(cur_filter);
				filter_wsize += wlen + 1;
				cur_filter += len + 1;
			}

			wchar_t* wtitle = nullptr;
			if (title)
			{
				usize wt_size = utf8_to_utf16_len(title);
				wtitle = (wchar_t*)alloca(sizeof(wchar_t) * (wt_size + 1));
				utf8_to_utf16((char16_t*)wtitle, wt_size + 1, title);
			}

			wchar_t* wfilter = (wchar_t*)alloca((filter_wsize + 1) * sizeof(wchar_t));
			cur_filter = filter;
			usize wfilter_cur = 0;
			while (*cur_filter)
			{
				usize len = strlen(cur_filter);
				usize outputted = utf8_to_utf16((char16_t*)wfilter + wfilter_cur, filter_wsize - wfilter_cur + 1, cur_filter);
				cur_filter += len + 1;
				wfilter_cur += outputted + 1;
			}
			wfilter[filter_wsize] = '\0';
			// Translate initial path if have.
			wchar_t out[1024];
			if (initial_file_path != Path())
			{
				auto path_str = initial_file_path.encode(EPathSeparator::back_slash, true);
				utf8_to_utf16((char16_t*)out, 1024, path_str.c_str());
			}
			else
			{
				out[0] = '\0';
			}
			OPENFILENAMEW ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = out;
			ofn.nMaxFile = 1024;
			ofn.lpstrFilter = wfilter;
			ofn.nFilterIndex = 1;
			ofn.lpstrDefExt = NULL;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.lpstrTitle = wtitle;
			ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON;

			bool res = GetSaveFileNameW(&ofn) != FALSE;

			if (!res)
			{
				DWORD err_code = CommDlgExtendedError();
				if (err_code == 0)
				{
					return BasicError::user_canceled();
				}
				get_error_object() = Error(BasicError::bad_system_call(), "Save File Dialog (common dialog box) reports failure, error code: %u", err_code);
				return BasicError::error_object();
			}

			usize ret_sz = utf16_to_utf8_len((char16_t*)ofn.lpstrFile);
			char* ret_buf = (char*)alloca(sizeof(char) * (ret_sz + 1));
			utf16_to_utf8(ret_buf, (ret_sz + 1), (char16_t*)ofn.lpstrFile);
			ret_path = Path(ret_buf);
			return ret_path;
		}

		LUNA_GFX_API R<Path> open_dir_dialog(const c8* title, const Path& initial_dir)
		{
			Path path;
			ComPtr<IFileDialog> pfd;
			if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
			{
				return BasicError::bad_system_call();
			}

			if (title)
			{
				wchar_t* wtitle = nullptr;
				usize wt_size = utf8_to_utf16_len(title);
				wtitle = (wchar_t*)alloca(sizeof(wchar_t) * (wt_size + 1));
				utf8_to_utf16((char16_t*)wtitle, wt_size + 1, title);
				pfd->SetTitle(wtitle);
			}

			if (initial_dir != Path())
			{
				PIDLIST_ABSOLUTE pidl;
				WCHAR wstarting_dir[MAX_PATH];
				auto initial_str = initial_dir.encode(EPathSeparator::back_slash, true);
				utf8_to_utf16((char16_t*)wstarting_dir, MAX_PATH, initial_str.c_str());
				HRESULT hresult = ::SHParseDisplayName(wstarting_dir, 0, &pidl, SFGAO_FOLDER, 0);
				if (FAILED(hresult))
				{
					return BasicError::bad_system_call();
				}
				ComPtr<IShellItem> psi;
				hresult = ::SHCreateShellItem(NULL, NULL, pidl, &psi);
				if (SUCCEEDED(hresult))
				{
					pfd->SetFolder(psi.Get());
				}
				ILFree(pidl);
			}

			DWORD dwOptions;
			if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
			{
				pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
			}

			if (SUCCEEDED(pfd->Show(NULL)))
			{
				ComPtr<IShellItem> psi;
				if (SUCCEEDED(pfd->GetResult(&psi)))
				{
					WCHAR* tmp;
					if (SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &tmp)))
					{
						usize path_len = utf16_to_utf8_len((char16_t*)tmp);
						char* buf = (char*)alloca(sizeof(char) * (path_len + 1));
						utf16_to_utf8(buf, path_len + 1, (char16_t*)tmp);

						path = Path(buf);
						path.flags() = (path.flags() | EPathFlag::diretory);

						CoTaskMemFree(tmp);
					}
				}
			}
			if (path == Path())
			{
				return BasicError::bad_system_call();
			}
			return path;
		}
	}
}

#endif