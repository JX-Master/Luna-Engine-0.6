// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RenderSystem.cpp
* @author JXMaster
* @date 2020/3/9
*/
#include "RenderSystem.hpp"

namespace luna
{
	namespace renderer
	{
		using namespace gfx;

		P<gfx::IGraphicDevice> m_main_device;
		P<gfx::ICommandQueue> m_main_graphic_queue;
		P<gfx::ICommandQueue> m_main_compute_queue;
		P<gfx::ICommandQueue> m_main_copy_queue;

		LUNA_RENDERER_API RV init(const RenderSystemStartupParams* params)
		{
			lutry
			{
				if (params)
				{
					luset(m_main_device, new_device(params->adapter));
				}
				else
				{
					luset(m_main_device, new_device());
				}
				luset(m_main_graphic_queue, m_main_device->new_command_queue(CommandQueueDesc(ECommandQueueType::graphic, ECommandQueuePriority::normal)));
				luset(m_main_compute_queue, m_main_device->new_command_queue(CommandQueueDesc(ECommandQueueType::compute, ECommandQueuePriority::normal)));
				luset(m_main_copy_queue, m_main_device->new_command_queue(CommandQueueDesc(ECommandQueueType::copy, ECommandQueuePriority::normal)));
				add_module("Renderer", deinit);
			}
			lucatchret;
			return s_ok;
		}
		void deinit()
		{
			m_main_copy_queue = nullptr;
			m_main_compute_queue = nullptr;
			m_main_graphic_queue = nullptr;
			m_main_device = nullptr;
		}
		LUNA_RENDERER_API gfx::IGraphicDevice* device()
		{
			return m_main_device.get();
		}
		LUNA_RENDERER_API gfx::ICommandQueue* main_graphic_queue()
		{
			return m_main_graphic_queue.get();
		}
		LUNA_RENDERER_API gfx::ICommandQueue* main_compute_queue()
		{
			return m_main_compute_queue.get();
		}
		LUNA_RENDERER_API gfx::ICommandQueue* main_copy_queue()
		{
			return m_main_copy_queue.get();
		}
	}
}