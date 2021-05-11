// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file CommandQueue.cpp
* @author JXMaster
* @date 2019/1/30
* @brief d3d12 implementation of ICommandQueue interface.
*/
#include "CommandQueue.hpp"

#ifdef LUNA_GFX_D3D12

#include "CommandBuffer.hpp"

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			RV CommandQueue::init(const CommandQueueDesc& desc)
			{
				m_desc = desc;
				ID3D12Device* dev = m_device->m_device.Get();
				D3D12_COMMAND_QUEUE_DESC d = {};
				d.NodeMask = 0;
				switch (desc.priority)
				{
				case ECommandQueuePriority::normal:
					d.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
					break;
				case ECommandQueuePriority::high:
					d.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
					break;
				default:
					lupanic();
					break;
				}
				d.Type = encode_command_list_type(desc.type);
				if (FAILED(dev->CreateCommandQueue(&d, IID_PPV_ARGS(&m_queue))))
				{
					return BasicError::failure();
				}
				m_mtx = new_mutex();
				return RV();
			}

			RV CommandQueue::wait_command_buffer(ICommandBuffer* command_buffer)
			{
				CommandBuffer* buffer = static_cast<CommandBuffer*>(command_buffer);
				HRESULT hr = m_queue->Wait(buffer->m_fence.Get(), buffer->m_wait_value);
				return SUCCEEDED(hr) ? RV() : BasicError::bad_system_call();
			}
			RP<ICommandBuffer> CommandQueue::new_command_buffer()
			{
				P<CommandBuffer> buffer = newobj<CommandBuffer>();
				if (!buffer)
				{
					return BasicError::bad_memory_alloc();
				}
				lutry
				{
					buffer->m_device = m_device;
					buffer->m_queue = (CommandQueue*)this;
					luexp(buffer->init());
				}
				lucatchret;
				return buffer;
			}
		}
	}
}
#endif