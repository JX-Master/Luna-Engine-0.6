// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file GraphicSystem.cpp
* @author JXMaster
* @date 2019/7/10
*/
#include "GraphicSystem.hpp"

#ifdef LUNA_GFX_D3D12

#include "../Windows/CommonInclude.hpp"
#include <d3d12.h>
#include "GraphicDevice.hpp"
#include "../Windows/GraphicAdapter.hpp"
#include "SwapChain.hpp"
#include "../Windows/Window.hpp"
#include <dxgi1_4.h>
#pragma comment(lib, "d3d12.lib")
namespace luna
{
	namespace gfx
	{
		namespace d3d12
		{

			RV init()
			{
				result_t r = win::init_graphic_system_windows();
				if (failed(r))
				{
					return r;
				}
#ifdef LUNA_DEBUG_LEVEL_DEBUG
				ComPtr<ID3D12Debug> debug;
				D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
				debug->EnableDebugLayer();
#endif
				return s_ok;
			}
		}
		void deinit()
		{
			win::deinit_graphic_system_windows();
		}

		LUNA_GFX_API RV init()
		{
			auto r = d3d12::init();
			if (failed(r))
			{
				return r;
			}
			add_module("Gfx", deinit);
			return s_ok;
		}

		LUNA_GFX_API RP<IGraphicDevice> new_device(IGraphicAdapter* adapter)
		{
			ComPtr<ID3D12Device> dev;
			IDXGIAdapter* ada;
			if (adapter)
			{
				ada = static_cast<win::GraphicAdapter*>(adapter)->m_adapter.Get();
			}
			else
			{
				ada = nullptr;
			}
			if (FAILED(::D3D12CreateDevice(ada, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dev))))
			{
				// try warp device.
				ComPtr<IDXGIAdapter> warp;
				ComPtr<IDXGIFactory4> fac;
				win::m_dxgi.As(&fac);
				fac->EnumWarpAdapter(IID_PPV_ARGS(&warp));

				if (FAILED(D3D12CreateDevice(warp.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dev))))
				{
					set_err(e_bad_system_call, "IGraphicSystem::new_device - Failed to create D3D12 Device.");
					return e_user_failure;
				}
			}
			d3d12::GraphicDevice* device = new_obj<d3d12::GraphicDevice>();
			device->init(dev.Get());
			return box_ptr(device);
		}
		LUNA_GFX_API RP<ISwapChain> new_swap_chain(ICommandQueue* queue, IWindow* window, const SwapChainDesc& desc)
		{
			P<d3d12::SwapChain> r = box_ptr(new_obj<d3d12::SwapChain>());
			if (!r)
			{
				return e_bad_memory_alloc;
			}
			lutry
			{
				luexp(r->init(static_cast<win::Window*>(window), static_cast<d3d12::CommandQueue*>(queue), desc));
			}
			lucatchret;
			return r;
		}
	}
}

#endif