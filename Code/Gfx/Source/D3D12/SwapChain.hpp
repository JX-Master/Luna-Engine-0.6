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
#include <Base/TSAssert.hpp>

namespace luna
{
	namespace gfx
	{
		namespace d3d12
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
				P<win::Window> m_window;
				ComPtr<IDXGISwapChain1> m_sc;
				SwapChainDesc m_desc;

				ComPtr<ID3D12Fence> m_fence;
				HANDLE m_event;
				ComPtr<ID3D12CommandAllocator> m_ca;
				ComPtr<ID3D12GraphicsCommandList> m_li;
				ComPtr<ID3D12RootSignature> m_root_signature;
				ComPtr<ID3D12DescriptorHeap> m_rtvs;
				ComPtr<ID3D12DescriptorHeap> m_srv;
				size_t m_rtv_size;

				// Present resources, need to be reset when the swap chain is resized.
				ComPtr<ID3D12PipelineState> m_pso;
				Vector<ComPtr<ID3D12Resource>> m_back_buffers;
				uint32 m_current_back_buffer;

				SwapChain() :
					luibind(get_module_allocator()),
					m_event(NULL),
					m_back_buffers(get_module_allocator()),
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

				RV init(win::Window* window, CommandQueue* queue, const SwapChainDesc& desc);

				//! Called when the back buffer is resized or when the swap chain is initialized.
				RV reset_back_buffer_resources(uint32 buffer_count, uint32 width, uint32 height, EResourceFormat new_format);

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
						return s_ok;
					}
					if (res == WAIT_TIMEOUT)
					{
						return e_pending;
					}
					return e_bad_system_call;
				}

				virtual IWindow* bounding_window() override
				{
					return m_window;
				}

				virtual SwapChainDesc desc() override
				{
					return m_desc;
				}
				virtual RV present(IResource* resource, uint32 subresource, uint32 sync_interval) override;
				virtual RV resize_buffers(uint32 buffer_count, uint32 width, uint32 height, EResourceFormat new_format) override;
			};
		}
	}
}

#endif