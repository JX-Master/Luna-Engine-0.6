// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Entity.hpp
* @author JXMaster
* @date 2020/5/7
*/
#pragma once
#include "SceneHeader.hpp"
#include <Runtime/TSAssert.hpp>
#include <Core/Interface.hpp>

namespace Luna
{
	namespace Scene
	{
		class Entity : public IEntity
		{
		public:
			lucid("{348918ca-22d8-47dc-9207-a06a1a8d21bf}");
			luiimpl(Entity, IEntity, IObject);
			lutsassert_lock();

			WP<IScene> m_belonging_scene;
			Name m_name;
			Vector<P<IComponent>> m_components;

			Entity() {}

			R<Variant> serialize();
			// Create the components but leaves their data uninitialized.
			RV pre_deserialize(const Variant& obj);
			// Initializes the data of the components.
			RV deserialize(const Variant& obj);
			virtual P<IScene> belonging_scene() override;
			virtual Name name() override;
			virtual RV set_name(const Name& name) override;
			virtual R<IComponent*> add_component(const Name& component_type) override;
			virtual RV remove_component(const Name& component_type) override;
			virtual void clear_components() override;
			virtual R<IComponent*> get_component(const Name& type_name) override;
			virtual R<IComponent*> add_or_get_component(const Name& type_name) override
			{
				auto comp = get_component(type_name);
				if (failed(comp))
				{
					comp = add_component(type_name);
				}
				return comp;
			}
			virtual Vector<IComponent*> components() override;
		};
	}
}