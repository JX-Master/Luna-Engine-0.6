// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IComponentType.hpp
* @author JXMaster
* @date 2020/4/13
*/
#pragma once
#include "IComponent.hpp"

namespace luna
{
	namespace scene
	{
		//! @interface IComponentType
		//! Stores the type information of one component and the data shared by all components.
		//! Implement this and register to the scene system to add a new component type.
		struct IComponentType : public IObject
		{
			luiid("{45bd9a58-564f-4a53-b73b-b59e0d567ffd}");

			//! Gets the component type name of this component type.
			virtual IName* type_name() = 0;

			//! Creates a new component with default-initialized data.
			//! @param[in] belonging_entity The entity to attach the component to. 
			//! The component should keep a weak reference to the entity instance, this will not be changed after the component
			//! is created.
			virtual RP<IComponent> new_component(IEntity* belonging_entity) = 0;

			//! Called when the data of one of the dependency assets of this component is loaded.
			virtual void on_dependency_data_load(IComponent* component, asset::IAsset* dependency_asset) = 0;

			//! Called when the data of one of the dependency assets is about to be unloaded.
			virtual void on_dependency_data_unload(IComponent* component, asset::IAsset* dependency_asset) = 0;

			//! Called when the data of one of the dependency assets is going to be replaced.
			virtual void on_dependency_replace(IComponent* component, const Guid& before, const Guid& after) = 0;
		};
	}
}