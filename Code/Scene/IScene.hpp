// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IScene.hpp
* @author JXMaster
* @date 2020/4/13
*/
#pragma once
#include "IEntity.hpp"
#include "ISceneComponent.hpp"
#include <Asset/Asset.hpp>

namespace luna
{
	namespace scene
	{
		//! @interface IScene
		//! Represents a container that contains entities.
		//! 
		//! * Asset Type: "Scene"
		struct IScene : public asset::IAsset
		{
			luiid("{bb85dc5f-befa-44ea-be0c-a0ab32690d20}");

			//! Resets the scene. This loads the scene as an empty scene. 
			//! This call is asynchronous.
			virtual void reset() = 0;

			//! Creates one entity and adds it to this scene. The scene keeps a strong reference to the added 
			//! entity.
			//! 
			//! The entity cannot be detached from the scene, it is valid only in the scene context. Once the 
			//! entity is detached from the scene, it will be removed immediately. So any other object should
			//! use a weak reference to keep the entity.
			//! @param[in] entity_name The name of the entity to be added.
			//! @return Returns `s_ok` if the entity is added, returns `e_item_already_exist` if one entity
			//! with the same name already exists in the scene.
			virtual R<IEntity*> add_entity(IName* entity_name) = 0;

			//! Removes one entity from the scene.
			//! @param[in] entity_name The name of the entity to be removed.
			//! @return Returns `s_ok` if the entity is removed, returns `e_item_not_exist` if the entity
			//! specified does not exist in the scene.
			virtual RV remove_entity(IName* entity_name) = 0;

			//! Looks up one entity by its name.
			//! @param[in] name The name of the entity.
			virtual R<IEntity*> find_entity(IName* name) = 0;

			//! Gets a list of all entities in the scene.
			virtual Vector<IEntity*> entities() = 0;

			//! Removes all entities in the scene.
			virtual void clear_entities() = 0;

			//! Creates and adds one default-initialized scene component to the scene and returns the component instance.
			//! @param[in] component_type The type of the component to add.
			virtual R<ISceneComponent*> add_scene_component(IName* component_type) = 0;

			//! Removes one scene component from the scene.
			//! @param[in] component_type The type of the component to remove.
			virtual RV remove_scene_component(IName* component_type) = 0;

			//! Clears all scene components.
			virtual void clear_scene_components() = 0;

			//! Fetches one scene component by its name.
			virtual R<ISceneComponent*> get_scene_component(IName* type_name) = 0;

			//! Gets a list of all scene components.
			virtual Vector<ISceneComponent*> scene_components() = 0;
		};
	}
}