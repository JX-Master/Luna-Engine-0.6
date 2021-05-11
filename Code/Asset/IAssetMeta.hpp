// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IAssetMeta.hpp
* @author JXMaster
* @date 2020/4/17
*/
#pragma once
#include <Core/Core.hpp>
#include <Runtime/Path.hpp>
#include <Runtime/Vector.hpp>
#include "IAsset.hpp"
#include "IAssetSaveRequest.hpp"

namespace Luna
{
	namespace Asset
	{
		enum class EAssetState : u32
		{
			//! The data for this asset is not loaded.
			unloaded = 0,
			//! The data for this asset is currently loading.
			loading = 1,
			//! The data for this asset is loaded and ready for use.
			loaded = 2,
		};

		enum class EAssetFlag : u32
		{
			none = 0,
			//! There are errors occurred when loading the asset. The error object is valid 
			//! when this flag is set.
			//! 
			//! This flag is set by the asset implementation when an error occurred during loading, and 
			//! cleared by system when another loading request is occurred. The asset is in unloaded 
			//! state if any error occurs.
			loading_error = 0x01,
		};

		enum class EAssetLoadFlag : u32
		{
			none = 0,
			//! Always reload the data even if the data has been loaded. If this flag is not set and 
			//! the asset is in `loaded` state, the `load` call does nothing.
			force_reload = 0x01,
			//! Tells the asset to load data from the parameters the user specified rather than loading
			//! the data from asset file. This is useful if the user wants to create and initialize 
			//! a runtime asset that does not have a saved file on the file system.
			//! 
			//! When this is specified, the system will not load the asset data from VFS and will call 
			//! `on_load_procedural_data` method of the asset type rather than the normal
			//! `on_load_data` method. This can be done even if `data_path` of the asset is not set.
			procedural = 0x02,
		};

		enum class EAssetUnloadFlag : u32
		{
			none = 0,
			//! Do not unload the data of any dependency assets of this asset even if they are unused.
			no_unload_unused_deps = 0x01,
		};

		enum class EAssetSaveFormat : u32
		{
			ascii = 1,
			// binary = 2, // [TODO] not implemented.
		};

		//! @interface IAssetMeta
		//! @threadsafe
		//! Holds the meta information loaded from `{asset_name}.meta.la/lb`.
		//! Contained by the asset object to communicate to the asset system.
		struct IAssetMeta : public IObject
		{
			luiid("{ed8670a0-227f-44c4-ac4d-b91ee5a43613}");

			//----------------------------------- PUBLIC API -----------------------------------
			//		The following APIs can be used either by asset implementation and user.
			//----------------------------------------------------------------------------------

			//! Gets the asset GUID. The asset GUID cannot be changed after
			//! it is created.
			virtual Guid guid() = 0;

			//! Gets the type of the asset. The asset type cannot be changed after
			//! it is created.
			virtual Name type() = 0;

			//! Gets the asset object this meta object is attached to.
			virtual P<IAsset> attaching_asset() = 0;

			//! Gets the state of the asset.
			virtual EAssetState state() = 0;

			//! Gets the flags of the asset.
			virtual EAssetFlag flags() = 0;

			//! Gets the path of the data file of this asset. The path does not include the extension 
			//! (.data.la/lb).
			//! 
			//! This may be `empty()` for runtime generated asset that does not have its file on VFS.
			virtual const Path& data_path() = 0;

			//! Gets the path of the meta file of this asset. The path does not include the extension 
			//! (.data.la/lb).
			//! 
			//! This may be `empty()` for runtime generated asset that does not have its file on VFS.
			virtual const Path& meta_path() = 0;

			//! Sets the path of the data file of this asset. Note that this call will not move any file on the 
			//! file system, the user should move the file manually before calling this if needed.
			//! 
			//! The path must be absolute in the virtual file system, and should not contain any extension (.data.la or .data.lb).
			//! 
			//! The path is always copy-assigned, so that the succeeding calls to modify the path passed in will not affect the 
			//! set path.
			//! 
			//! The path may be `empty()`, which removes the previous path.
			virtual void set_data_path(const Path& path) = 0;

