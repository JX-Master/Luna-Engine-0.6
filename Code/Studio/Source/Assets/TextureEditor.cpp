// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextureEditor.cpp
* @author JXMaster
* @date 2020/5/9
*/
#include "TextureEditor.hpp"

namespace Luna
{
	namespace editor
	{
		void TextureEditor::on_render(ImGui::IContext* ctx)
		{
			auto tex = m_tex.lock();
			if (!tex)
			{
				m_open = false;
				return;
			}

			char name[32];
			sprintf_s(name, "Texture###%d", (u32)(usize)this);
			
			ctx->begin(name, &m_open, ImGui::EWindowFlag::no_collapse);

			lutry
			{
				lulet(res, tex->texture());
				auto desc = res->desc();
				ctx->image(res, Float2((f32)desc.width, (f32)desc.height));
			}
			lucatch
			{
				ctx->text("Texture Unavailable.");
			}

			ctx->end();
		}
	}
}