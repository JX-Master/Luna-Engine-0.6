// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ITransform.hpp
* @author JXMaster
* @date 2020/5/14
*/
#pragma once
#include <Scene/Scene.hpp>
#include <Base/Math.hpp>

namespace luna
{
	namespace e3d
	{
		struct ITransform : public scene::IComponent
		{
		public:
			luiid("{5068efe9-4307-44db-ab73-cd1780ccdd55}");

			//! Gets the parent transform, or `nullptr` if this transform does not have a parent.
			//! The transform keeps a weak reference to its parent.
			virtual P<ITransform> parent() = 0;

			//! Gets the children.
			//! The transform keeps a weak reference to its children.
			virtual Vector<P<ITransform>> children() = 0;

			//! Removes one child.
			virtual RV remove_child(ITransform* child) = 0;

			//! Removes all children.
			virtual void remove_all_children() = 0;

			//! Gets the local position of this transform relative to its parent.
			virtual Float3 local_position() = 0;
			//! Gets the local rotation of this transform relative to its parent.
			virtual Quaternion local_rotation() = 0;
			//! Gets the local scale of this transform relative to its parent.
			virtual Float3 local_scale() = 0;
			//! Sets the local position.
			virtual void set_local_position(const Float3& position) = 0;
			//! Sets the local rotation.
			virtual void set_local_rotation(const Quaternion& rotation) = 0;
			//! Sets the local scale.
			virtual void set_local_scale(const Float3& scale) = 0;

			//! Gets the world position of this transform relative to its parent.
			virtual Float3 world_position() = 0;
			//! Gets the world rotation of this transform relative to its parent.
			virtual Quaternion world_rotation() = 0;
			//! Sets the world position.
			virtual void set_world_position(const Float3& position) = 0;
			//! Gets the transform matrix that transforms a point in parent space to this space. 
			virtual Float4x4 parent_to_this_matrix() = 0;
			//! Gets the transform matrix that transforms a point in this space to parent space.
			virtual Float4x4 this_to_parent_matrix() = 0;
			//! Gets the local to world matrix.
			virtual Float4x4 local_to_world_matrix() = 0;
			//! Gets the world to local matrix.
			virtual Float4x4 world_to_local_matrix() = 0;

			//! Sets the parent to this matrix.
			virtual void set_parent_to_this_matrix(const Float4x4& mat) = 0;
			//! Sets the this to parent matrix.
			virtual void set_this_to_parent_matrix(const Float4x4& mat) = 0;
			//! Sets the local to world matrix.
			virtual void set_local_to_world_matrix(const Float4x4& mat) = 0;
			//! Sets the world to local matrix.
			virtual void set_world_to_local_matrix(const Float4x4& mat) = 0;
		};
	}
}