// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IAssetEditorType.hpp
* @author JXMaster
* @date 2020/5/8
*/
#pragma once
#include "StudioHeader.hpp"
#include "IAssetEditor.hpp"

namespace Luna
{
	namespace editor
	{
		//! @interface IAssetEditorType
		struct IAssetEditorType : public IObject
		{
			luiid("{bf28b011-6839-4b65-9340-f857d295cc6e}");

			//! Gets the type that this editor type is responsible to.
			virtual Name asset_type() = 0;

			//! Called when the tile is going to be drawn in asset browser.
			virtual void on_draw_tile(ImGui::IContext* ctx, Asset::IAsset* asset, const RectF& draw_rect) = 0;

			//! Called when a new editor is requested to be open for the specified asset.
			virtual P<IAssetEditor> new_editor(Asset::IAsset* editing_asset) = 0;
		};
	}
}