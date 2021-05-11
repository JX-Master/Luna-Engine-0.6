// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IGraphicAdapter.hpp
* @author JXMaster
* @date 2019/7/17
*/
#pragma once
#include <Core/Core.hpp>

namespace Luna
{
	namespace Gfx
	{
		enum class EGraphicAdapterFlag
		{
			none = 0x0000,
			software = 0x0001,	// This adapter is software simulated and does not have hardware device.
			dedicated = 0x0002,	// This adapter has dedicated GPU processors and GPU memory, not embedded in CPU.
		};

		//! Describes an adapter to system.
		struct GraphicAdapterDesc
		{
			//! The name of the adapter.
			char name[256];
			//! The amount of GPU local(dedicated) memory installed to this adapter. 
			//! The GPU dedicated memory may be allocated from video memory for dedicated video card,
			//! or part of system memory for embedded GPU. The memory can only be read/write by GPU,
			//! CPU does not have access to it.
			usize local_memory;
			//! The amount of memory shared from system main memory for this adapter. The memory is allocated
			//! in system memory, and can be accessed by both CPU and GPU. GPU accesses this memory usually
			//! through PCIes and with cache enabled so it is much slower than accessing local memory.
			usize shared_memory;
			//! A combination of EGraphicAdapterFlag.
			EGraphicAdapterFlag flags;
		};

		//! @interface IGraphicAdapter
		//! Represents a physical graphic subsystem installed on this platform and can be used to 
		//! create a virtual graphic device (IGraphicDevice).
		struct IGraphicAdapter : public IObject
		{
			luiid("{74826a90-66b3-4cf0-8b4e-06c688df0af8}");

			virtual GraphicAdapterDesc desc() = 0;
		};
	}
}