// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IAssetSaveRequest.hpp
* @author JXMaster
* @date 2020/4/15
*/
#pragma once
#include "IAsset.hpp"

namespace Luna
{
	namespace Asset
	{
		//! @interface IAssetSaveRequest
		//! @threadsafe
		//! Represents an asynchronous save request that you can use to check the saving results
		//! and waits the asset to be saved.
		struct IAssetSaveRequest : public IWaitable
		{
			luiid("{6ae3a455-50c9-48fa-a756-0d4d451d9c08}");

			//! Gets the result code of the saving operation. Returns 0 if no error was occurred.
			virtual errcode_t result() = 0;

			//! Gets the error object. The error object is valid only if the result code is 
			//! `BasicError::error_object`.
			virtual Error& error_object() = 0;
		};
	}
}