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

namespace luna
{
	namespace scene
	{
		Unconstructed<HashMap<P<IName>, P<IComponentType>>> g_component_types;
		Unconstructed<HashMap<P<IName>, P<ISceneComponentType>>> g_scene_component_types;
		Unconstructed<SceneAssetType> g_scene_asset_type;

		void deinit()
		{
			P<IName> scene_type_name = g_scene_asset_type.get().m_scene_type_name;
			auto _ = asset::unregister_asset_type(scene_type_name);
			g_scene_component_types.destruct();
			g_component_types.destruct();
		}

		LUNA_SCENE_API void init()
		{
			g_component_types.construct(get_module_allocator());
			g_scene_component_types.construct(get_module_allocator());
			g_scene_asset_type.construct();
			auto ptr = box_ptr(&g_scene_asset_type.get());
			auto _ = asset::register_asset_type(ptr);
			add_module("Scene", deinit);
		}
		LUNA_SCENE_API RV register_component_type(IComponentType* component_type)
		{
			auto name = component_type->type_name();
			auto iter = g_component_types.get().find(name);
			if (iter != g_component_types.get().end())
			{
				set_err(e_item_already_exists, "scene::register_component_type - Component type %s has already been registered.", name->c_str());
				return e_user_failure;
			}
			g_component_types.get().insert(Pair<P<IName>, P<IComponentType>>(name, component_type));
			return s_ok;
		}
		LUNA_SCENE_API RV unregister_component_type(IComponentType* component_type)
		{
			auto name = component_type->type_name();
			auto iter = g_component_types.get().find(name);
			if (iter != g_component_types.get().end())
			{
				g_component_types.get().erase(iter);
				return s_ok;
			}
			set_err(e_item_not_exist, "scene::unregister_component_type - Component type %s is not registered to the system.", name->c_str());
			return e_user_failure;
		}
		LUNA_SCENE_API RV register_scene_component_type(ISceneComponentType* component_type)
		{
			auto name = component_type->type_name();
			auto iter = g_scene_component_types.get().find(name);
			if (iter != g_scene_component_types.get().end())
			{
				set_err(e_item_already_exists, "scene::register_scene_component_type - Component type %s has already been registered.", name->c_str());
				return e_user_failure;
			}
			g_scene_component_types.get().insert(Pair<P<IName>, P<ISceneComponentType>>(name, component_type));
			return s_ok;
		}
		LUNA_SCENE_API RV unregister_scene_component_type(ISceneComponentType* component_type)
		{
			auto name = component_type->type_name();
			auto iter = g_scene_component_types.get().find(name);
			if (iter != g_scene_component_types.get().end())
			{
				g_scene_component_types.get().erase(iter);
				return s_ok;
			}
			set_err(e_item_not_exist, "scene::unregister_component_type - Component type %s is not registered to the system.", name->c_str());
			return e_user_failure;
		}
		LUNA_SCENE_API Vector<IComponentType*> component_types()
		{
			Vector<IComponentType*> types(get_module_allocator());
			types.reserve(g_component_types.get().size());
			for (auto& i : g_component_types.get())
			{
				types.push_back(i.second.get());
			}
			return types;
		}
		LUNA_SCENE_API Vector<ISceneComponentType*> scene_component_types()
		{
			Vector<ISceneComponentType*> types(get_module_allocator());
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
				luset(s, asset::new_asset(intern_name("Scene")));
			}
			lucatchret;
			return s;
		}
	}
}