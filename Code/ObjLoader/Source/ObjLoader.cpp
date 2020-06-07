// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ObjLoader.cpp
* @author JXMaster
* @date 2020/5/12
*/
#include "ObjLoaderHeader.hpp"
#include "ObjFile.hpp"
#include <TinyObjLoader/tiny_obj_loader.h>

namespace luna
{
	namespace obj_loader
	{
		static_assert(sizeof(Index) == sizeof(tinyobj::index_t), "Index size does not match");

		void deinit() {}

		LUNA_OBJ_LOADER_API void init()
		{
			add_module("ObjLoader", deinit);
		}

		void copy_shape(Shape& dest, const tinyobj::shape_t& src)
		{
			dest.name = intern_name(src.name.c_str());
			
			// Copy mesh.
			dest.mesh.indices.resize(src.mesh.indices.size());
			if (dest.mesh.indices.size())
			{
				memcpy(&dest.mesh.indices[0], &src.mesh.indices[0], dest.mesh.indices.size() * sizeof(Index));
			}
			dest.mesh.num_face_vertices.resize(src.mesh.num_face_vertices.size());
			if (dest.mesh.num_face_vertices.size())
			{
				memcpy(&dest.mesh.num_face_vertices[0], &src.mesh.num_face_vertices[0], dest.mesh.num_face_vertices.size() * sizeof(uint8));
			}
			dest.mesh.material_ids.resize(src.mesh.material_ids.size());
			if (dest.mesh.material_ids.size())
			{
				memcpy(&dest.mesh.material_ids[0], &src.mesh.material_ids[0], dest.mesh.material_ids.size() * sizeof(int32));
			}
			dest.mesh.smoothing_group_ids.resize(src.mesh.smoothing_group_ids.size());
			if (dest.mesh.smoothing_group_ids.size())
			{
				memcpy(&dest.mesh.smoothing_group_ids[0], &src.mesh.smoothing_group_ids[0], dest.mesh.smoothing_group_ids.size() * sizeof(int32));
			}

			// Copy lines.
			dest.lines.indices.resize(src.lines.indices.size());
			if (dest.lines.indices.size())
			{
				memcpy(&dest.lines.indices[0], &src.lines.indices[0], dest.lines.indices.size() * sizeof(Index));
			}
			dest.lines.num_line_vertices.resize(src.lines.num_line_vertices.size());
			if (dest.lines.num_line_vertices.size())
			{
				memcpy(&dest.lines.num_line_vertices[0], &src.lines.num_line_vertices[0], dest.lines.num_line_vertices.size() * sizeof(int32));
			}

			// Copy points.
			dest.points.indices.resize(src.points.indices.size());
			if (dest.points.indices.size())
			{
				memcpy(&dest.points.indices[0], &src.points.indices[0], dest.points.indices.size() * sizeof(Index));
			}
		}

		LUNA_OBJ_LOADER_API RP<IObjFile> load(const char* platform_file_name)
		{
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> meterials;

			std::string warn;
			std::string err;

			bool ret = tinyobj::LoadObj(&attrib, &shapes, &meterials, &warn, &err, platform_file_name);

			if (!ret)
			{
				if (!err.empty())
				{
					set_err(e_failure, err.c_str());
					return e_user_failure;
				}
				return e_failure;
			}

			auto obj = box_ptr(new_obj<ObjFile>(get_module_allocator()));

			auto& attributes = obj->m_attributes;

			// Copy attributes.
			attributes.vertices.resize(attrib.vertices.size() / 3);
			attributes.normals.resize(attrib.normals.size() / 3);
			attributes.texcoords.resize(attrib.texcoords.size() / 2);
			attributes.colors.resize(attrib.colors.size() / 3);

			if (attributes.vertices.size())
			{
				memcpy(&attributes.vertices[0], &attrib.vertices[0], sizeof(Float3U) * attributes.vertices.size());
			}
			if (attributes.normals.size())
			{
				memcpy(&attributes.normals[0], &attrib.normals[0], sizeof(Float3U) * attributes.normals.size());
			}
			if (attributes.texcoords.size())
			{
				memcpy(&attributes.texcoords[0], &attrib.texcoords[0], sizeof(Float2U) * attributes.texcoords.size());
			}
			if (attributes.colors.size())
			{
				memcpy(&attributes.colors[0], &attrib.colors[0], sizeof(Float3U) * attributes.colors.size());
			}

			// Copy shape information.
			obj->m_shapes.resize(shapes.size());
			for (size_t i = 0; i < shapes.size(); ++i)
			{
				copy_shape(obj->m_shapes[i], shapes[i]);
			}

			return obj;
		}
	}
}