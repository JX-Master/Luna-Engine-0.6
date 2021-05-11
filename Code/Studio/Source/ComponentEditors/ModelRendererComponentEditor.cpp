// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ModelRendererComponentEditor.cpp
* @author JXMaster
* @date 2020/5/28
*/
#include "ModelRendererComponentEditor.hpp"

namespace Luna
{
	namespace editor
	{
		void ModelRendererComponentEditor::on_render(ImGui::IContext* ctx)
		{
			auto c = m_component.lock().as<E3D::IModelRenderer>();

			auto new_model_asset = Asset::fetch_asset(Path(m_model_name.c_str()));
			if (succeeded(new_model_asset) && (new_model_asset.get()->meta()->type() == Name("Model")))
			{
				c->set_model(new_model_asset.get()->meta()->guid());
			}

			auto model = c->model().lock();
			if (!m_editing && model)
			{
				m_model_name.assign(model->meta()->meta_path().encode().c_str());
			}
			else
			{
				m_model_name.clear();
			}

			ctx->input_text("Model Asset", m_model_name);
			m_editing = ctx->is_item_active();
		}
	}
}