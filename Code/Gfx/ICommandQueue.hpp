// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ICommandQueue.hpp
* @author JXMaster
* @date 2019/8/25
*/
#pragma once
#include "GraphicDefines.hpp"
#include "ICommandBuffer.hpp"

namespace Luna
{
	namespace Gfx
	{

		enum class ECommandQueuePriority : u32
		{
			normal,
			high,
		};

		struct CommandQueueDesc
		{
			ECommandQueueType type;
			ECommandQueuePriority priority;

			CommandQueueDesc() = default;
			CommandQueueDesc(
				ECommandQueueType _type,
				ECommandQueuePriority _priority) :
				type(_type),
				priority(_priority) {}
		};

		//! @interface ICommandQueue
		//! @threadsafe
		struct ICommandQueue : public IGraphicDeviceChild
		{
			luiid("{7d6f857a-daa4-40ee-b635-90d251a58b78}");

			virtual CommandQueueDesc desc() = 0;

			//! Tells the command queue to wait for the specified command buffer being finished by another
			//! command queue before proceeding to the next submitted command buffer.
			//! 
			//! The command buffer can be in recording state or submitted state. You should never waits for a 
			//! command buffer that is submitted to the same command queue that is waiting for the command buffer
			//! to prevent dead lock on the queue.
			virtual RV wait_command_buffer(ICommandBuffer* command_buffer) = 0;

			//! Creates a new command buffer that is bound on this queue.
			virtual RP<ICommandBuffer> new_command_buffer() = 0;
		};
	}
}