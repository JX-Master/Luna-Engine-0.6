// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IResource.hpp
* @author JXMaster
* @date 2019/7/20
*/
#pragma once
#include "IGraphicDeviceChild.hpp"
#include "GraphicDefines.hpp"

namespace luna
{
	namespace gfx
	{
		//! Describe a resource to the system.
		struct ResourceDesc
		{
			//! The width of the texture resource, or the size of the buffer
			//! resource in bytes.
			uint64 width;
			//! The resource type.
			EResourceType type;
			//! The resource format. Specify EResourceFormat::unknown for buffers.
			EResourceFormat format;
			//! The resource access type, this affects which heap this resource
			//! will be placed in.
			//! 
			//! If the resource is created in a specified heap, this parameter
			//! must be the same as the heap's access type, or it can be set to
			//! `EAccessType::unknwon` to match the heap type automatically.
			EAccessType access_type;
			//! A combination of EResourceUsage flags to indicate all possible 
			//! usages of this resource.
			EResourceUsageFlag usages;
			//! The height of the texture resource, specify 1 if this is a buffer
			//! resource.
			uint32 height;
			//! The sample count per pixel for multi-sample texture resource, specify 1 for
			//! other type of resources.
			uint32 sample_count;
			//! The sample quality level for multi-sample texture resource, specify 0 for other
			//! type of resources.
			uint32 sample_quality;
			//! The depth for 3D texture or the array size of resource array. Specify 1
			//! for other type of resources.
			uint16 depth_or_array_size;
			//! The number of mip-maps this resource have. This only applies to texture 
			//! resource, specify 1 for other resource types.
			uint16 mip_levels;

			static inline ResourceDesc buffer(EAccessType access_type, EResourceUsageFlag usages, uint64 size)
			{
				ResourceDesc d;
				d.width = size;
				d.type = EResourceType::buffer;
				d.format = EResourceFormat::unknown;
				d.access_type = access_type;
				d.usages = usages;
				d.height = 1;
				d.sample_count = 1;
				d.sample_quality = 0;
				d.depth_or_array_size = 1;
				d.mip_levels = 1;
				return d;
			}

			static inline ResourceDesc tex1d(EResourceFormat format, EAccessType access_type, EResourceUsageFlag usages, uint64 width, uint32 array_size = 1, uint32 mip_levels = 0)
			{
				ResourceDesc d;
				d.width = width;
				d.type = EResourceType::texture_1d;
				d.format = format;
				d.access_type = access_type;
				d.usages = usages;
				d.height = 1;
				d.sample_count = 1;
				d.sample_quality = 0;
				d.depth_or_array_size = array_size;
				d.mip_levels = mip_levels;
				return d;
			}

			static inline ResourceDesc tex2d(EResourceFormat format, EAccessType access_type, EResourceUsageFlag usages, uint32 width, uint32 height, uint32 array_size = 1, uint32 mip_levels = 0,
				uint32 sample_count = 1, uint32 sample_quality = 0)
			{
				ResourceDesc d;
				d.width = width;
				d.type = EResourceType::texture_2d;
				d.format = format;
				d.access_type = access_type;
				d.usages = usages;
				d.height = height;
				d.sample_count = sample_count;
				d.sample_quality = sample_quality;
				d.depth_or_array_size = array_size;
				d.mip_levels = mip_levels;
				return d;
			}

			static inline ResourceDesc tex3d(EResourceFormat format, EAccessType access_type, EResourceUsageFlag usages, uint32 width, uint32 height, uint32 depth, uint32 mip_levels = 0)
			{
				ResourceDesc d;
				d.width = width;
				d.type = EResourceType::texture_3d;
				d.format = format;
				d.access_type = access_type;
				d.usages = usages;
				d.height = height;
				d.sample_count = 1;
				d.sample_quality = 0;
				d.depth_or_array_size = depth;
				d.mip_levels = mip_levels;
				return d;
			}
		};

		//! @interface IResource
		//! Represents a memory region that can be accessed by GPU.
		//! `IBuffer` and `ITexture` inherits from `IResource`.
		struct IResource : public IGraphicDeviceChild
		{
			luiid("{413427de-84a3-43aa-a583-3d311adc0334}");

			//! Gets the descriptor object of the resource.
			virtual ResourceDesc desc() = 0;

