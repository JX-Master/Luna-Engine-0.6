// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IObjFile.hpp
* @author JXMaster
* @date 2020/5/12
*/
#pragma once
#include <Core/Core.hpp>
#include <Base/Math.hpp>

namespace luna
{
	namespace obj_loader
	{
		// Index struct to support different indices for vtx/normal/texcoord.
		// -1 means not used.
		struct Index
		{
			int32 vertex_index;
			int32 normal_index;
			int32 texcoord_index;
		};

		struct Mesh
		{
			Vector<Index> indices;
			//! The number of vertices per
			//! face. 3 = triangle, 4 = quad,
			//! ... Up to 255 vertices per face.
			Vector<uint8> num_face_vertices;
			//! per-face material ID
			Vector<int32> material_ids;
			// per-face smoothing group
			// ID(0 = off. positive value
			// = group id)
			Vector<uint32> smoothing_group_ids;
		};

		//! Linear flattened indices.
		struct Lines
		{
			//! indices for vertices(polygon lines)
			Vector<Index> indices;
			//! The number of vertices per line.
			Vector<int32> num_line_vertices;
		};

		struct Points
		{
			//! indices for points
			Vector<Index> indices;
		};

		struct Shape
		{
			P<IName> name;
			Mesh mesh;
			Lines lines;
			Points points;
		};

		struct Attributes
		{
			Vector<Float3U> vertices;
			Vector<Float3U> normals;
			Vector<Float2U> texcoords;
			Vector<Float3U> colors;
		};

		//! @interface IObjFile
		//! Represents a parsed obj file.
		struct IObjFile : public IObject
		{
			luiid("{25ef4784-610d-4b29-9bf9-56ee2635aec0}");

			//! Vertex attributes.
			virtual Attributes& attributes() = 0;

			//! Gets the shapes array.
			virtual Vector<Shape>& shapes() = 0;
		};
	}
}