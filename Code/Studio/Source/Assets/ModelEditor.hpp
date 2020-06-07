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
namespace luna
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

			WP<e3d::IModel> m_model;

			P<IStringBuffer> m_mesh_name;

			Vector<P<IStringBuffer>> m_mat_names;

			bool m_open = true;

			ModelEditor(IAllocator* alloc) :
				luibind(alloc) 
			{
				m_mesh_name = new_string_buffer();
			}

			virtual void on_render(imgui::IContext* ctx) override;
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

			P<IName> m_type;

			ModelEditorType(IAllocator* alloc) :
				luibind(alloc)
			{
				m_type = intern_name("Model");
			}

			virtual IName* asset_type() override
			{
				return m_type;
			}
			virtual void on_draw_tile(imgui::IContext* ctx, asset::IAsset* asset, const RectF& draw_rect) override;
			virtual P<IAssetEditor> new_editor(asset::IAsset* editing_asset) override;
		};

		class ModelCreator : public IAssetEditor
		{
		public:
			lucid("{bd5b3a4d-d52c-4a6d-9e49-49a083096039}");
			luiimpl(ModelCreator, IAssetEditor, IObject);

			P<IPath> m_create_dir;
			P<IStringBuffer> m_asset_name;
			bool m_open;

			ModelCreator(IAllocator* alloc) :
				luibind(alloc),
				m_open(true)
			{
				m_asset_name = new_string_buffer();
			}

			virtual void on_render(imgui::IContext* ctx) override;
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

			P<IName> m_name;

			ModelCreatorType(IAllocator* alloc) :
				luibind(alloc),
				m_name(intern_name("Model")) {}

			virtual IName* name() override
			{
				return m_name;
			}

			virtual P<IAssetEditor> new_importer(IPath* create_dir) override
			{
				auto dialog = box_ptr(new_obj<ModelCreator>(get_module_allocator()));
				dialog->m_create_dir = create_dir->clone();
				return dialog;
			}
		};
	}
}