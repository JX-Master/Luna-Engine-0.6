// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ITexture.hpp
* @author JXMaster
* @date 2020/5/8
*/
#pragma once
#include <Asset/Asset.hpp>
#include <Gfx/Gfx.hpp>

namespace Luna
{
	namespace Texture
	{
		struct SubresourceData
		{
			//! The buffer that holds the loaded image data.
			Blob data;

			//! The subresource to upload the data to.
			u32 subresource;

			// Information that describes the data.

			Gfx::EResourceFormat format;
			u32 width;
			u32 height;
			u32 depth;
			u32 row_pitch;
		};

		//! @interface ITexture
		//! `ITexture` interface represents a single texture resource that maps to one `gfx::IResource`
		//! texture resource object.
		//! 
		//! The texture object contains all information needed to create one texture 1D, 2D or 3D resource. 
		//! The information includes the resource description and the data to be loaded to the resource.
		struct ITexture : public Asset::IAsset
		{
			luiid("{511d9773-1b01-46d9-8e73-edd53348cbeb}");

			//! Gets the resource for this texture. This call fails when the resource is not loaded.
			virtual RP<Gfx::IResource> texture() = 0;

			//! Resets this texture object with the provided resource description. This call is asynchronous 
			//! and is treated as one reloading request.
			//! 
			//! The resource type specified in `desc` must be 1D, 2D or 3D texture.
			//! @param[in] desc The descriptor object to initialize the texture resource.
			//! @param[in] initial_data An array of data used to initialize all subresources of this resource.
			//! This array may be `nullptr` only if `num_initial_data` is 0, in which case the initialization
			//! for the resource is skipped and the data in the texture is undefined.
			//! @param[in] num_initial_data The number of elements in the `initial_data` array.
			virtual void reset(const Gfx::ResourceDesc& desc, SubresourceData* initial_data = nullptr, u32 num_initial_data = 0) = 0;
		};
	}
}