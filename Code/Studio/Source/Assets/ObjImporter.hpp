// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ObjImporter.hpp
* @author JXMaster
* @date 2020/5/13
*/
#pragma once
#include "../IAssetEditor.hpp"
#include "../IAssetImporterType.hpp"
#include <ObjLoader/ObjLoader.hpp>
namespace Luna
{
	namespace editor
	{
		class ObjImporter : public IAssetEditor
		{
		public:
			lucid("{770ac671-c013-4b89-a0a2-ab222e919a35}");
			luiimpl(ObjImporter, IAssetEditor, IObject);

			Path m_create_dir;

			Path m_source_file_path;

			P<obj_loader::IObjFile> m_obj_file;

			Vector<String> m_import_names;

			ObjImporter() {}

			bool m_open;

			virtual void on_render(ImGui::IContext* ctx) override;
			virtual bool closed() override
			{
				return !m_open;
			}
		};

		class ObjImporterType : public IAssetImporterType
		{
		public:
			lucid("{b3135a6b-19c3-49d2-9a72-09c4c797ce68}");
			luiimpl(ObjImporterType, IAssetImporterType, IObject);

			Name m_name;

			ObjImporterType() :
				m_name("Obj Model") {}

			virtual Name name() override
			{
				return m_name;
			}

			virtual P<IAssetEditor> new_importer(const Path& create_dir) override;
		};
	}
}