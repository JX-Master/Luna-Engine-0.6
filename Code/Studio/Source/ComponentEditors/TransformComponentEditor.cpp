// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TransformComponentEditor.cpp
* @author JXMaster
* @date 2020/5/18
*/
#include "TransformComponentEditor.hpp"

namespace Luna
{
	namespace editor
	{
		void TransformComponentEditor::on_render(ImGui::IContext* ctx)
		{
			P<E3D::ITransform> t = m_component.lock();

			auto position = t->local_position();
			ctx->drag_float3("Position", position.m, 0.01f);
			if (ctx->is_item_edited())
			{
				t->set_local_position(position);
			}

			auto rotation = t->local_rotation();
			auto cached_rotation = Quaternion::from_roll_pitch_yaw(m_cached_rotation_eular.x, m_cached_rotation_eular.y, m_cached_rotation_eular.z);
			if (rotation != cached_rotation)
			{
				// Update cache.
				auto rot_mat = Float4x4::make_rotation_quaternion(rotation);
				auto angle = rot_mat.euler_angles();
				m_cached_rotation_eular = angle;
			}
			Float3 angle = m_cached_rotation_eular;
			angle *= 180.0f / pi;
			ctx->drag_float3("Rotation", angle.m, 1.0f);
			angle *= pi / 180.0f;
			if (ctx->is_item_edited())
			{
				m_cached_rotation_eular = angle;
				auto new_rot = Quaternion::from_roll_pitch_yaw(angle.x, angle.y, angle.z);
				t->set_local_rotation(new_rot);
			}
			
			auto scale = t->local_scale();
			ctx->drag_float3("Scale", scale.m, 0.01f);
			if (ctx->is_item_edited())
			{
				t->set_local_scale(scale);
			}
		}
	}
}