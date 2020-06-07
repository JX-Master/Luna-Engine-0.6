// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Lights.hpp
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
		class DirectionalLight : public IDirectionalLight
		{
		public:
			lucid("{5eba04da-2816-45a8-a394-b30a93845c99}");
			luiimpl(DirectionalLight, IDirectionalLight, ILight, scene::IComponent, ISerializable, IObject);
			lutsassert_lock();

			DirectionalLight(IAllocator* alloc) :
				luibind(alloc) {}

			WP<scene::IEntity> m_entity;

			Float3 m_intensity = { 0.5f, 0.5f, 0.5f };

			float32 m_intensity_multiplier = 1.0f;

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
			virtual Float3 intensity() override
			{
				lutsassert();
				return m_intensity;
			}
			virtual void set_intensity(const Float3& intensity_value) override
			{
				lutsassert();
				m_intensity = intensity_value;
			}
			virtual float32 intensity_multiplier() override
			{
				lutsassert();
				return m_intensity_multiplier;
			}
			virtual void set_intensity_multiplier(float32 multiplier) override
			{
				lutsassert();
				m_intensity_multiplier = multiplier;
			}
		};

		class PointLight : public IPointLight
		{
		public:
			lucid("{7637531a-29f8-4d5a-8f19-f9a103a62bdf}");
			luiimpl(PointLight, IPointLight, ILight, scene::IComponent, ISerializable, IObject);
			lutsassert_lock();

			PointLight(IAllocator* alloc) :
				luibind(alloc) {}

			WP<scene::IEntity> m_entity;

			Float3 m_intensity = { 0.5f, 0.5f, 0.5f };
			float32 m_intensity_multiplier = 1.0f;
			float32 m_attenuation_power = 1.0f;

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
			virtual Float3 intensity() override
			{
				lutsassert();
				return m_intensity;
			}
			virtual void set_intensity(const Float3& intensity_value) override
			{
				lutsassert();
				m_intensity = intensity_value;
			}
			virtual float32 intensity_multiplier() override
			{
				return m_intensity_multiplier;
			}
			virtual void set_intensity_multiplier(float32 multiplier) override
			{
				lutsassert();
				m_intensity_multiplier = multiplier;
			}
			virtual float32 attenuation_power() override
			{
				lutsassert();
				return m_attenuation_power;
			}
			virtual void set_attenuation_power(float32 value) override
			{
				lutsassert();
				m_attenuation_power = value;
			}
		};

		class SpotLight : public ISpotLight
		{
		public:
			lucid("{7637531a-29f8-4d5a-8f19-f9a103a62bdf}");
			luiimpl(SpotLight, ISpotLight, ILight, scene::IComponent, ISerializable, IObject);
			lutsassert_lock();

			SpotLight(IAllocator* alloc) :
				luibind(alloc) {}

			WP<scene::IEntity> m_entity;

			Float3 m_intensity = { 0.5f, 0.5f, 0.5f }; 
			float32 m_intensity_multiplier = 1.0f;
			float32 m_attenuation_power = 1.0f;
			float32 m_spot_power = 64.0f;

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
			virtual Float3 intensity() override
			{
				lutsassert();
				return m_intensity;
			}
			virtual void set_intensity(const Float3& intensity_value) override
			{
				lutsassert();
				m_intensity = intensity_value;
			}
			virtual float32 intensity_multiplier() override
			{
				lutsassert();
				return m_intensity_multiplier;
			}
			virtual void set_intensity_multiplier(float32 multiplier) override
			{
				lutsassert();
				m_intensity_multiplier = multiplier;
			}
			virtual float32 attenuation_power() override
			{
				lutsassert();
				return m_attenuation_power;
			}
			virtual float32 spot_power() override
			{
				lutsassert();
				return m_spot_power;
			}
			virtual void set_attenuation_power(float32 value) override
			{
				lutsassert();
				m_attenuation_power = value;
			}
			virtual void set_spot_power(float32 value) override
			{
				lutsassert();
				m_spot_power = value;
			}
		};

		class DirectionalLightType : public scene::IComponentType
		{
		public:
			lucid("{0c8d30d3-a020-4e8d-86d4-114fcdfd51c1}");
			luiimpl_static(DirectionalLightType, scene::IComponentType, IObject);

			P<IName> m_type_name;

			DirectionalLightType() :
				m_type_name(intern_name("Directional Light")) {}

			virtual IName* type_name() override
			{
				return m_type_name;
			}
			virtual RP<scene::IComponent> new_component(scene::IEntity* belonging_entity) override
			{
				auto t = box_ptr(new_obj_aligned<DirectionalLight>(get_module_allocator()));
				t->m_entity = belonging_entity;
				return t;
			}
			virtual void on_dependency_data_load(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(scene::IComponent* component, const Guid& before, const Guid& after) override {}
		};

		extern Unconstructed<DirectionalLightType> g_directional_light_type;

		class PointLightType : public scene::IComponentType
		{
		public:
			lucid("{792a6da7-c9fd-4537-a5d2-804e327adac9}");
			luiimpl_static(PointLightType, scene::IComponentType, IObject);

			P<IName> m_type_name;

			PointLightType() :
				m_type_name(intern_name("Point Light")) {}

			virtual IName* type_name() override
			{
				return m_type_name;
			}
			virtual RP<scene::IComponent> new_component(scene::IEntity* belonging_entity) override
			{
				auto t = box_ptr(new_obj_aligned<PointLight>(get_module_allocator()));
				t->m_entity = belonging_entity;
				return t;
			}
			virtual void on_dependency_data_load(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(scene::IComponent* component, const Guid& before, const Guid& after) override {}
		};

		extern Unconstructed<PointLightType> g_point_light_type;

		class SpotlLightType : public scene::IComponentType
		{
		public:
			lucid("{f3c74df5-1fc7-4ea8-bdaf-030b362db645}");
			luiimpl_static(SpotlLightType, scene::IComponentType, IObject);

			P<IName> m_type_name;

			SpotlLightType() : 
				m_type_name(intern_name("Spot Light")) {}

			virtual IName* type_name() override
			{
				return m_type_name;
			}
			virtual RP<scene::IComponent> new_component(scene::IEntity* belonging_entity) override
			{
				auto t = box_ptr(new_obj_aligned<SpotLight>(get_module_allocator()));
				t->m_entity = belonging_entity;
				return t;
			}
			virtual void on_dependency_data_load(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(scene::IComponent* component, const Guid& before, const Guid& after) override {}
		};

		extern Unconstructed<SpotlLightType> g_spot_light_type;
	}
}