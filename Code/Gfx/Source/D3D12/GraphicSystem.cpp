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
#include <Runtime/Module.hpp>
#pragma comment(lib, "d3d12.lib")
namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{

			RV init()
			{
				RV r = Win::init_graphic_system_windows();
				if (!r.valid())
				{
					return r.errcode();
				}
#ifdef LUNA_DEBUG_LEVEL_DEBUG
				ComPtr<ID3D12Debug> debug;
				D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
				debug->EnableDebugLayer();
#endif
				return RV();
			}
		}
		void deinit()
		{
			Win::deinit_graphic_system_windows();
		}

		RV init()
		{
			auto r = D3D12::init();
			if (!r.valid())
			{
				return r.errcode();
			}
			return RV();
		}

		StaticRegisterModule m("Gfx", "Core;Input", init, deinit);

		LUNA_GFX_API RP<IGraphicDevice> new_device(IGraphicAdapter* adapter)
		{
			ComPtr<ID3D12Device> dev;
			IDXGIAdapter* ada;
			if (adapter)
			{
				ada = static_cast<Win::GraphicAdapter*>(adapter)->m_adapter.Get();
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
				Win::m_dxgi.As(&fac);
				fac->EnumWarpAdapter(IID_PPV_ARGS(&warp));

				if (FAILED(D3D12CreateDevice(warp.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dev))))
				{
					get_error_object() = Error(BasicError::bad_system_call(), "IGraphicSystem::new_device - Failed to create D3D12 Device.");
					return BasicError::error_object();
				}
			}
			P<D3D12::GraphicDevice> device = newobj<D3D12::GraphicDevice>();
			device->init(dev.Get());
			return device;
		}
		LUNA_GFX_API RP<ISwapChain> new_swap_chain(ICommandQueue* queue, IWindow* window, const SwapChainDesc& desc)
		{
			P<D3D12::SwapChain> r = newobj<D3D12::SwapChain>();
			lutry
			{
				luexp(r->init(static_cast<Win::Window*>(window), static_cast<D3D12::CommandQueue*>(queue), desc));
			}
			lucatchret;
			return r;
		}
	}
}

#endif