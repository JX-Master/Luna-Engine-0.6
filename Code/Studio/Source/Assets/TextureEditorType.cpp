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

namespace luna
{
	namespace editor
	{
		void TextureEditorType::on_draw_tile(imgui::IContext* ctx, asset::IAsset* asset, const RectF& draw_rect)
		{
			P<texture::ITexture> tex = asset;
			if (tex->meta()->state() == asset::EAssetState::loaded)
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
		P<IAssetEditor> TextureEditorType::new_editor(asset::IAsset* editing_asset)
		{
			auto edit = box_ptr(new_obj<TextureEditor>(get_module_allocator()));
			edit->m_tex = P<texture::ITexture>(editing_asset);
			return edit;
		}
	}
}