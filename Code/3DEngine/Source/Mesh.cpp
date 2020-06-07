// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Mesh.cpp
* @author JXMaster
* @date 2020/5/10
*/
#include "Mesh.hpp"
#include <Renderer/Renderer.hpp>

namespace luna
{
	namespace e3d
	{
		void Mesh::reset(IBlob* vertex_data, IBlob* index_data, uint32 num_pieces, const MeshPieceDesc* pieces)
		{
			auto data = new_var(EVariantType::table);
			auto pieces_field = new_var1(EVariantType::variant, num_pieces);
			auto pieces_data = pieces_field->var_buf();
			data->set_field(0, intern_name("pieces"), pieces_field);
			for (uint32 i = 0; i < num_pieces; ++i)
			{
				auto piece = new_var(EVariantType::table);
				auto offset = new_var(EVariantType::u32);
				auto size = new_var(EVariantType::u32);
				offset->u32() = pieces[i].first_index_offset;
				size->u32() = pieces[i].num_indices;
				piece->set_field(0, intern_name("offset"), offset);
				piece->set_field(0, intern_name("size"), size);
				pieces_data[i] = piece;
			}
			auto vert = new_var(EVariantType::blob);
			vert->blob() = vertex_data;
			auto index = new_var(EVariantType::blob);
			index->blob() = index_data;
			data->set_field(0, intern_name("vertices"), vert);
			data->set_field(0, intern_name("indices"), index);
			meta()->load(asset::EAssetLoadFlag::force_reload | asset::EAssetLoadFlag::procedural, data);
		}

		P<asset::IAsset> MeshType::on_new_asset(asset::IAssetMeta* meta)
		{
			P<Mesh> mesh = box_ptr(new_obj<Mesh>(get_module_allocator()));
			mesh->m_meta = meta;
			return mesh;
		}

		RV load_data(asset::IAsset* target_asset, IVariant* data)
		{
			lutry
			{
				lulet(pieces_field, data->field(0, intern_name("pieces")));
				auto num_pieces = pieces_field->length(1);
				lulet(pieces_data, pieces_field->check_var_buf());

				Vector<Mesh::Piece> pieces(get_module_allocator());

				for (size_t i = 0; i < num_pieces; ++i)
				{
					auto piece = pieces_data[i];
					lulet(offset, piece->field(0, intern_name("offset")));
					lulet(size, piece->field(0, intern_name("size")));
					Mesh::Piece p;
					luset(p.m_offset, offset->check_u32());
					luset(p.m_size, size->check_u32());
					pieces.push_back(p);
				}
				// Upload resource.
				lulet(vert, data->field(0, intern_name("vertices")));
				lulet(index, data->field(0, intern_name("indices")));
				lulet(vert_blob, vert->check_blob());
				lulet(index_blob, index->check_blob());
				lulet(vert_res, renderer::device()->new_resource(gfx::ResourceDesc::buffer(
					gfx::EAccessType::gpu_local, gfx::EResourceUsageFlag::vertex_buffer, vert_blob.get()->size())));
				lulet(index_res, renderer::device()->new_resource(gfx::ResourceDesc::buffer(
					gfx::EAccessType::gpu_local, gfx::EResourceUsageFlag::index_buffer, index_blob.get()->size())));
				lulet(vert_upload_res, renderer::device()->new_resource(gfx::ResourceDesc::buffer(
					gfx::EAccessType::upload, gfx::EResourceUsageFlag::none, vert_blob.get()->size())));
				lulet(index_upload_res, renderer::device()->new_resource(gfx::ResourceDesc::buffer(
					gfx::EAccessType::upload, gfx::EResourceUsageFlag::none, index_blob.get()->size())));
				lulet(vert_mapped, vert_upload_res->map_subresource(0, false, 1, 0));
				lulet(index_mapped, index_upload_res->map_subresource(0, false, 1, 0));
				memcpy(vert_mapped, vert_blob.get()->data(), vert_blob.get()->size());
				memcpy(index_mapped, index_blob.get()->data(), index_blob.get()->size());
				vert_upload_res->unmap_subresource(0, true);
				index_upload_res->unmap_subresource(0, true);
				lulet(cmdbuf, renderer::main_copy_queue()->new_command_buffer());
				cmdbuf->copy_resource(vert_res, vert_upload_res);
				cmdbuf->copy_resource(index_res, index_upload_res);
				luexp(cmdbuf->submit());
				cmdbuf->wait();
				P<Mesh> mesh = target_asset;
				MutexGuard g(mesh->meta()->mutex());
				mesh->m_pieces = pieces;
				mesh->m_vb = vert_res;
				mesh->m_ib = index_res;
				mesh->m_vb_count = vert_blob.get()->size() / sizeof(Vertex);
				mesh->m_ib_count = index_blob.get()->size() / sizeof(uint32);
			}
			lucatchret;
			return s_ok;
		}

