// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Mesh.cpp
* @author JXMaster
* @date 2020/5/10
*/
#include "Mesh.hpp"
#include <Renderer/Renderer.hpp>

namespace Luna
{
	namespace E3D
	{
		void Mesh::reset(const Blob& vertex_data, const Blob& index_data, u32 num_pieces, const MeshPieceDesc* pieces)
		{
			auto data = Variant(EVariantType::table);
			auto pieces_field = Variant(EVariantType::variant, num_pieces);
			auto pieces_data = pieces_field.to_var_buf();
			data.set_field(0, Name("pieces"), pieces_field);
			for (u32 i = 0; i < num_pieces; ++i)
			{
				auto piece = Variant(EVariantType::table);
				auto offset = Variant(EVariantType::u32);
				auto size = Variant(EVariantType::u32);
				offset.to_u32() = pieces[i].first_index_offset;
				size.to_u32() = pieces[i].num_indices;
				piece.set_field(0, Name("offset"), offset);
				piece.set_field(0, Name("size"), size);
				pieces_data[i] = piece;
			}
			auto vert = Variant(EVariantType::blob);
			vert.to_blob() = vertex_data;
			auto index = Variant(EVariantType::blob);
			index.to_blob() = index_data;
			data.set_field(0, Name("vertices"), vert);
			data.set_field(0, Name("indices"), index);
			meta()->load(Asset::EAssetLoadFlag::force_reload | Asset::EAssetLoadFlag::procedural, data);
		}

		P<Asset::IAsset> MeshType::on_new_asset(Asset::IAssetMeta* meta)
		{
			P<Mesh> mesh = newobj<Mesh>();
			mesh->m_meta = meta;
			return mesh;
		}

		RV load_data(Asset::IAsset* target_asset, const Variant& data)
		{
			lutry
			{
				auto& pieces_field = data.field(0, Name("pieces"));
				auto num_pieces = pieces_field.length(1);
				lulet(pieces_data, pieces_field.check_var_buf());

				Vector<Mesh::Piece> pieces;

				for (usize i = 0; i < num_pieces; ++i)
				{
					auto piece = pieces_data[i];
					auto& offset = piece.field(0, Name("offset"));
					auto& size = piece.field(0, Name("size"));
					Mesh::Piece p;
					luset(p.m_offset, offset.check_u32());
					luset(p.m_size, size.check_u32());
					pieces.push_back(p);
				}
				// Upload resource.
				auto& vert = data.field(0, Name("vertices"));
				auto& index = data.field(0, Name("indices"));
				lulet(vert_blob, vert.check_blob());
				lulet(index_blob, index.check_blob());
				lulet(vert_res, Renderer::device()->new_resource(Gfx::ResourceDesc::buffer(
					Gfx::EAccessType::gpu_local, Gfx::EResourceUsageFlag::vertex_buffer, vert_blob.get().size())));
				lulet(index_res, Renderer::device()->new_resource(Gfx::ResourceDesc::buffer(
					Gfx::EAccessType::gpu_local, Gfx::EResourceUsageFlag::index_buffer, index_blob.get().size())));
				lulet(vert_upload_res, Renderer::device()->new_resource(Gfx::ResourceDesc::buffer(
					Gfx::EAccessType::upload, Gfx::EResourceUsageFlag::none, vert_blob.get().size())));
				lulet(index_upload_res, Renderer::device()->new_resource(Gfx::ResourceDesc::buffer(
					Gfx::EAccessType::upload, Gfx::EResourceUsageFlag::none, index_blob.get().size())));
				lulet(vert_mapped, vert_upload_res->map_subresource(0, false, 1, 0));
				lulet(index_mapped, index_upload_res->map_subresource(0, false, 1, 0));
				memcpy(vert_mapped, vert_blob.get().data(), vert_blob.get().size());
				memcpy(index_mapped, index_blob.get().data(), index_blob.get().size());
				vert_upload_res->unmap_subresource(0, true);
				index_upload_res->unmap_subresource(0, true);
				lulet(cmdbuf, Renderer::main_copy_queue()->new_command_buffer());
				cmdbuf->copy_resource(vert_res, vert_upload_res);
				cmdbuf->copy_resource(index_res, index_upload_res);
				luexp(cmdbuf->submit());
				cmdbuf->wait();
				P<Mesh> mesh = target_asset;
				MutexGuard g(mesh->meta()->mutex());
				mesh->m_pieces = pieces;
				mesh->m_vb = vert_res;
				mesh->m_ib = index_res;
				mesh->m_vb_count = (u32)vert_blob.get().size() / (u32)sizeof(Vertex);
				mesh->m_ib_count = (u32)index_blob.get().size() / (u32)sizeof(u32);
			}
			lucatchret;
			return RV();
		}

		RV MeshType::on_load_data(Asset::IAsset* target_asset, const Variant& data, const Variant& params)
		{
			return load_data(target_asset, data);
		}
		RV MeshType::on_load_procedural_data(Asset::IAsset* target_asset, const Variant& params)
		{
			return load_data(target_asset, params);
		}
		void MeshType::on_unload_data(Asset::IAsset* target_asset)
		{
			P<Mesh> mesh = target_asset;
			mesh->m_vb = nullptr;
			mesh->m_ib = nullptr;
			mesh->m_vb_count = 0;
			mesh->m_ib_count = 0;
			mesh->m_pieces.clear();
		}
		R<Variant> MeshType::on_save_data(Asset::IAsset* target_asset, const Variant& params)
		{
			Variant data;
			lutry
			{
				P<Mesh> mesh = target_asset;
				data = Variant(EVariantType::table);
				auto pieces_field = Variant(EVariantType::variant, mesh->m_pieces.size());
				auto pieces_data = pieces_field.to_var_buf();
				data.set_field(0, Name("pieces"), pieces_field);
				for (u32 i = 0; i < mesh->m_pieces.size(); ++i)
				{
					auto piece = Variant(EVariantType::table);
					auto offset = Variant(EVariantType::u32);
					auto size = Variant(EVariantType::u32);
					offset.to_u32() = mesh->m_pieces[i].m_offset;
					size.to_u32() = mesh->m_pieces[i].m_size;
					piece.set_field(0, Name("offset"), offset);
					piece.set_field(0, Name("size"), size);
					pieces_data[i] = piece;
				}

				// Read back vertex & index data from resource.
				using namespace Gfx;
				lulet(vb_res, Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::readback, EResourceUsageFlag::none, mesh->m_vb_count * sizeof(Vertex))));
				lulet(ib_res, Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::readback, EResourceUsageFlag::none, mesh->m_ib_count * sizeof(u32))));

				lulet(cmdbuf, Renderer::main_copy_queue()->new_command_buffer());
				cmdbuf->copy_resource(vb_res, mesh->m_vb);
				cmdbuf->copy_resource(ib_res, mesh->m_ib);
				luexp(cmdbuf->submit());
				cmdbuf->wait();

				lulet(vb_mapped, vb_res->map_subresource(0, true));
				lulet(ib_mapped, ib_res->map_subresource(0, true));

				auto vert = Variant(EVariantType::blob);
				vert.to_blob() = Blob(vb_mapped, mesh->m_vb_count * sizeof(Vertex));
				auto index = Variant(EVariantType::blob);
				index.to_blob() = Blob(ib_mapped, mesh->m_ib_count * sizeof(u32));
				data.set_field(0, Name("vertices"), vert);
				data.set_field(0, Name("indices"), index);
			}
			lucatchret;

			return data;
		}

		Unconstructed<MeshType> g_mesh_type;
	}
}