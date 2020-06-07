// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IAssetSaveRequest.hpp
* @author JXMaster
* @date 2020/4/15
*/
#pragma once
#include "IAsset.hpp"

namespace luna
{
	namespace asset
	{
		//! @interface IAssetSaveRequest
		//! @threadsafe
		//! Represents an asynchronous save request that you can use to check the saving results
		//! and waits the asset to be saved.
		struct IAssetSaveRequest : public IWaitable
		{
			luiid("{6ae3a455-50c9-48fa-a756-0d4d451d9c08}");

			//! Gets the result code of the saving operation.
			virtual result_t result() = 0;

			//! Gets the error object if the result code is `e_user_failure` or `e_user_success`.
			//! Returns `nullptr` on other cases.
			virtual IError* error_object() = 0;
		};
	}
}