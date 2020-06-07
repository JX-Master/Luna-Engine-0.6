// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneManager.hpp
* @author JXMaster
* @date 2020/5/7
*/
#pragma once
#include "SceneHeader.hpp"
#include <Base/HashMap.hpp>
namespace luna
{
	namespace scene
	{
		extern Unconstructed<HashMap<P<IName>, P<IComponentType>>> g_component_types;
		extern Unconstructed<HashMap<P<IName>, P<ISceneComponentType>>> g_scene_component_types;
	}
}