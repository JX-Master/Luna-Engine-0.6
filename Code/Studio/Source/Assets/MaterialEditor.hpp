// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MaterialEditor.hpp
* @author JXMaster
* @date 2020/5/29
*/
#pragma once
#include "../IAssetEditor.hpp"
#include "../IAssetEditorType.hpp"
#include "../IAssetImporterType.hpp"

namespace Luna
{
	namespace editor
	{
		class MaterialEditorType;
		class MaterialEditor : public IAssetEditor
		{
		public:
			lucid("{705b8d2f-75ef-4784-a72e-f99dcf3f67aa}");
			luiimpl(MaterialEditor, IAssetEditor, IObject);
			
			MaterialEditorType* m_type;

			WP<E3D::IMaterial> m_material;

			String m_base_color_name;
			String m_roughness_name;
			String m_normal_name;
			String m_metallic_name;
			String m_emissive_name;

			bool m_open = true;

			MaterialEditor() {}

			virtual void on_render(ImGui::IContext* ctx) override;
			virtual bool closed() override
			{
				return !m_open;
			}
		};

		class MaterialEditorType : public IAssetEditorType
		{
		public:
			lucid("{a4fe7266-6b7a-43fa-99b8-9199233f1fbf}");
			luiimpl(MaterialEditorType, IAssetEditorType, IObject);

			Name m_type;

			MaterialEditorType()
			{
				m_type = u8"Material";
			}

			virtual Name asset_type() override
			{
				return m_type;
			}
			virtual void on_draw_tile(ImGui::IContext* ctx, Asset::IAsset* asset, const RectF& draw_rect) override;
			virtual P<IAssetEditor> new_editor(Asset::IAsset* editing_asset) override;
		};

		class MaterialCreator : public IAssetEditor
		{
		public:
			lucid("{d87a96b8-a7e8-4a1d-b5e6-1d31987b60a9}");
			luiimpl(MaterialCreator, IAssetEditor, IObject);

			Path m_create_dir;
			String m_asset_name;
			bool m_open;

			MaterialCreator() :
				m_open(true) {}

			virtual void on_render(ImGui::IContext* ctx) override;
			virtual bool closed() override
			{
				return !m_open;
			}
		};

		class MaterialCreatorType : public IAssetImporterType
		{
		public:
			lucid("{a55b2a64-58f0-4e9c-9829-2b0cec305c02}");
			luiimpl(MaterialCreatorType, IAssetImporterType, IObject);

			Name m_name;

			MaterialCreatorType() :
				m_name(u8"Material") {}

			virtual Name name() override
			{
				return m_name;
			}

			virtual P<IAssetEditor> new_importer(const Path& create_dir) override
			{
				auto dialog = newobj<MaterialCreator>();
				dialog->m_create_dir = create_dir;
				return dialog;
			}
		};
	}
}