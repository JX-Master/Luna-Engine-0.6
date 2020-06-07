// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IComponent.hpp
* @author JXMaster
* @date 2020/4/13
*/
#pragma once
#include <Core/Core.hpp>
#include <Asset/Asset.hpp>

namespace luna
{
	namespace scene
	{
		struct IEntity;
		struct IComponentType;

		//! @interface IComponent
		struct IComponent : public ISerializable
		{
			luiid("{d1afbbe2-63be-45ab-a0a4-741a26c59d0a}");

			//! Gets the type object of this component.
			virtual IComponentType* type_object() = 0;

			//! Gets the belonging entity.
			//! The component should keep a weak reference to the entity.
			virtual P<IEntity> belonging_entity() = 0;

			//! Gets a list of assets this component refers to.
			virtual Vector<Guid> referred_assets() = 0;
		};
	}
}