		RV MeshType::on_load_data(asset::IAsset* target_asset, IVariant* data, IVariant* params)
		{
			return load_data(target_asset, data);
		}
		RV MeshType::on_load_procedural_data(asset::IAsset* target_asset, IVariant* params)
		{
			return load_data(target_asset, params);
		}
		void MeshType::on_unload_data(asset::IAsset* target_asset)
		{
			P<Mesh> mesh = target_asset;
			mesh->m_vb = nullptr;
			mesh->m_ib = nullptr;
			mesh->m_vb_count = 0;
			mesh->m_ib_count = 0;
			mesh->m_pieces.clear();
		}
		RP<IVariant> MeshType::on_save_data(asset::IAsset* target_asset, IVariant* params)
		{
			P<IVariant> data;
			lutry
			{
				P<Mesh> mesh = target_asset;
				data = new_var(EVariantType::table);
				auto pieces_field = new_var1(EVariantType::variant, mesh->m_pieces.size());
				auto pieces_data = pieces_field->var_buf();
				data->set_field(0, intern_name("pieces"), pieces_field);
				for (uint32 i = 0; i < mesh->m_pieces.size(); ++i)
				{
					auto piece = new_var(EVariantType::table);
					auto offset = new_var(EVariantType::u32);
					auto size = new_var(EVariantType::u32);
					offset->u32() = mesh->m_pieces[i].m_offset;
					size->u32() = mesh->m_pieces[i].m_size;
					piece->set_field(0, intern_name("offset"), offset);
					piece->set_field(0, intern_name("size"), size);
					pieces_data[i] = piece;
				}

				// Read back vertex & index data from resource.
				using namespace gfx;
				lulet(vb_res, renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::readback, EResourceUsageFlag::none, mesh->m_vb_count * sizeof(Vertex))));
				lulet(ib_res, renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::readback, EResourceUsageFlag::none, mesh->m_ib_count * sizeof(uint32))));

				lulet(cmdbuf, renderer::main_copy_queue()->new_command_buffer());
				cmdbuf->copy_resource(vb_res, mesh->m_vb);
				cmdbuf->copy_resource(ib_res, mesh->m_ib);
				luexp(cmdbuf->submit());
				cmdbuf->wait();

				lulet(vb_mapped, vb_res->map_subresource(0, true));
				lulet(ib_mapped, ib_res->map_subresource(0, true));

				auto vert = new_var(EVariantType::blob);
				vert->blob() = new_blob(vb_mapped, mesh->m_vb_count * sizeof(Vertex));
				auto index = new_var(EVariantType::blob);
				index->blob() = new_blob(ib_mapped, mesh->m_ib_count * sizeof(uint32));
				data->set_field(0, intern_name("vertices"), vert);
				data->set_field(0, intern_name("indices"), index);
			}
			lucatchret;

			return data;
		}

		Unconstructed<MeshType> g_mesh_type;
	}
}