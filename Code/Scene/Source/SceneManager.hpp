// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneManager.hpp
* @author JXMaster
* @date 2020/5/7
*/
#pragma once
#include "SceneHeader.hpp"
#include <Runtime/HashMap.hpp>
namespace Luna
{
	namespace Scene
	{
		extern Unconstructed<HashMap<Name, P<IComponentType>>> g_component_types;
		extern Unconstructed<HashMap<Name, P<ISceneComponentType>>> g_scene_component_types;
	}
}