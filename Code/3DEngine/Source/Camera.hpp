// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Camera.hpp
* @author JXMaster
* @date 2020/5/20
*/
#pragma once
#include "3DEngineHeader.hpp"
#include <Base/TSAssert.hpp>

namespace luna
{
	namespace e3d
	{
		class Camera : public ICamera
		{
		public:
			lucid("{7c388740-d97a-4e6c-9b7f-dc04e704629f}");
			luiimpl(Camera, ICamera, scene::IComponent, ISerializable, IObject);
			lutsassert_lock();

			WP<scene::IEntity> m_entity;

			ECameraType m_type = ECameraType::perspective;
			float32 m_fov = pi / 3.0f;
			float32 m_size = 5.0f;
			float32 m_near_clipping_plane = 0.3f;
			float32 m_far_clipping_plane = 1000.0f;
			float32 m_aspect_ratio = 16.0f / 9.0f;

			Camera(IAllocator* alloc) :
				luibind(alloc) {}

			virtual RP<IVariant> serialize() override;
			virtual RV deserialize(IVariant* obj) override;

			virtual scene::IComponentType* type_object() override;
			virtual P<scene::IEntity> belonging_entity() override
			{
				return m_entity.lock();
			}
			virtual Vector<Guid> referred_assets() override
			{
				return Vector<Guid>();
			}

			virtual ECameraType camera_type() override
			{
				lutsassert();
				return m_type;
			}
			virtual void set_camera_type(ECameraType type) override
			{
				lutsassert();
				m_type = type;
			}
			virtual float32 field_of_view() override
			{
				lutsassert();
				return m_fov;
			}
			virtual void set_field_of_view(float32 value) override
			{
				lutsassert();
				m_fov = value;
			}
			virtual float32 size() override
			{
				lutsassert();
				return m_size;
			}
			virtual void set_size(float32 value) override
			{
				lutsassert();
				m_size = value;
			}
			virtual float32 near_clipping_plane() override
			{
				lutsassert();
				return m_near_clipping_plane;
			}
			virtual float32 far_clipping_plane() override
			{
				lutsassert();
				return m_far_clipping_plane;
			}
			virtual void set_near_clipping_plane(float32 value) override
			{
				lutsassert();
				m_near_clipping_plane = value;
			}
			virtual void set_far_clipping_plance(float32 value) override
			{
				lutsassert();
				m_far_clipping_plane = value;
			}
			virtual float32 aspect_ratio() override
			{
				lutsassert();
				return m_aspect_ratio;
			}
			virtual void set_aspect_ratio(float32 aspect_ratio) override
			{
				lutsassert();
				m_aspect_ratio = aspect_ratio;
			}
			virtual Float4x4 projection_matrix() override
			{
				lutsassert();
				if (m_type == ECameraType::perspective)
				{
					return Float4x4::make_perspective_field_of_view(m_fov, m_aspect_ratio, m_near_clipping_plane, m_far_clipping_plane);
				}
				else
				{
					return Float4x4::make_orthographic(m_size, m_size / m_aspect_ratio, m_near_clipping_plane, m_far_clipping_plane);
				}
			}
		};

		class CameraType : public scene::IComponentType
		{
		public:
			lucid("{d458f87a-d9f4-4e1e-ab9e-5fd9f60dc8b2}");
			luiimpl_static(CameraType, scene::IComponentType, IObject);

			P<IName> m_name;

			CameraType() :
				m_name(intern_name("Camera")) {}

			virtual IName* type_name() override
			{
				return m_name;
			}
			virtual RP<scene::IComponent> new_component(scene::IEntity* belonging_entity) override;
			virtual void on_dependency_data_load(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(scene::IComponent* component, const Guid& before, const Guid& after) override {}
		};

		extern Unconstructed<CameraType> g_camera_type;
	}
}