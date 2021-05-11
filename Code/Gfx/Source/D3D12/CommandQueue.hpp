// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file CommandQueue.hpp
* @author JXMaster
* @date 2019/1/30
* @brief d3d12 implementation of ICommandQueue interface.
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12
#include "D3D12Common.hpp"
#include "GraphicDevice.hpp"

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			inline D3D12_COMMAND_LIST_TYPE encode_command_list_type(ECommandQueueType t)
			{
				switch (t)
				{
				case ECommandQueueType::graphic:
					return D3D12_COMMAND_LIST_TYPE_DIRECT;
					break;
				case ECommandQueueType::compute:
					return D3D12_COMMAND_LIST_TYPE_COMPUTE;
					break;
				case ECommandQueueType::copy:
					return D3D12_COMMAND_LIST_TYPE_COPY;
					break;
				default:
					lupanic();
					break;
				}
				return D3D12_COMMAND_LIST_TYPE_DIRECT;
			}

			class CommandQueue : public ICommandQueue
			{
			public:
				lucid("{681926d0-8aaf-4766-ad37-591cf6ef428b}");
				luiimpl(CommandQueue, ICommandQueue, IGraphicDeviceChild, IObject);
				
				P<GraphicDevice> m_device;
				ComPtr<ID3D12CommandQueue> m_queue;
				CommandQueueDesc m_desc;

				// Used by command buffers to execute in order.
				P<IMutex> m_mtx;

				CommandQueue(GraphicDevice* device) :
					m_device(device) {}

				RV init(const CommandQueueDesc& desc);
				virtual IGraphicDevice* get_device() override
				{
					return m_device;
				}
				virtual CommandQueueDesc desc() override
				{
					return m_desc;
				}
				virtual RV wait_command_buffer(ICommandBuffer* command_buffer) override;
				virtual RP<ICommandBuffer> new_command_buffer() override;
			};
		}
	}
}

#endif