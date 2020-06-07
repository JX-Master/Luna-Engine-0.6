// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Mesh.hpp
* @author JXMaster
* @date 2020/5/10
*/
#pragma once
#include "3DEngineHeader.hpp"

namespace luna
{
	namespace e3d
	{
		class Mesh : public IMesh
		{
		public:
			lucid("{fa5639f6-49d0-41e3-b403-9d216ddf876d}");
			luiimpl(Mesh, IMesh, asset::IAsset, IObject);

			P<asset::IAssetMeta> m_meta;

			P<gfx::IResource> m_vb;
			P<gfx::IResource> m_ib;

			uint32 m_vb_count;
			uint32 m_ib_count;

			struct Piece
			{
				uint32 m_offset;	// The index of first uint32's in the buffer.
				uint32 m_size;	// Number of uint32's
			};
			
			Vector<Piece> m_pieces;

			Mesh(IAllocator* alloc) :
				luibind(alloc),
				m_pieces(alloc) {}

			virtual asset::IAssetMeta* meta() override
			{
				return m_meta;
			}
			virtual void reset(IBlob* vertex_data, IBlob* index_data, uint32 num_pieces, const MeshPieceDesc* pieces) override;
			virtual P<gfx::IResource> vertex_buffer() override
			{
				luassert_msg_usr(meta()->state() == asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				return m_vb;
			}
			virtual P<gfx::IResource> index_buffer() override
			{
				luassert_msg_usr(meta()->state() == asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				return m_ib;
			}
			virtual uint32 count_vertices() override
			{
				luassert_msg_usr(meta()->state() == asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				return m_vb_count;
			}
			virtual uint32 count_indices() override
			{
				luassert_msg_usr(meta()->state() == asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				return m_ib_count;
			}
			virtual uint32 count_pieces() override
			{
				luassert_msg_usr(meta()->state() == asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				return (uint32)m_pieces.size();
			}
			virtual uint32 piece_first_index_offset(uint32 piece_index) override
			{
				luassert_msg_usr(meta()->state() == asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				luassert_usr(piece_index < count_pieces());
				return m_pieces[piece_index].m_offset;
			}
			virtual uint32 piece_count_indices(uint32 piece_index) override
			{
				luassert_msg_usr(meta()->state() == asset::EAssetState::loaded, "This call must be called when the mesh is loaded.");
				luassert_usr(piece_index < count_pieces());
				return m_pieces[piece_index].m_size;
			}
		};

		class MeshType : public asset::IAssetType
		{
		public:
			lucid("{d91c05f9-ba18-49c1-b57e-42d67b8c589b}");
			luiimpl_static(MeshType, asset::IAssetType, IObject);

			P<IName> m_mesh_type_name;

			MeshType() :
				m_mesh_type_name(intern_name("Mesh")) {}

			virtual IName* type_name() override
			{
				return m_mesh_type_name;
			}
			virtual P<asset::IAsset> on_new_asset(asset::IAssetMeta* meta) override;
			virtual RV on_load_data(asset::IAsset* target_asset, IVariant* data, IVariant* params) override;
			virtual RV on_load_procedural_data(asset::IAsset* target_asset, IVariant* params) override;
			virtual void on_unload_data(asset::IAsset* target_asset) override;
			virtual RP<IVariant> on_save_data(asset::IAsset* target_asset, IVariant* params) override;
			virtual void on_dependency_data_load(asset::IAsset* current_asset, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(asset::IAsset* current_asset, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(asset::IAsset* current_asset, const Guid& before, const Guid& after) override {}
		};

		extern Unconstructed<MeshType> g_mesh_type;
	}
}