			//! Sets the path of the meta file of this asset. Note that this call will not move any file on the 
			//! file system, the user should move the file manually before calling this if needed.
			//! 
			//! The path must be absolute in the virtual file system, and should not contain any extension (.meta.la or .meta.lb).
			//! 
			//! The path is always copy-assigned, so that the succeeding calls to modify the path passed in will not affect the 
			//! set path.
			//! 
			//! The path may be `empty()`, which removes the previous path.
			virtual RV set_meta_path(const Path& path) = 0;

			//! Gets the error object attached with this asset. The error object is valid only if `EAssetFlag::error`
			//! flag is set.
			virtual Error& error_object() = 0;

			//! Gets the mutex attached to this asset.
			virtual IMutex* mutex() = 0;

			//! Checks if this asset is unused.
			//! An asset is unused if:
			//! 1. All if its dependent assets are unused or unload, and,
			//! 2. The asset is not pinned (the pin count for the asset is 0).
			virtual bool unused() = 0;

			//! Adds the pin count for this asset by one.
			//! The asset data will not be automatically unloaded by system when its pin count is not 0, 
			//! but the user can still unload the asset data by calling `IAssetMeta::unload` manually.
			virtual void pin() = 0;

			//! Releases the pin count for this asset by one.
			virtual void unpin() = 0;

			//! Gets the pin count of this asset.
			virtual u32 pin_count() = 0;

			//! Loads or reloads the data of the asset from file. This call is asynchronous.
			//! @param[in] flags The load flags to specify.
			//! @param[in] params The load parameter object passed to the implementation to provide additional 
			//! load parameters.
			//! @remark The load operation will actually be scheduled if:
			//! * The asset is in `unloaded` state.
			//! * The asset is in `loaded` state and `EAssetLoadFlag::force_reload` is specified.
			//! 
			//! The load operation will be ignored if the asset is already in `loading` state.
			virtual void load(EAssetLoadFlag flags = EAssetLoadFlag::none, const Variant& params = Variant()) = 0;

			//! Unloads the data of the asset. This call is synchronous.
			virtual void unload(EAssetUnloadFlag flags = EAssetUnloadFlag::none) = 0;

			//! Saves the data to the corresponding files. This call is asynchronous.
			virtual RP<IAssetSaveRequest> save_data(EAssetSaveFormat save_format, const Variant& params = Variant()) = 0;

			//! Saves the meta to the corresponding files. This call is asynchronous.
			virtual RP<IAssetSaveRequest> save_meta(EAssetSaveFormat save_format) = 0;

			//! Gets a list of the GUIDs of all dependencies.
			virtual Vector<Guid> dependencies() = 0;

			//! Gets a list of the guids of all assets that depends on this asset. This list is managed by system and 
			//! gets updated automatically when `add_dependency` and `remove_dependency` on other assets are called.
			virtual Vector<Guid> dependents() = 0;

			//! Replaces one dependency asset of this asset to another asset. This can only be called when the asset is 
			//! loaded.
			virtual RV replace_dependency(const Guid& before, const Guid& after) = 0;

			//----------------------------------- PRIVATE API -----------------------------------
			//		The following APIs are designed for asset implementation use only, they
			//	are prefixed with `internal_` and the asset user must not use these APIs 
			//	directly to prevent breaking the data consistency.
			//----------------------------------------------------------------------------------

			//! Adds one dependency record to the meta data.
			//! 
			//! One GUID can be added multiple times, thus produce multiple records. For every add call, one remove call 
			//! must be made to completely remove the dependency.
			virtual void internal_add_dependency(const Guid& guid) = 0;

			//! Removes one dependency record from the meta data.
			//! @return Returns `true` if the specified asset is removed. Returns `false` if the asset is not found.
			virtual bool internal_remove_dependency(const Guid& guid) = 0;

			//! Removes all dependency records from the meta data.
			virtual void internal_remove_all_dependencies() = 0;
		};
	}
}