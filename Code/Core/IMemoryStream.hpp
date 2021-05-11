// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IMemoryStream.hpp
* @author JXMaster
* @date 2020/1/28
*/
#pragma once
#include "IStream.hpp"
#include <Runtime/Blob.hpp>

namespace Luna
{
	//! @interface IMemoryStream
	//! The memory stream implements a stream object that is saved in the memory.
	struct IMemoryStream : public IStream
	{
		luiid("{b2fee308-b292-4fd4-b2c5-ece036a35e4a}");

		//! Gets the underlying blob that contains the memory stream data. 
		//! Note that the beginning of the blob is not the beginning of the 
		//! data, the data range may be shifted due to alignment requirement.
		virtual const Blob& get_blob() = 0;

		//! Gets the offset of the data from the beginning of the blob memory.
		virtual usize get_data_offset() = 0;

		//! Get the data of the memory stream. The returned pointer equals to 
		//! `(usize)get_blob().data() + get_data_offset()`, which is also the 
		//! cursor position 0.
		//! The available data region is : [get_data(), get_data() + size()).
		virtual void* get_data() = 0;

		//! Sets the underlying blob object.
		//! 
		//! The cursor position will be reset to 0 after this call.
		//! @param[in] blob The blob to set.
		//! @param[in] offset The offset between the beginning of the blob memory 
		//! and the beginning of the data region.
		//! @param[in] sz The size of the data region. Specify `usize_max` will use the full 
		//! range of the blob from `offset` to the end of the blob memory.
		//! @return Returns the original blob object.
		//! @remark The 0 position of the cursor will be set at 
		//! `(usize)get_blob().data() + offset`, while the data region will be set as:
		//! [(usize)get_blob().data() + offset, (usize)get_blob().data() + offset + sz)
		virtual Blob set_blob(const Blob& blob, usize offset = 0, usize sz = usize_max) = 0;

		//! Sets the underlying blob object.
		//! 
		//! The cursor position will be reset to 0 after this call.
		//! @param[in] blob The blob to set.
		//! @param[in] offset The offset between the beginning of the blob memory 
		//! and the beginning of the data region.
		//! @param[in] sz The size of the data region. Specify `usize_max` will use the full 
		//! range of the blob from `offset` to the end of the blob memory.
		//! @return Returns the original blob object.
		//! @remark The 0 position of the cursor will be set at 
		//! `(usize)get_blob().data() + offset`, while the data region will be set as:
		//! [(usize)get_blob().data() + offset, (usize)get_blob().data() + offset + sz)
		virtual Blob set_blob(Blob&& blob, usize offset = 0, usize sz = usize_max) = 0;
	};
}