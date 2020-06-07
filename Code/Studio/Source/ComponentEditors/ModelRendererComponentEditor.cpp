// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ModelRendererComponentEditor.cpp
* @author JXMaster
* @date 2020/5/28
*/
#include "ModelRendererComponentEditor.hpp"

namespace luna
{
	namespace editor
	{
		void ModelRendererComponentEditor::on_render(imgui::IContext* ctx)
		{
			auto c = m_component.lock().as<e3d::IModelRenderer>();

			auto new_model_asset = asset::fetch_asset(new_path(m_model_name->c_str()));
			if (succeeded(new_model_asset) && (new_model_asset.get()->meta()->type() == intern_name("Model")))
			{
				c->set_model(new_model_asset.get()->meta()->guid());
			}

			auto model = c->model().lock();
			if (!m_editing && model)
			{
				m_model_name->assign(model->meta()->meta_path()->encode()->c_str());
			}
			else
			{
				m_model_name->clear();
			}

			ctx->input_text("Model Asset", m_model_name);
			m_editing = ctx->is_item_active();
		}
	}
}