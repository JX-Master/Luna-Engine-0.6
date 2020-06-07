// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file CameraComponentEditor.cpp
* @author JXMaster
* @date 2020/5/20
*/
#include "CameraComponentEditor.hpp"

namespace luna
{
	namespace editor
	{
		void CameraComponentEditor::on_render(imgui::IContext* ctx)
		{
			P<e3d::ICamera> c = m_component.lock();

			const char* types[] = {
				"Perspective",
				"Orthographic",
			};

			e3d::ECameraType type = c->camera_type();
			ctx->combo("Camera Type", (int32*)&type, types, 2);
			c->set_camera_type(type);

			if (type == e3d::ECameraType::orthographic)
			{
				float32 size = c->size();
				ctx->drag_float("Size", &size);
				c->set_size(size);
			}
			else
			{
				float32 fov = c->field_of_view();
				fov = rad_to_deg(fov);
				ctx->drag_float("Field of View", &fov, 1.0f, 10.0f, 175.0f);
				c->set_field_of_view(deg_to_rad(fov));
			}

			float32 near_plane = c->near_clipping_plane();
			float32 far_plane = c->far_clipping_plane();
			ctx->drag_float("Near Clipping Plane", &near_plane, 1.0f, 0.0000001f, far_plane);
			ctx->drag_float("Far Clipping Plane", &far_plane, 1.0f, near_plane, FLT_MAX);
			c->set_near_clipping_plane(near_plane);
			c->set_far_clipping_plance(far_plane);
		}
	}
}