// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextureEditorType.hpp
* @author JXMaster
* @date 2020/5/8
*/
#pragma once
#include "../IAssetEditorType.hpp"

namespace Luna
{
	namespace editor
	{
		class TextureEditorType : public IAssetEditorType
		{
		public:
			lucid("{86a024c6-8135-41da-8dc3-77b3dcb04d48}");
			luiimpl(TextureEditorType, IAssetEditorType, IObject);

			Name m_name;

			TextureEditorType() :
				m_name(Name("Texture")) {}

			virtual Name asset_type() override
			{
				return m_name;
			}

			//! Called when the tile is going to be drawn in asset browser.
			virtual void on_draw_tile(ImGui::IContext* ctx, Asset::IAsset* asset, const RectF& draw_rect) override;

			//! Called when a new editor is requested to be open for the specified asset.
			virtual P<IAssetEditor> new_editor(Asset::IAsset* editing_asset) override;
		};
	}
}