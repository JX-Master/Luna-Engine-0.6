// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IEntity.hpp
* @author JXMaster
* @date 2020/4/13
*/
#pragma once
#include <Core/Core.hpp>
#include "IComponent.hpp"

namespace Luna
{
	namespace Scene
	{
		struct IScene;

		//! @interface IEntity
		struct IEntity : public IObject
		{
			luiid("{d70d7547-b03e-4a96-bbac-64c0c868d936}");

			//! Gets the scene this entity belongs to.
			virtual P<IScene> belonging_scene() = 0;

			//! Gets the name of this entity.
			virtual Name name() = 0;
			
			//! Changes the name of this entity.
			virtual RV set_name(const Name& name) = 0;

			//! Creates and attaches a new component to this entity.
			//! @param[in] component_name The name of the component type.
			//! @return Returns the new added component instance. If there is already a component attached, this function fails.
			virtual R<IComponent*> add_component(const Name& component_type) = 0;

			//! Detaches one component with the specified type. The component will be deleted when detached.
			//! @param[in] component_name The name of the component type.
			virtual RV remove_component(const Name& component_type) = 0;

			//! Detaches all components.
			virtual void clear_components() = 0;

			//! Gets the component with the specified type.
			virtual R<IComponent*> get_component(const Name& type_name) = 0;

			//! Gets the component with the specified type, or creates a default one if it is not exist.
			virtual R<IComponent*> add_or_get_component(const Name& type_name) = 0;

			//! Gets a vector of all components attached to this entity.
			virtual Vector<IComponent*> components() = 0;
		};
	}
}