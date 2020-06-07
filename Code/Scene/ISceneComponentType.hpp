// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ISceneComponentType.hpp
* @author JXMaster
* @date 2020/5/4
*/
#pragma once
#include "ISceneComponent.hpp"

namespace luna
{
	namespace scene
	{
		struct IScene;

		//! @interface ISceneComponentType
		struct ISceneComponentType : public IObject
		{
			luiid("{4c178ba0-99ab-438c-bc38-3eb1abd300df}");

			//! Gets the component type name of this component type.
			virtual IName* type_name() = 0;

			//! Creates a new component with default-initialized data.
			virtual RP<ISceneComponent> new_component(IScene* belonging_scene) = 0;

			//! Called when the data of one of the dependency assets of this component is loaded.
			virtual void on_dependency_data_load(ISceneComponent* component, asset::IAsset* dependency_asset) = 0;

			//! Called when the data of one of the dependency assets is about to be unloaded.
			virtual void on_dependency_data_unload(ISceneComponent* component, asset::IAsset* dependency_asset) = 0;

			//! Called when the data of one of the dependency assets is going to be replaced.
			virtual void on_dependency_replace(ISceneComponent* component, const Guid& before, const Guid& after) = 0;
		};
	}
}