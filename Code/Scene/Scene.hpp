// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Scene.hpp
* @author JXMaster
* @date 2020/4/13
*/
#pragma once
#include "IScene.hpp"
#include "IEntity.hpp"
#include "IComponentType.hpp"
#include "ISceneComponentType.hpp"

#ifndef LUNA_SCENE_API
#define LUNA_SCENE_API
#endif

namespace Luna
{
	namespace Scene
	{
		//! Registers a new component type to the manager. 
		//! The manager keeps a strong reference to the component type.
		//! @param[in] component_type The component type object to register.
		//! @return Returns 
		LUNA_SCENE_API RV register_component_type(IComponentType* component_type);

		//! Unregisters a component type.
		LUNA_SCENE_API RV unregister_component_type(IComponentType* component_type);

		//! Registers a new scene component type to the manager. 
		//! The manager keeps a strong reference to the component type.
		//! @param[in] component_type The component type object to register.
		//! @return Returns 
		LUNA_SCENE_API RV register_scene_component_type(ISceneComponentType* component_type);

		//! Unregisters a scene component type.
		LUNA_SCENE_API RV unregister_scene_component_type(ISceneComponentType* component_type);

		//! Gets a list of all registered component types.
		LUNA_SCENE_API Vector<IComponentType*> component_types();

		//! Gets a list of all registered scene component types.
		LUNA_SCENE_API Vector<ISceneComponentType*> scene_component_types();

		//! Creates a new scene asset. 
		//! This call behaves the same as calling `asset::new_asset` with "Scene" type.
		LUNA_SCENE_API RP<IScene> new_scene();
	}
}