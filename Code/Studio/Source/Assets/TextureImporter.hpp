// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextureImporter.hpp
* @author JXMaster
* @date 2020/5/8
*/
#pragma once
#include "../IAssetEditor.hpp"
#include "../IAssetImporterType.hpp"

namespace Luna
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

			Path m_create_dir;

			Path m_source_file_path;

			Image::ImageDesc m_desc;

			int m_import_format;
			String m_asset_name;

			bool m_open;

			bool m_allow_render_target;


			TextureImporter() :
				m_open(true),
				m_allow_render_target(false),
				m_import_format(7)
			{
				m_asset_name = String();
			}

			virtual void on_render(ImGui::IContext* ctx) override;
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

			Name m_name;

			P<Gfx::IShaderInputLayout> m_mipmapping_slayout;
			P<Gfx::IPipelineState> m_mipmapping_pso;

			TextureImporterType() :
				m_name(Name("Texture Image")) {}

			RV init();

			virtual void generate_mipmaps(Gfx::IResource* resource_with_most_detailed_mip, Gfx::ICommandBuffer* compute_cmdbuf);

			virtual Name name() override
			{
				return m_name;
			}

			virtual P<IAssetEditor> new_importer(const Path& create_dir) override;
		};
	}
}