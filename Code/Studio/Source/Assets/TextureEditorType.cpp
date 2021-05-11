// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextureEditorType.cpp
* @author JXMaster
* @date 2020/5/8
*/
#include "TextureEditorType.hpp"
#include <Texture/Texture.hpp>
#include "TextureImporter.hpp"
#include "TextureEditor.hpp"

namespace Luna
{
	namespace editor
	{
		void TextureEditorType::on_draw_tile(ImGui::IContext* ctx, Asset::IAsset* asset, const RectF& draw_rect)
		{
			P<Texture::ITexture> tex = asset;
			if (tex->meta()->state() == Asset::EAssetState::loaded)
			{
				MutexGuard g(tex->meta()->mutex());
				auto texres = tex->texture();
				if (succeeded(texres))
				{
					ctx->set_cursor_pos(Float2(draw_rect.left, draw_rect.top));
					ctx->image(texres.get(), Float2(draw_rect.right - draw_rect.left, draw_rect.bottom - draw_rect.top));
				}
			}
			else
			{
				// Draw tex.
				auto text_sz = ctx->calc_text_size("Texture");
				Float2 center = Float2((draw_rect.right + draw_rect.left) / 2.0f, (draw_rect.top + draw_rect.bottom) / 2.0f);
				ctx->set_cursor_pos(center - text_sz / 2.0f);
				ctx->text("Texture");
			}
		}
		P<IAssetEditor> TextureEditorType::new_editor(Asset::IAsset* editing_asset)
		{
			auto edit = newobj<TextureEditor>();
			edit->m_tex = P<Texture::ITexture>(editing_asset);
			return edit;
		}
	}
}