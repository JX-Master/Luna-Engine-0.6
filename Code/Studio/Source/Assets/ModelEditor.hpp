// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ModelEditor.hpp
* @author JXMaster
* @date 2020/5/27
*/
#pragma once
#include "../IAssetEditor.hpp"
#include "../IAssetEditorType.hpp"
#include "../IAssetImporterType.hpp"
namespace Luna
{
	namespace editor
	{
		class ModelEditorType;
		class ModelEditor : public IAssetEditor
		{
		public:
			lucid("{46d8b09d-1d7d-4deb-95b1-ac008c7998d4}");
			luiimpl(ModelEditor, IAssetEditor, IObject);

			ModelEditorType* m_type;

			WP<E3D::IModel> m_model;

			String m_mesh_name;

			Vector<String> m_mat_names;

			bool m_open = true;

			ModelEditor() {}

			virtual void on_render(ImGui::IContext* ctx) override;
			virtual bool closed() override
			{
				return !m_open;
			}
		};

		class ModelEditorType : public IAssetEditorType
		{
		public:
			lucid("{6627e2aa-3638-4d35-a72a-0618d235fac6}");
			luiimpl(ModelEditorType, IAssetEditorType, IObject);

			Name m_type;

			ModelEditorType()
			{
				m_type = "Model";
			}

			virtual Name asset_type() override
			{
				return m_type;
			}
			virtual void on_draw_tile(ImGui::IContext* ctx, Asset::IAsset* asset, const RectF& draw_rect) override;
			virtual P<IAssetEditor> new_editor(Asset::IAsset* editing_asset) override;
		};

		class ModelCreator : public IAssetEditor
		{
		public:
			lucid("{bd5b3a4d-d52c-4a6d-9e49-49a083096039}");
			luiimpl(ModelCreator, IAssetEditor, IObject);

			Path m_create_dir;
			String m_asset_name;
			bool m_open;

			ModelCreator() :
				m_open(true) {}

			virtual void on_render(ImGui::IContext* ctx) override;
			virtual bool closed() override
			{
				return !m_open;
			}
		};

		class ModelCreatorType : public IAssetImporterType
		{
		public:
			lucid("{a7cc3b27-fe34-4521-b052-797448364888}");
			luiimpl(ModelCreatorType, IAssetImporterType, IObject);

			Name m_name;

			ModelCreatorType() :
				m_name("Model") {}

			virtual Name name() override
			{
				return m_name;
			}

			virtual P<IAssetEditor> new_importer(const Path& create_dir) override
			{
				auto dialog = newobj<ModelCreator>();
				dialog->m_create_dir = create_dir;
				return dialog;
			}
		};
	}
}