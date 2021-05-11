// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ObjImporter.cpp
* @author JXMaster
* @date 2020/5/13
*/
#include "ObjImporter.hpp"
#include <Runtime/HashMap.hpp>

namespace Luna
{
	namespace editor
	{
		static RV load_mesh_from_obj(E3D::IMesh* mesh, obj_loader::IObjFile* obj_file, u32 shape_index)
		{
			auto& m = obj_file->shapes()[shape_index].mesh;	// We only consider the mesh part of the specified shape.
			auto& faces = m.num_face_vertices;	// 
			auto& attrib = obj_file->attributes();

			// Convert indices to vertices.
			Vector<E3D::Vertex> vertices;
			for (auto& i : m.indices)
			{
				E3D::Vertex v;
				v.position = attrib.vertices[i.vertex_index];
				auto& color3 = attrib.colors[i.vertex_index];
				v.color = Float4U(color3.r, color3.g, color3.b, 1.0f);
				if (i.normal_index != -1 && i.normal_index < attrib.normals.size())
				{
					v.normal = attrib.normals[i.normal_index];
				}
				else
				{
					v.normal = Float3U(0.0f, 0.0f, 1.0f);
				}
				if (i.texcoord_index != -1 && i.texcoord_index < attrib.texcoords.size())
				{
					v.texcoord = attrib.texcoords[i.texcoord_index];
				}
				else
				{
					v.texcoord = Float2U(0.0f, 0.0f);
				}
				// Tangent is left to be computed later.
				vertices.push_back(v);
			}

			// Build index list.
			// Material ID -> Index list.
			HashMap<u32, Vector<u32>> mat_map;
			u32 vert_offset = 0;
			for (u32 i = 0; i < (u32)faces.size(); ++i)	// Once per face.
			{
				auto mat_id = m.material_ids[i];	// Finds the material ID for this face, this is used for pieces.
				auto iter = mat_map.find(mat_id);
				if (iter == mat_map.end())
				{
					iter = mat_map.insert(make_pair(mat_id, Vector<u32>())).first;
				}

				// If this is not a triangle face, convert this to triangles.
				for (u32 j = 0; j < (u32)(faces[i] - 2); ++j)
				{
					iter->second.push_back(vert_offset);
					iter->second.push_back(vert_offset + j + 1);
					iter->second.push_back(vert_offset + j + 2);
				}
				vert_offset += faces[i];
			}

			// Calculate tangents.
			Vector<Float3U> tangents;
			Vector<Float3U> binormals;
			tangents.resize(vertices.size(), Float3U(0.0f, 0.0f, 0.0f));
			binormals.resize(vertices.size(), Float3U(0.0f, 0.0f, 0.0f));

			u32 idx_offset = 0;
			for (auto& i : mat_map)
			{
				usize num_tris = i.second.size() / 3;
				for (usize j = 0; j < num_tris; ++j)
				{
					u32 i1 = i.second[j * 3];
					u32 i2 = i.second[j * 3 + 1];
					u32 i3 = i.second[j * 3 + 2];
					E3D::Vertex& v1 = vertices[i1];
					E3D::Vertex& v2 = vertices[i2];
					E3D::Vertex& v3 = vertices[i3];
					Float3 e1 = v3.position - v1.position;
					Float3 e2 = v2.position - v1.position;
					Float2 uv1 = v3.texcoord - v1.texcoord;
					Float2 uv2 = v2.texcoord - v1.texcoord;
					f32 r = 1.0f / (uv1.x * uv2.y - uv1.y * uv2.x);
					Float3U tangent(
						((e1.x * uv2.y) - (e2.x * uv1.y)) * r,
						((e1.y * uv2.y) - (e2.y * uv1.y)) * r,
						((e1.z * uv2.y) - (e2.z * uv1.y)) * r
					);
					Float3U binormal(
						((e1.x * uv2.x) - (e2.x * uv1.x)) * r,
						((e1.y * uv2.x) - (e2.y * uv1.x)) * r,
						((e1.z * uv2.x) - (e2.z * uv1.x)) * r
					);
					tangents[i1] = tangents[i1] + tangent;
					tangents[i2] = tangents[i2] + tangent;
					tangents[i3] = tangents[i3] + tangent;
					binormals[i1] = binormals[i1] + binormal;
					binormals[i2] = binormals[i2] + binormal;
					binormals[i3] = binormals[i3] + binormal;
				}

				idx_offset += (u32)i.second.size();
			}

			for (usize i = 0; i < vertices.size(); ++i)
			{
				Float3 n = vertices[i].normal;
				Float3 t = tangents[i];

				// Gram-Schmidt orthogonalize
				Float3 tang = normalize(t - n * dot(n, t));

				// Calculate handedness
				f32 w = dot(cross(n, t), binormals[i]);
				if (w < 0.0f)
				{
					tang = -tang;
				}
				vertices[i].tangent = tang;
			}

			// Fill vertex data.
			auto vb_blob = Blob(vertices.size() * sizeof(E3D::Vertex));
			memcpy(vb_blob.data(), vertices.data(), vertices.size() * sizeof(E3D::Vertex));

			// Fill indices data.
			usize idx_count = 0;
			for (auto& i : mat_map)
			{
				idx_count += i.second.size();
			}

			auto ib_blob = Blob(idx_count * sizeof(u32));
			
			idx_offset = 0;
			Vector<E3D::MeshPieceDesc> pieces;
			for (auto& i : mat_map)
			{
				E3D::MeshPieceDesc p;
				p.first_index_offset = idx_offset;
				p.num_indices = (u32)i.second.size();
				memcpy((u32*)ib_blob.data() + idx_offset, i.second.data(), sizeof(u32) * i.second.size());
				idx_offset += p.num_indices;
				pieces.push_back(p);
			}

			mesh->reset(vb_blob, ib_blob, (u32)pieces.size(), pieces.data());

			return RV();
		}

