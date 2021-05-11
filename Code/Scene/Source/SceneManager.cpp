// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneManager.cpp
* @author JXMaster
* @date 2020/5/7
*/
#include "SceneManager.hpp"
#include "Scene.hpp"
#include "Entity.hpp"
#include "SceneAssetType.hpp"
#include <Runtime/Module.hpp>

namespace Luna
{
	namespace Scene
	{
		Unconstructed<HashMap<Name, P<IComponentType>>> g_component_types;
		Unconstructed<HashMap<Name, P<ISceneComponentType>>> g_scene_component_types;
		Unconstructed<SceneAssetType> g_scene_asset_type;

		void deinit()
		{
			Name scene_type_name = g_scene_asset_type.get().m_scene_type_name;
			auto _ = Asset::unregister_asset_type(scene_type_name);
			g_scene_component_types.destruct();
			g_component_types.destruct();
		}

		RV init()
		{
			g_component_types.construct();
			g_scene_component_types.construct();
			g_scene_asset_type.construct();
			auto ptr = box_ptr(&g_scene_asset_type.get());
			auto _ = Asset::register_asset_type(ptr);
			return RV();
		}

		StaticRegisterModule m(u8"Scene", u8"Core;Asset", init, deinit);

		LUNA_SCENE_API RV register_component_type(IComponentType* component_type)
		{
			auto name = component_type->type_name();
			auto iter = g_component_types.get().find(name);
			if (iter != g_component_types.get().end())
			{
				return custom_error(BasicError::already_exists(), "scene::register_component_type - Component type %s has already been registered.", name.c_str());
			}
			g_component_types.get().insert(Pair<Name, P<IComponentType>>(name, component_type));
			return RV();
		}
		LUNA_SCENE_API RV unregister_component_type(IComponentType* component_type)
		{
			auto name = component_type->type_name();
			auto iter = g_component_types.get().find(name);
			if (iter != g_component_types.get().end())
			{
				g_component_types.get().erase(iter);
				return RV();
			}
			return custom_error(BasicError::not_found(), "scene::unregister_component_type - Component type %s is not registered to the system.", name.c_str());
		}
		LUNA_SCENE_API RV register_scene_component_type(ISceneComponentType* component_type)
		{
			auto name = component_type->type_name();
			auto iter = g_scene_component_types.get().find(name);
			if (iter != g_scene_component_types.get().end())
			{
				return custom_error(BasicError::already_exists(), "scene::register_scene_component_type - Component type %s has already been registered.", name.c_str());
			}
			g_scene_component_types.get().insert(Pair<Name, P<ISceneComponentType>>(name, component_type));
			return RV();
		}
		LUNA_SCENE_API RV unregister_scene_component_type(ISceneComponentType* component_type)
		{
			auto name = component_type->type_name();
			auto iter = g_scene_component_types.get().find(name);
			if (iter != g_scene_component_types.get().end())
			{
				g_scene_component_types.get().erase(iter);
				return RV();
			}
			return custom_error(BasicError::not_found(), "scene::unregister_component_type - Component type %s is not registered to the system.", name.c_str());
		}
		LUNA_SCENE_API Vector<IComponentType*> component_types()
		{
			Vector<IComponentType*> types;
			types.reserve(g_component_types.get().size());
			for (auto& i : g_component_types.get())
			{
				types.push_back(i.second.get());
			}
			return types;
		}
		LUNA_SCENE_API Vector<ISceneComponentType*> scene_component_types()
		{
			Vector<ISceneComponentType*> types;
			types.reserve(g_scene_component_types.get().size());
			for (auto& i : g_scene_component_types.get())
			{
				types.push_back(i.second.get());
			}
			return types;

		}
		LUNA_SCENE_API RP<IScene> new_scene()
		{
			P<IScene> s;
			lutry
			{
				luset(s, Asset::new_asset(Name("Scene")));
			}
			lucatchret;
			return s;
		}
	}
}