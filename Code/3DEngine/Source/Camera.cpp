// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Camera.cpp
* @author JXMaster
* @date 2020/5/20
*/
#include "Camera.hpp"

namespace luna
{
	namespace e3d
	{
		Unconstructed<CameraType> g_camera_type;

		RP<IVariant> Camera::serialize()
		{
			lutsassert();
			P<IVariant> var = new_var(EVariantType::table);
			auto type = new_var(EVariantType::name);
			if (m_type == ECameraType::perspective)
			{
				type->name() = intern_name("perspective");
			}
			else
			{
				type->name() = intern_name("orthographic");
			}
			var->set_field(0, intern_name("type"), type);
			auto fov = new_var(EVariantType::f32);
			fov->f32() = m_fov;
			var->set_field(0, intern_name("fov"), fov);
			auto size = new_var(EVariantType::f32);
			size->f32() = m_size;
			var->set_field(0, intern_name("size"), size);
			auto near_clipping_plane = new_var(EVariantType::f32);
			near_clipping_plane->f32() = m_near_clipping_plane;
			var->set_field(0, intern_name("near_clipping_plane"), near_clipping_plane);
			auto far_clipping_plane = new_var(EVariantType::f32);
			far_clipping_plane->f32() = m_far_clipping_plane;
			var->set_field(0, intern_name("far_clipping_plane"), far_clipping_plane);
			return var;
		}

		RV Camera::deserialize(IVariant* obj)
		{
			lutsassert();
			lutry
			{
				lulet(type, obj->field(0, intern_name("type")));
				lulet(fov, obj->field(0, intern_name("fov")));
				lulet(size, obj->field(0, intern_name("size")));
				lulet(near_clipping_plane, obj->field(0, intern_name("near_clipping_plane")));
				lulet(far_clipping_plane, obj->field(0, intern_name("far_clipping_plane")));
				lulet(type_value, type->check_name());
				m_type = type_value.get() == intern_name("perspective") ? ECameraType::perspective : ECameraType::orthographic;
				luset(m_fov, fov->check_f32());
				luset(m_size, size->check_f32());
				luset(m_near_clipping_plane, near_clipping_plane->check_f32());
				luset(m_far_clipping_plane, far_clipping_plane->check_f32());
			}
			lucatchret;
			return s_ok;
		}

		scene::IComponentType* Camera::type_object()
		{
			return &g_camera_type.get();
		}

		RP<scene::IComponent> CameraType::new_component(scene::IEntity* belonging_entity)
		{
			auto e = new_obj<Camera>(get_module_allocator());
			e->m_entity = belonging_entity;
			return e;
		}
	}
}