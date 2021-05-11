// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IMesh.hpp
* @author JXMaster
* @date 2020/5/10
*/
#pragma once
#include <Asset/Asset.hpp>
#include <Gfx/Gfx.hpp>

namespace Luna
{
	namespace E3D
	{
		//! The vertex format.
		struct Vertex
		{
			Float3U position;
			Float3U normal;
			Float3U tangent;
			Float2U texcoord;
			Float4U color;
		};

		static_assert(sizeof(Vertex) == 60, "Wrong Vertex struct size");

		struct MeshPieceDesc
		{
			u32 first_index_offset;
			u32 num_indices;
		};

		//! @interface IMesh
		//! Represents a mesh asset.
		//! One mesh asset is combined by serial pieces, all pieces shares the same vertex buffer but each of them has 
		//! its independent index buffer that refers to a different range of vertex and can be assigned with its own 
		//! material in model asset.
		//! 
		//! The mesh is updated to GPU as two buffers: one buffer for vertex data and one buffer for index data. All 
		//! pieces are grouped in one index buffer and can be indexed by its offset from the base address.
		struct IMesh : public Asset::IAsset
		{
			luiid("{8d9cc712-702e-4050-a77b-d739fb9c8d47}");

			//! Resets the mesh with the provided vertex and indices data. This call is asynchronous and will reload the 
			//! asset.
			//! 
			//! @param[in] vertex_data The vertex data passed in. The data is interpreted as const Vertex* array. The system keeps a strong
			//! reference to this blob until the asset is reloaded.
			//! @param[in] index_data The index data passed in. The data is interpreted as const u32* array. The system keeps a strong
			//! reference to this blob until the asset is reloaded.
			virtual void reset(const Blob& vertex_data, const Blob& index_data, u32 num_pieces, const MeshPieceDesc* pieces) = 0;

			//! Gets the vertex buffer. This call must be called when the mesh is loaded.
			virtual P<Gfx::IResource> vertex_buffer() = 0;

			//! Gets the index buffer. This call must be called when the mesh is loaded.
			virtual P<Gfx::IResource> index_buffer() = 0;

			//! Gets the number of vertices in the vertex buffer.
			virtual u32 count_vertices() = 0;

			//! Gets the number of indices in the index buffer.
			virtual u32 count_indices() = 0;

			//! Gets the number of pieces. This call must be called when the mesh is loaded.
			virtual u32 count_pieces() = 0;
			
			//! Gets the offset of the first index for the specified piece. This call must be called when the mesh is loaded.
			//! The offset is counted by `n` uint32s from the start of the index buffer, not by `n` bytes.
			virtual u32 piece_first_index_offset(u32 piece_index) = 0;

			//! Gets the number of indices for the specified piece. This call must be called when the mesh is loaded.
			virtual u32 piece_count_indices(u32 piece_index) = 0;
		};
	}
}