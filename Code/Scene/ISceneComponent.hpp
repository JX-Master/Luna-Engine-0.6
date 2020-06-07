// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ISceneComponent.hpp
* @author JXMaster
* @date 2020/5/4
*/
#pragma once
#include <Core/Core.hpp>

namespace luna
{
	namespace scene
	{
		struct ISceneComponentType;
		//! @interface ISceneComponent
		struct ISceneComponent : public ISerializable
		{
			luiid("{b8b90644-f78d-41e7-b85f-9d6653c011ed}");

			//! Gets the type object of this component.
			virtual ISceneComponentType* type_object() = 0;

			//! Gets the belonging scene.
			//! The component should keep a weak reference to the scene.
			virtual P<IScene> belonging_scene() = 0;

			//! Gets a list of assets this component refers to.
			virtual Vector<Guid> referred_assets() = 0;
		};
	}
}