// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextureImporter.hpp
* @author JXMaster
* @date 2020/5/8
*/
#pragma once
#include "../IAssetEditor.hpp"
#include "../IAssetImporterType.hpp"

namespace luna
{
	namespace editor
	{
		class TextureImporterType;

		class TextureImporter : public IAssetEditor
		{
		public:
			lucid("{29488656-e1e3-4e7d-b772-2cf93308ba8b}");
			luiimpl(TextureImporter, IAssetEditor, IObject);

			TextureImporterType* m_type;

			P<IPath> m_create_dir;

			P<IPath> m_source_file_path;

			image::ImageDesc m_desc;

			int m_import_format;
			P<IStringBuffer> m_asset_name;

			bool m_open;

			bool m_allow_render_target;


			TextureImporter(IAllocator* alloc) :
				luibind(alloc),
				m_open(true),
				m_allow_render_target(false),
				m_import_format(7)
			{
				m_asset_name = new_string_buffer();
			}

			virtual void on_render(imgui::IContext* ctx) override;
			virtual bool closed() override
			{
				return !m_open;
			}
		};

		class TextureImporterType : public IAssetImporterType
		{
		public:
			lucid("{20ce27ad-7530-480e-aac6-4edcde99bb88}");
			luiimpl(TextureImporterType, IAssetImporterType, IObject);

			P<IName> m_name;

			P<gfx::IShaderInputLayout> m_mipmapping_slayout;
			P<gfx::IPipelineState> m_mipmapping_pso;

			TextureImporterType(IAllocator* alloc) :
				luibind(alloc),
				m_name(intern_name("Texture Image")) {}

			RV init();

			virtual void generate_mipmaps(gfx::IResource* resource_with_most_detailed_mip, gfx::ICommandBuffer* compute_cmdbuf);

			virtual IName* name() override
			{
				return m_name;
			}

			virtual P<IAssetEditor> new_importer(IPath* create_dir) override;
		};
	}
}