		void ObjImporter::on_render(ImGui::IContext* ctx)
		{
			char title[32];
			sprintf_s(title, "Texture Importer###%d", (u32)(usize)this);

			ctx->begin("Obj Mesh Importer", &m_open, ImGui::EWindowFlag::no_collapse);

			if (ctx->button("Select Source File"))
			{
				lutry
				{
					lulet(file_path, Gfx::open_file_dialog("Obj File\0*.obj\0\0",
						"Select Source File"));
					// Open file.
					lulet(file, platform_open_file(file_path[0].encode(EPathSeparator::system_preferred).c_str(),
						EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing));

					luset(m_obj_file, obj_loader::load(file_path[0].encode().c_str()));

					m_source_file_path = file_path[0];

					m_import_names.clear();
					for (auto& i : m_obj_file->shapes())
					{
						m_import_names.push_back(String());
					}
				}
				lucatch
				{
					if (lures != BasicError::user_canceled())
					{
						auto _ = Gfx::message_box(get_errmsg(lures), "Failed to import obj file",
							Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
					}
					m_source_file_path.clear();
					
				}
			}

			if (m_source_file_path.empty())
			{
				ctx->text("No obj file selected.");
			}
			else
			{
				ctx->text(m_source_file_path.encode().c_str());
				ctx->text("Object Information:");

				ctx->text("Vertex entries count: %u", (u32)m_obj_file->attributes().vertices.size());
				ctx->text("Normal entries count: %u", (u32)m_obj_file->attributes().normals.size());
				ctx->text("TexCoord entries count: %u", (u32)m_obj_file->attributes().texcoords.size());
				ctx->text("Color entries count: %u", (u32)m_obj_file->attributes().colors.size());

				if (m_obj_file->shapes().empty())
				{
					ctx->text("No Shape information detected, this model cannot be imported.");
				}
				else
				{
					ctx->text("%u meshes found", (u32)m_obj_file->shapes().size());
					if (ctx->collapsing_header("Shapes"))
					{
						for (u32 i = 0; i < (u32)m_obj_file->shapes().size(); ++i)
						{
							ctx->text("Name: %s", m_obj_file->shapes()[i].name.c_str());
							ctx->text("Faces: %u", (u32)m_obj_file->shapes()[i].mesh.num_face_vertices.size());

							ctx->push_id(i);
							ctx->input_text("Asset Name", m_import_names[i]);
							if (!m_import_names[i].empty())
							{
								ctx->text("The mesh will be imported as: %s%s", m_create_dir.encode().c_str(), m_import_names[i].c_str());
								if (ctx->button("Import"))
								{
									lutry2
									{
										P<E3D::IMesh> mesh;
										luset2(mesh, E3D::new_mesh());
										luexp2(load_mesh_from_obj(mesh, m_obj_file, i));
										
										// Save the asset.
										auto ass_path = m_create_dir;
										ass_path.push_back(Name(m_import_names[i].c_str()));
										ass_path.flags() = (ass_path.flags() & ~EPathFlag::diretory);

										luexp2(mesh->meta()->set_meta_path(ass_path));
										mesh->meta()->set_data_path(ass_path);
										lulet2(r1, mesh->meta()->save_meta(Asset::EAssetSaveFormat::ascii));
										lulet2(r2, mesh->meta()->save_data(Asset::EAssetSaveFormat::ascii));
										r1->wait();
										r2->wait();
									}
									lucatch2
									{
										auto _ = Gfx::message_box(get_errmsg(lures2), "Failed to import obj mesh asset",
										Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
									}
								}
							}
							ctx->pop_id();


						}
					}
				}
			}
			ctx->end();
		}

		P<IAssetEditor> ObjImporterType::new_importer(const Path& create_dir)
		{
			auto importer = newobj<ObjImporter>();
			importer->m_create_dir = create_dir;
			return importer;
		}
	}
}