// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Entity.hpp
* @author JXMaster
* @date 2020/5/7
*/
#pragma once
#include "SceneHeader.hpp"
#include <Base/TSAssert.hpp>

namespace luna
{
	namespace scene
	{
		class Entity : public IEntity
		{
		public:
			lucid("{348918ca-22d8-47dc-9207-a06a1a8d21bf}");
			luiimpl(Entity, IEntity, IObject);
			lutsassert_lock();

			WP<IScene> m_belonging_scene;
			P<IName> m_name;
			Vector<P<IComponent>> m_components;

			Entity(IAllocator* alloc) :
				luibind(alloc),
				m_components(alloc) {}

			RP<IVariant> serialize();
			// Create the components but leaves their data uninitialized.
			RV pre_deserialize(IVariant* obj);
			// Initializes the data of the components.
			RV deserialize(IVariant* obj);
			virtual P<IScene> belonging_scene() override;
			virtual IName* name() override;
			virtual RV set_name(IName* name) override;
			virtual R<IComponent*> add_component(IName* component_type) override;
			virtual RV remove_component(IName* component_type) override;
			virtual void clear_components() override;
			virtual R<IComponent*> get_component(IName* type_name) override;
			virtual R<IComponent*> add_or_get_component(IName* type_name) override
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