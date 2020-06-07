// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Scene.cpp
* @author JXMaster
* @date 2020/5/7
*/
#include "Scene.hpp"
#include "SceneManager.hpp"

namespace luna
{
	namespace scene
	{
		void Scene::reset()
		{
			m_meta->load(asset::EAssetLoadFlag::force_reload | asset::EAssetLoadFlag::procedural, nullptr);
		}
		R<IEntity*> Scene::add_entity(IName* entity_name)
		{
			MutexGuard g(m_meta->mutex());
			luassert_msg_usr(m_meta->state() != asset::EAssetState::unloaded, "This call is not allowed when the scene is not loaded.");
			// Check name.
			auto iter = m_entities.find(entity_name);
			if (iter != m_entities.end())
			{
				return e_item_already_exists;
			}
			// Create Entity.
			P<Entity> e = box_ptr(new_obj<Entity>(get_module_allocator()));
			e->m_name = entity_name;
			e->m_belonging_scene = this;
			// Attach the entity to this scene.
			m_entities.insert(Pair<P<IName>, P<Entity>>(entity_name, e));
			return e.get();
		}
		RV Scene::remove_entity(IName* entity_name)
		{
			MutexGuard g(m_meta->mutex());
			luassert_msg_usr(m_meta->state() != asset::EAssetState::unloaded, "This call is not allowed when the scene is not loaded.");
			auto iter = m_entities.find(entity_name);
			if (iter != m_entities.end())
			{
				iter->second->clear_components();
				m_entities.erase(iter);
				return s_ok;
			}
			return e_item_not_exist;
		}
		R<IEntity*> Scene::find_entity(IName* name)
		{
			MutexGuard g(m_meta->mutex());
			luassert_msg_usr(m_meta->state() != asset::EAssetState::unloaded, "This call is not allowed when the scene is not loaded.");
			auto iter = m_entities.find(name);
			if (iter != m_entities.end())
			{
				return iter->second.get();
			}
			return e_item_not_exist;
		}
		Vector<IEntity*> Scene::entities()
		{
			MutexGuard g(m_meta->mutex());
			luassert_msg_usr(m_meta->state() != asset::EAssetState::unloaded, "This call is not allowed when the scene is not loaded.");
			Vector<IEntity*> ret(get_module_allocator());
			ret.reserve(m_entities.size());
			for (auto& i : m_entities)
			{
				ret.push_back(i.second.get());
			}
			return ret;
		}
		void Scene::clear_entities()
		{
			MutexGuard g(m_meta->mutex());
			luassert_msg_usr(m_meta->state() != asset::EAssetState::unloaded, "This call is not allowed when the scene is not loaded.");
			auto ents = entities();
			for (auto& i : ents)
			{
				auto _ = remove_entity(i->name());
			}
		}
		R<ISceneComponent*> Scene::add_scene_component(IName* component_type)
		{
			MutexGuard g(m_meta->mutex());
			luassert_msg_usr(m_meta->state() != asset::EAssetState::unloaded, "This call is not allowed when the scene is not loaded.");
			// Check name.
			for (auto& i : m_scene_components)
			{
				if (i->type_object()->type_name() == component_type)
				{
					return e_item_already_exists;
				}
			}
			
			// Create new scene component.
			auto iter = g_scene_component_types.get().find(component_type);
			if (iter == g_scene_component_types.get().end())
			{
				return e_type_dismatch;
			}
			auto component = iter->second->new_component(this);

			if (failed(component))
			{
				return component.result();
			}

			// Attach the component to this scene.
			m_scene_components.push_back(component.get());

			// Add asset registry. This is rarely used.
			auto assets = component.get()->referred_assets();
			for (auto& i : assets)
			{
				meta()->internal_add_dependency(i);
			}
			return component.get();
		}
		RV Scene::remove_scene_component(IName* component_type)
		{
			MutexGuard g(m_meta->mutex());
			luassert_msg_usr(m_meta->state() != asset::EAssetState::unloaded, "This call is not allowed when the scene is not loaded.");
			for (auto iter = m_scene_components.begin(); iter != m_scene_components.end(); ++iter)
			{
				if (component_type == iter->get()->type_object()->type_name())
				{
					// Remove asset registry.
					auto assets = iter->get()->referred_assets();
					for (auto& i : assets)
					{
						auto _ = meta()->internal_remove_dependency(i);
					}
					m_scene_components.erase(iter);
					return s_ok;
				}
			}
			return e_item_not_exist;
		}
		void Scene::clear_scene_components()
		{
			MutexGuard g(m_meta->mutex());
			luassert_msg_usr(m_meta->state() != asset::EAssetState::unloaded, "This call is not allowed when the scene is not loaded.");
			auto components = m_scene_components;
			for (auto& i : components)
			{
				auto _ = remove_scene_component(i->type_object()->type_name());
			}
		}
		R<ISceneComponent*> Scene::get_scene_component(IName* type_name)
		{
			MutexGuard g(m_meta->mutex());
			luassert_msg_usr(m_meta->state() != asset::EAssetState::unloaded, "This call is not allowed when the scene is not loaded.");
			for (auto& i : m_scene_components)
			{
				if (i->type_object()->type_name() == type_name)
				{
					return i.get();
				}
			}
			return e_item_not_exist;
		}
		Vector<ISceneComponent*> Scene::scene_components()
		{
			MutexGuard g(m_meta->mutex());
			luassert_msg_usr(m_meta->state() != asset::EAssetState::unloaded, "This call is not allowed when the scene is not loaded.");

			Vector<ISceneComponent*> ret(get_module_allocator());
			ret.reserve(m_scene_components.size());
			for (auto& i : m_scene_components)
			{
				ret.push_back(i.get());
			}
			return ret;
		}
	}
}