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

namespace Luna
{
	namespace E3D
	{
		//! @interface IModel
		struct IModel : public Asset::IAsset
		{
			luiid("{e0b7d1f0-d9af-4fb8-b929-1cf8745084ee}");

			virtual Asset::PAsset<IMesh> mesh() = 0;

			virtual void set_mesh(Asset::PAsset<IMesh> mesh_asset) = 0;

			virtual usize count_materials() = 0;

			//! Adds one material slot.
			virtual void add_material(usize index) = 0;

			//! Removes one material slot.
			virtual void remove_material(usize index) = 0;

			virtual Asset::PAsset<IMaterial> material(usize index) = 0;

			virtual void set_material(usize index, Asset::PAsset<IMaterial> material) = 0;
		};
	}
}