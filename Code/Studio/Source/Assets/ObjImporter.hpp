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
namespace luna
{
	namespace editor
	{
		class ObjImporter : public IAssetEditor
		{
		public:
			lucid("{770ac671-c013-4b89-a0a2-ab222e919a35}");
			luiimpl(ObjImporter, IAssetEditor, IObject);

			P<IPath> m_create_dir;

			P<IPath> m_source_file_path;

			P<obj_loader::IObjFile> m_obj_file;

			Vector<P<IStringBuffer>> m_import_names;

			ObjImporter(IAllocator* alloc) :
				luibind(alloc) {}

			bool m_open;

			virtual void on_render(imgui::IContext* ctx) override;
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

			P<IName> m_name;

			ObjImporterType(IAllocator* alloc) :
				luibind(alloc),
				m_name(intern_name("Obj Model")) {}

			virtual IName* name() override
			{
				return m_name;
			}

			virtual P<IAssetEditor> new_importer(IPath* create_dir) override;
		};
	}
}