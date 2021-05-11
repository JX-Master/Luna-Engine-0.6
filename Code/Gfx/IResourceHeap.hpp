// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IResourceHeap.hpp
* @author JXMaster
* @date 2019/7/20
*/
#pragma once
#include "IGraphicDeviceChild.hpp"
#include "ICommandQueue.hpp"
#include "IResource.hpp"

namespace Luna
{
	namespace Gfx
	{
		//! Specify what kind of resources this heap can hold.
		enum class EResourceHeapUsageFlag : u32
		{
			//! This is invalid as arguments.
			none = 0x00,
			//! This heap can be used to store buffers.
			buffer = 0x01,
			//! This heap can be used to store textures not used for render target and depth stencil.
			texture_non_rt_ds = 0x02,
			//! This heap can be used to store textures used for writing as render target or depth stencil.
			texture_rt_ds = 0x04,
			//! This heap can be used to store multi-sample anti-aliasing textures.
			texture_ms = 0x04,
		};

		//! Describes a resource heap to the system.
		struct ResourceHeapDesc
		{
			//! The size of the heap.
			u64 size;
			//! The intended use of the heap, that is, the resource type that you want to placed in
			//! this heap.
			EResourceHeapUsageFlag resource_heap_usages;
			//! The access type of the heap. This determines where to allocate memory for this heap.
			EAccessType access_type;

			ResourceHeapDesc() = default;
			ResourceHeapDesc(u64 _size, EResourceHeapUsageFlag _resource_heap_usages, EAccessType _access_type) :
				size(_size),
				resource_heap_usages(_resource_heap_usages),
				access_type(_access_type) {}
		};

		//! @interface IResourceHeap
		//! Represent a resource heap that can be used to allocate resource. Resources allocated
		//! in one heap may reuse the underlying heap memory. Resources allocated from different 
		//! heaps will not to use the same underlying memory.
		//! 
		//! The resource sub-allocation feature may not be supported by all underlying graphic APIs. Check `EDeviceFeature::placed_resource`
		//! by calling `IGraphicDevice::check_feature` before creating a new heap, or the heap creation may fail.
		struct IResourceHeap : IGraphicDeviceChild
		{
			luiid("{aa8f1116-b27d-427e-8275-77633d65475a}");

			virtual ResourceHeapDesc desc() = 0;

			//! Uses this heap to create a new resource. See documents of `IGraphicDevice::new_resource` for details.
			virtual RP<IResource> new_resource(u64 offset, const ResourceDesc& desc, const ClearValue* optimized_clear_value = nullptr) = 0;
		};
	}
}