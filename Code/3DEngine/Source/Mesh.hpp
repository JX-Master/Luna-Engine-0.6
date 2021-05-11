// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Mesh.hpp
* @author JXMaster
* @date 2020/5/10
*/
#pragma once
#include "3DEngineHeader.hpp"

namespace Luna
{
	namespace E3D
	{
		class Mesh : public IMesh
		{
		public:
			lucid("{fa5639f6-49d0-41e3-b403-9d216ddf876d}");
			luiimpl(Mesh, IMesh, Asset::IAsset, IObject);

			P<Asset::IAssetMeta> m_meta;

			P<Gfx::IResource> m_vb;
			P<Gfx::IResource> m_ib;

			u32 m_vb_count;
			u32 m_ib_count;

			struct Piece
			{
				u32 m_offset;	// The index of first u32's in the buffer.
				u32 m_size;	// Number of u32's
			};
			
			Vector<Piece> m_pieces;

			Mesh() {}

			virtual Asset::IAssetMeta* meta() override
			{
				return m_meta;
			}
			virtual void reset(const Blob& vertex_data, const Blob& index_data, u32 num_pieces, const MeshPieceDesc* pieces) override;
			virtual P<Gfx::IResource> vertex_buffer() override
			{
				lucheck_msg(meta()->state() == Asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				return m_vb;
			}
			virtual P<Gfx::IResource> index_buffer() override
			{
				lucheck_msg(meta()->state() == Asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				return m_ib;
			}
			virtual u32 count_vertices() override
			{
				lucheck_msg(meta()->state() == Asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				return m_vb_count;
			}
			virtual u32 count_indices() override
			{
				lucheck_msg(meta()->state() == Asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				return m_ib_count;
			}
			virtual u32 count_pieces() override
			{
				lucheck_msg(meta()->state() == Asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				return (u32)m_pieces.size();
			}
			virtual u32 piece_first_index_offset(u32 piece_index) override
			{
				lucheck_msg(meta()->state() == Asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				lucheck(piece_index < count_pieces());
				return m_pieces[piece_index].m_offset;
			}
			virtual u32 piece_count_indices(u32 piece_index) override
			{
				lucheck_msg(meta()->state() == Asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				lucheck(piece_index < count_pieces());
				return m_pieces[piece_index].m_size;
			}
		};

		class MeshType : public Asset::IAssetType
		{
		public:
			lucid("{d91c05f9-ba18-49c1-b57e-42d67b8c589b}");
			luiimpl_static(MeshType, Asset::IAssetType, IObject);

			Name m_mesh_type_name;

			MeshType() :
				m_mesh_type_name(Name("Mesh")) {}

			virtual Name type_name() override
			{
				return m_mesh_type_name;
			}
			virtual P<Asset::IAsset> on_new_asset(Asset::IAssetMeta* meta) override;
			virtual RV on_load_data(Asset::IAsset* target_asset, const Variant& data, const Variant& params) override;
			virtual RV on_load_procedural_data(Asset::IAsset* target_asset, const Variant& params) override;
			virtual void on_unload_data(Asset::IAsset* target_asset) override;
			virtual R<Variant> on_save_data(Asset::IAsset* target_asset, const Variant& params) override;
			virtual void on_dependency_data_load(Asset::IAsset* current_asset, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(Asset::IAsset* current_asset, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(Asset::IAsset* current_asset, const Guid& before, const Guid& after) override {}
		};

		extern Unconstructed<MeshType> g_mesh_type;
	}
}