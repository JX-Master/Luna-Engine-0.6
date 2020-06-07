// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextureEditor.cpp
* @author JXMaster
* @date 2020/5/9
*/
#include "TextureEditor.hpp"

namespace luna
{
	namespace editor
	{
		void TextureEditor::on_render(imgui::IContext* ctx)
		{
			auto tex = m_tex.lock();
			if (!tex)
			{
				m_open = false;
				return;
			}

			char name[32];
			sprintf_s(name, "Texture###%d", (uint32)this);
			
			ctx->begin(name, &m_open, imgui::EWindowFlag::no_collapse);

			lutry
			{
				lulet(res, tex->texture());
				auto desc = res->desc();
				ctx->image(res, Float2((float32)desc.width, (float32)desc.height));
			}
			lucatch
			{
				ctx->text("Texture Unavailable.");
			}

			ctx->end();
		}
	}
}