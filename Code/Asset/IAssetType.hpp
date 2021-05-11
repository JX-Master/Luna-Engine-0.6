// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IAssetType.cpp
* @author JXMaster
* @date 2020/4/15
*/
#pragma once
#include "IAssetMeta.hpp"

namespace Luna
{
	namespace Asset
	{
		//! @interface IAssetType
		//! @threadsafe
		//! Implemented by user and registered to the asset system to provide a factory
		//! for the one asset type.
		//! 
		//! The asset system keeps a weak reference to the asset type object, while all instances of assets this type
		//! object creates keeps a strong reference to the asset type object. The user should keep a strong reference 
		//! to the asset type object while it is registered to the asset system, and unregisters the asset type object
		//! when it is no longer needed.
		struct IAssetType : public IObject
		{
			luiid("{e0a3caaa-c66c-43b2-b59a-b1b1570a746d}");

			//! Gets the type name of this asset type.
			virtual Name type_name() = 0;

			//! Called when a new empty asset object is required to be created.
			//! @param[in] meta The asset meta object for the returned asset object, initialized with the meta information 
			//! loaded from file if the asset is required to be loaded, or only filled with the type and path information
			//! if the asset is required as a new asset. The asset object should keep a strong reference to this meta object
			//! until the asset object is destroyed. The asset state is set to unloaded.
			//! @return Returns the new created asset object with no data loaded (EAssetState::unloaded state).
			//! This operation cannot fail, so the return value cannot be `nullptr`.
			virtual P<IAsset> on_new_asset(IAssetMeta* meta) = 0;

			//! Called by the worker thread when the data of the asset is required to be loaded or reloaded from file.
			//! @param[in] target_asset The asset whose data is required to be loaded. The state of the asset will be `loading`
			//! when this function is called, if this is a reloading call, the original data in the asset will be preserved.
			//! @param[in] data The data variant object created by parsing `{asset_name}.data.la/.lb` file.
			//! @param[in] params Additional parameters the user provides. This may be `nullptr` if the user does not provide and 
			//! parameters.
			//! @return Returns success if the data is successfully loaded, or an error code if the data loading is failed. In case
			//! of any error, the system will record the error in the error object of the asset meta. The asset state will be set as loaded
			//! if the return value is success, or unloaded with error flag set if the return value is not success.
			virtual RV on_load_data(IAsset* target_asset, const Variant& data, const Variant& params) = 0;

			//! Called by the worker thread when the procedural asset data is required to be loaded to the target asset. This will be
			//! called if the user specify `EAssetLoadFlag::procedural` flag when issuing the load request.
			//! @param[in] target_asset The asset whose data is required to be loaded. The state of the asset will be `loading`
			//! when this function is called, if this is a reloading call, the original data in the asset will be preserved.
			//! @param[in] params Additional parameters the user provides. This may be `nullptr` if the user does not provide and 
			//! parameters.
			virtual RV on_load_procedural_data(IAsset* target_asset, const Variant& params) = 0;

			//! Called when the data of the asset is required to be unloaded. This call is not asynchronous.
			//! @remark The implementation should set the state of the object to `unloaded` manually.
			virtual void on_unload_data(IAsset* target_asset) = 0;

			//! Called by the worker thread when the data of the asset is required to be saved to the file.
			//! @param[in] target_asset The asset to save.
			//! @return Returns the data object that holds the serialized data for the asset. This data 
			//! object will be saved to file.
			virtual R<Variant> on_save_data(IAsset* target_asset, const Variant& params) = 0;

			//! Called when the data of one of the dependency assets is loaded and the current asset's state is `loaded`.
			virtual void on_dependency_data_load(IAsset* current_asset, IAsset* dependency_asset) = 0;

			//! Called when the data of one of the dependency assets is about to be unloaded and the current asset's state is `loaded`.
			virtual void on_dependency_data_unload(IAsset* current_asset, IAsset* dependency_asset) = 0;

			//! Called when `IAssetMeta::replace_dependency` is called for one asset of this type.
			virtual void on_dependency_replace(IAsset* current_asset, const Guid& before, const Guid& after) = 0;

			//! Called when the dependency asset registry is about to be removed from the system.
			//virtual void on_dependency_remove(IAsset* current_asset, IAsset* dependency_asset) = 0;

			//! Called when the dependency asset registry is created and added to the system.
			//virtual void on_dependency_create(IAsset* current_asset, IAsset* dependency_asset) = 0;
		};
	}
}