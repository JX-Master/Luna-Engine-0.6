// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SwapChain.hpp
* @author JXMaster
* @date 2019/9/20
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12

#include "D3D12Common.hpp"
#include <dxgi1_2.h>
#include "GraphicSystem.hpp"
#include "GraphicDevice.hpp"
#include "CommandQueue.hpp"
#include "../Windows/Window.hpp"
#include <Runtime/TSAssert.hpp>

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			class GraphicDevice;
			class BackBufferResource;
			class SwapChain : public ISwapChain
			{
			public:
				lucid("{067d14fa-59c7-4f66-8fb0-1981d90a5a45}");
				luqbegin();
				luqitem(this, SwapChain, ISwapChain, IWaitable);
				luqitem((IGraphicDeviceChild*)this, IGraphicDeviceChild, IObject);
				luqend();
				lurc();
				lutsassert_lock();

				P<GraphicDevice> m_device;
				P<CommandQueue> m_queue;
				P<Win::Window> m_window;
				ComPtr<IDXGISwapChain1> m_sc;
				SwapChainDesc m_desc;

				ComPtr<ID3D12Fence> m_fence;
				HANDLE m_event;
				ComPtr<ID3D12CommandAllocator> m_ca;
				ComPtr<ID3D12GraphicsCommandList> m_li;
				ComPtr<ID3D12RootSignature> m_root_signature;
				ComPtr<ID3D12DescriptorHeap> m_rtvs;
				ComPtr<ID3D12DescriptorHeap> m_srv;
				usize m_rtv_size;

				// Present resources, need to be reset when the swap chain is resized.
				ComPtr<ID3D12PipelineState> m_pso;
				Vector<ComPtr<ID3D12Resource>> m_back_buffers;
				u32 m_current_back_buffer;

				SwapChain() :
					m_event(NULL),
					m_current_back_buffer(0) {}

				~SwapChain()
				{
					if (m_event)
					{
						::CloseHandle(m_event);
						m_event = NULL;
					}
				}

				//! Initializes all resources stored in device, which is shared between all swap chains for the same device.
				RV init_shared_res();

				RV init(Win::Window* window, CommandQueue* queue, const SwapChainDesc& desc);

				//! Called when the back buffer is resized or when the swap chain is initialized.
				RV reset_back_buffer_resources(u32 buffer_count, u32 width, u32 height, EResourceFormat new_format);

				virtual IGraphicDevice* get_device() override
				{
					return m_device;
				}

				virtual void wait() override
				{
					DWORD res = ::WaitForSingleObject(m_event, INFINITE);
					if (res != WAIT_OBJECT_0)
					{
						lupanic_msg_always("WaitForSingleObject failed.");
					}
				}
				virtual RV try_wait() override
				{
					DWORD res = ::WaitForSingleObject(m_event, 0);
					if (res == WAIT_OBJECT_0)
					{
						return RV();
					}
					if (res == WAIT_TIMEOUT)
					{
						return BasicError::timeout();
					}
					return BasicError::bad_system_call();
				}

				virtual IWindow* bounding_window() override
				{
					return m_window;
				}

				virtual SwapChainDesc desc() override
				{
					return m_desc;
				}
				virtual RV present(IResource* resource, u32 subresource, u32 sync_interval) override;
				virtual RV resize_buffers(u32 buffer_count, u32 width, u32 height, EResourceFormat new_format) override;
			};
		}
	}
}

#endif