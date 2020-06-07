// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IComponentEditorType.hpp
* @author JXMaster
* @date 2020/5/15
*/
#pragma once
#include "IComponentEditor.hpp"

namespace luna
{
	namespace editor
	{
		struct IComponentEditorType : public IObject
		{
			luiid("{b3aef80d-c4ed-4799-b059-f73472fc93d8}");

			//! Gets the type of the component this editor plugin manages.
			virtual IName* type() = 0;

			//! Creates a new instance of the editor for the specified component.
			//! The editor should keep a weak reference to the component.
			virtual P<IComponentEditor> new_editor(scene::IComponent* component) = 0;
		};
	}
}