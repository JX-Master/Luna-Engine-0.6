// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IAsset.cpp
* @author JXMaster
* @date 2020/4/15
*/
#pragma once
#include <Core/Core.hpp>

namespace Luna
{
	namespace Asset
	{
		struct IAssetMeta;

		//! @interface IAsset
		//! @threadsafe
		//! Implement this interface to define your own asset object type.
		struct IAsset : public IObject
		{
			luiid("{f99ee83a-8c39-4355-96af-3c6fe5c454f9}");

			//! Gets the asset meta object of this asset. The asset object always keep a strong reference to the meta object.
			virtual IAssetMeta* meta() = 0;
		};
	}
}