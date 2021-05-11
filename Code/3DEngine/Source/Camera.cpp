// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Camera.cpp
* @author JXMaster
* @date 2020/5/20
*/
#include "Camera.hpp"

namespace Luna
{
	namespace E3D
	{
		Unconstructed<CameraType> g_camera_type;

		R<Variant> Camera::serialize()
		{
			lutsassert();
			Variant var = Variant(EVariantType::table);
			auto type = Variant(EVariantType::name);
			if (m_type == ECameraType::perspective)
			{
				type.to_name() = Name("perspective");
			}
			else
			{
				type.to_name() = Name("orthographic");
			}
			var.set_field(0, Name("type"), type);
			auto fov = Variant(EVariantType::f32);
			fov.to_f32() = m_fov;
			var.set_field(0, Name("fov"), fov);
			auto size = Variant(EVariantType::f32);
			size.to_f32() = m_size;
			var.set_field(0, Name("size"), size);
			auto near_clipping_plane = Variant(EVariantType::f32);
			near_clipping_plane.to_f32() = m_near_clipping_plane;
			var.set_field(0, Name("near_clipping_plane"), near_clipping_plane);
			auto far_clipping_plane = Variant(EVariantType::f32);
			far_clipping_plane.to_f32() = m_far_clipping_plane;
			var.set_field(0, Name("far_clipping_plane"), far_clipping_plane);
			return var;
		}

		RV Camera::deserialize(const Variant& obj)
		{
			lutsassert();
			lutry
			{
				auto& type = obj.field(0, Name("type"));
				auto& fov = obj.field(0, Name("fov"));
				auto& size = obj.field(0, Name("size"));
				auto& near_clipping_plane = obj.field(0, Name("near_clipping_plane"));
				auto& far_clipping_plane = obj.field(0, Name("far_clipping_plane"));
				lulet(type_value, type.check_name());
				m_type = type_value.get() == Name("perspective") ? ECameraType::perspective : ECameraType::orthographic;
				luset(m_fov, fov.check_f32());
				luset(m_size, size.check_f32());
				luset(m_near_clipping_plane, near_clipping_plane.check_f32());
				luset(m_far_clipping_plane, far_clipping_plane.check_f32());
			}
			lucatchret;
			return RV();
		}

		Scene::IComponentType* Camera::type_object()
		{
			return &g_camera_type.get();
		}

		RP<Scene::IComponent> CameraType::new_component(Scene::IEntity* belonging_entity)
		{
			auto e = newobj<Camera>();
			e->m_entity = belonging_entity;
			return e;
		}
	}
}