			//! Gets the global state of the resource. This is the state that this resource will be after all pending commands that 
			//! refers to this resource are finished by GPU.
			//! 
			//! For buffers in the upload heap, the state will always be EResourceState::generic_read.
			//! For buffers in the readback heap, the state will always be EResourceState::copy_dest.
			//! For buffers in the default heap and textures with EResourceUsageFlag::simultaneous_access set in the default heap, 
			//! the state will always be EResourceState::common.
			//! 
			//! @param[in] subresource The subresource to fetch the global state.
			//! @remark The resource state tracking system requires you to submit the command buffers in the same order that you want the command 
			//! queue to process them. This is certain for a single-queue case, but not for a multi-queue case, you need to take care of the order
			//! you submit the command buffers yourself. This restriction only applies to textures in gpu-local heap without EResourceUsageFlag::simultaneous_access
			//! being set, which really have a global state to track for.
			virtual EResourceState global_state(uint32 subresource) = 0;

			//! Maps the data to system memory and gets a pointer to the buffer in order to enable CPU read/write.
			//! Map/unmap operations are reference counted, for each `map` operation, you need to call `unmap` once to finally unmap the memory.
			//! @param[in] subresource The subresource you want to map.
			//! @param[in] read_full_range In a `readback` resource, specify this to indicates that the whole subresource should be mapped, that is,
			//! transport the data to CPU readable memory address.
			//! @param[in] read_range_begin If `read_full_range` is `false`, use this to specify the beginning offset of the range to map.
			//! If `read_full_range` is `true`, this parameter is ignored.
			//! @param[in] read_range_end If `read_full_range` is `false`, use this to specify the end offset of the range to map.
			//! If `read_full_range` is `true`, this parameter is ignored.
			virtual R<void*> map_subresource(uint32 subresource, bool read_full_range, size_t read_range_begin = 0, size_t read_range_end = 0) = 0;

			//! Invalidates the pointer to the buffer.
			//! @param[in] subresource The subresource you want to map.
			//! @param[in] write_full_range In a `upload` buffer, specify this to indicates that the whole buffer data should be uploaded to GPU.
			//! @param[in] write_range_begin If `write_full_range` is `false`, use this to specify the beginning offset of the range to upload data to GPU.
			//! If `write_full_range` is `true`, this parameter is ignored.
			//! @param[in] write_range_end If `write_full_range` is `false`, use this to specify the end offset of the range to upload data to GPU.
			//! If `write_full_range` is `true`, this parameter is ignored.
			virtual void unmap_subresource(uint32 subresource, bool write_full_range, size_t write_range_begin = 0, size_t write_range_end = 0) = 0;
		
			//! Write data from the address specified to the resource.
			//! The resource must be mapped first by calling `map_subresource`. No GPU queue should use the resource when the resource is mapped to the 
			//! CPU, and the resource should be transferred to `EResourceState::common` state exlicitly or imexplicitly from the last command list.
			//! @param[in] subresource The index of the subresource to operate.
			//! @param[in] src A pointer to the source data.
			//! @param[in] src_row_pitch The offset between every row of source data.
			//! @param[in] src_depth_pitch The offset between every layer of source data.
			//! @param[in] write_box Specify the write offset in width, height and depth, and the size to write in width, height and depth.
			virtual RV write_subresource(uint32 subresource, const void* src, uint32 src_row_pitch, uint32 src_depth_pitch, const BoxU& write_box) = 0;

			//! Read data from subresource to the address specified.
			//! The resource must be mapped first by calling `map_subresource`. No GPU queue should use the resource when the resource is mapped to the 
			//! CPU, and the resource should be transferred to `EResourceState::common` state exlicitly or imexplicitly from the last command list.
			//! @param[in] dest A pointer to the memory that the data will be written to.
			//! @param[in] dest_row_pitch The offset between every row of destination buffer.
			//! @param[in] dest_depth_pitch The offset between every layer of destination buffer.
			//! @param[in] subresource The index of the subresource to operate.
			//! @param[in] read_box Specify the read offset in width, height and depth, and the size to read in width, height and depth.
			virtual RV read_subresource(void* dest, uint32 dest_row_pitch, uint32 dest_depth_pitch, uint32 subresource, const BoxU& read_box) = 0;
		};
	}
}