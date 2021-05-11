// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IAssetImporterType.hpp
* @author JXMaster
* @date 2020/5/12
*/
#pragma once
#include "StudioHeader.hpp"
#include "IAssetEditor.hpp"

namespace Luna
{
	namespace editor
	{
		struct IAssetImporterType : public IObject
		{
			luiid("{3a383f59-8455-4fed-873d-febf4d57ec0b}");

			//! Gets the name of the importer.
			virtual Name name() = 0;

			//! Called when a new importer is requested to be open for the specified asset.
			virtual P<IAssetEditor> new_importer(const Path& create_dir) = 0;
		};
	}
}