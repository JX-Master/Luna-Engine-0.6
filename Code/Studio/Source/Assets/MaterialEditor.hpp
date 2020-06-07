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

namespace luna
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

			WP<e3d::IMaterial> m_material;

			P<IStringBuffer> m_base_color_name;
			P<IStringBuffer> m_roughness_name;
			P<IStringBuffer> m_normal_name;
			P<IStringBuffer> m_metallic_name;
			P<IStringBuffer> m_emissive_name;

			bool m_open = true;

			MaterialEditor(IAllocator* alloc) :
				luibind(alloc)
			{
				m_base_color_name = new_string_buffer();
				m_roughness_name = new_string_buffer();
				m_normal_name = new_string_buffer();
				m_metallic_name = new_string_buffer();
				m_emissive_name = new_string_buffer();
			}

			virtual void on_render(imgui::IContext* ctx) override;
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

			P<IName> m_type;

			MaterialEditorType(IAllocator* alloc) :
				luibind(alloc)
			{
				m_type = intern_name("Material");
			}

			virtual IName* asset_type() override
			{
				return m_type;
			}
			virtual void on_draw_tile(imgui::IContext* ctx, asset::IAsset* asset, const RectF& draw_rect) override;
			virtual P<IAssetEditor> new_editor(asset::IAsset* editing_asset) override;
		};

		class MaterialCreator : public IAssetEditor
		{
		public:
			lucid("{d87a96b8-a7e8-4a1d-b5e6-1d31987b60a9}");
			luiimpl(MaterialCreator, IAssetEditor, IObject);

			P<IPath> m_create_dir;
			P<IStringBuffer> m_asset_name;
			bool m_open;

			MaterialCreator(IAllocator* alloc) :
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

		class MaterialCreatorType : public IAssetImporterType
		{
		public:
			lucid("{a55b2a64-58f0-4e9c-9829-2b0cec305c02}");
			luiimpl(MaterialCreatorType, IAssetImporterType, IObject);

			P<IName> m_name;

			MaterialCreatorType(IAllocator* alloc) :
				luibind(alloc),
				m_name(intern_name("Material")) {}

			virtual IName* name() override
			{
				return m_name;
			}

			virtual P<IAssetEditor> new_importer(IPath* create_dir) override
			{
				auto dialog = box_ptr(new_obj<MaterialCreator>(get_module_allocator()));
				dialog->m_create_dir = create_dir->clone();
				return dialog;
			}
		};
	}
}