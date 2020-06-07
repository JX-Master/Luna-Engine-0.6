// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IModel.hpp
* @author JXMaster
* @date 2020/5/25
*/
#pragma once
#include <Asset/Asset.hpp>
#include "IMesh.hpp"
#include "IMaterial.hpp"

namespace luna
{
	namespace e3d
	{
		//! @interface IModel
		struct IModel : public asset::IAsset
		{
			luiid("{e0b7d1f0-d9af-4fb8-b929-1cf8745084ee}");

			virtual asset::PAsset<IMesh> mesh() = 0;

			virtual void set_mesh(asset::PAsset<IMesh> mesh_asset) = 0;

			virtual size_t count_materials() = 0;

			//! Adds one material slot.
			virtual void add_material(size_t index) = 0;

			//! Removes one material slot.
			virtual void remove_material(size_t index) = 0;

			virtual asset::PAsset<IMaterial> material(size_t index) = 0;

			virtual void set_material(size_t index, asset::PAsset<IMaterial> material) = 0;
		};
	}
}