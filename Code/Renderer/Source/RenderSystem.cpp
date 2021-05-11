// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RenderSystem.cpp
* @author JXMaster
* @date 2020/3/9
*/
#include "RenderSystem.hpp"
#include <Runtime/Module.hpp>

namespace Luna
{
	namespace Renderer
	{
		using namespace Gfx;

		P<Gfx::IGraphicDevice> m_main_device;
		P<Gfx::ICommandQueue> m_main_graphic_queue;
		P<Gfx::ICommandQueue> m_main_compute_queue;
		P<Gfx::ICommandQueue> m_main_copy_queue;

		RV init()
		{
			lutry
			{
				luset(m_main_device, new_device());
				luset(m_main_graphic_queue, m_main_device->new_command_queue(CommandQueueDesc(ECommandQueueType::graphic, ECommandQueuePriority::normal)));
				luset(m_main_compute_queue, m_main_device->new_command_queue(CommandQueueDesc(ECommandQueueType::compute, ECommandQueuePriority::normal)));
				luset(m_main_copy_queue, m_main_device->new_command_queue(CommandQueueDesc(ECommandQueueType::copy, ECommandQueuePriority::normal)));
			}
			lucatchret;
			return RV();
		}
		void deinit()
		{
			m_main_copy_queue = nullptr;
			m_main_compute_queue = nullptr;
			m_main_graphic_queue = nullptr;
			m_main_device = nullptr;
		}
		StaticRegisterModule m(u8"Renderer", u8"Core;Gfx", init, deinit);
		LUNA_RENDERER_API Gfx::IGraphicDevice* device()
		{
			return m_main_device.get();
		}
		LUNA_RENDERER_API Gfx::ICommandQueue* main_graphic_queue()
		{
			return m_main_graphic_queue.get();
		}
		LUNA_RENDERER_API Gfx::ICommandQueue* main_compute_queue()
		{
			return m_main_compute_queue.get();
		}
		LUNA_RENDERER_API Gfx::ICommandQueue* main_copy_queue()
		{
			return m_main_copy_queue.get();
		}
	}
}