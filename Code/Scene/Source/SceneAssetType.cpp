// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneAssetType.cpp
* @author JXMaster
* @date 2020/5/7
*/
#include "SceneAssetType.hpp"

namespace luna
{
	namespace scene
	{
		using namespace asset;
		P<asset::IAsset> SceneAssetType::on_new_asset(asset::IAssetMeta* meta)
		{
			auto s = box_ptr(new_obj<Scene>(get_module_allocator()));
			s->m_meta = meta;
			return s;
		}
		RV SceneAssetType::on_load_data(asset::IAsset* target_asset, IVariant* data, IVariant* params)
		{
			P<Scene> s = target_asset;
			MutexGuard g(s->meta()->mutex());
			lutry
			{
				s->clear_entities();
				s->clear_scene_components();
				lulet(entities_field, data->field(0, intern_name("entities")));
				lulet(components_field, data->field(0, intern_name("components")));
				lulet(entities, entities_field->fields(0));
				lulet(components, components_field->fields(0));

				// First creates all entities and components.
				for (size_t i = 0; i < entities.size(); ++i)
				{
					lulet(entity, s->add_entity(entities[i]));
					lulet(entity_data, entities_field->field(0, entities[i]));
					luexp(static_cast<Entity*>(entity)->pre_deserialize(entity_data));	// Create all components with default data.
				}
				for (size_t i = 0; i < components.size(); ++i)
				{
					lulet(component, s->add_scene_component(components[i]));
				}
				// Then deserialize their data. So that all weak references are prepared.
				for (size_t i = 0; i < entities.size(); ++i)
				{
					lulet(entity, s->find_entity(entities[i]));
					lulet(entity_data, entities_field->field(0, entities[i]));
					luexp(static_cast<Entity*>(entity)->deserialize(entity_data));
				}
				for (size_t i = 0; i < components.size(); ++i)
				{
					lulet(component, s->get_scene_component(components[i]));
					lulet(component_data, components_field->field(0, components[i]));
					luexp(component->deserialize(component_data));
				}
			}
			lucatchret;
			return s_ok;
		}
		RV SceneAssetType::on_load_procedural_data(asset::IAsset* target_asset, IVariant* params)
		{
			P<Scene> s = target_asset;
			MutexGuard g(s->meta()->mutex());
			s->clear_entities();
			s->clear_scene_components();
			return s_ok;
		}
		void SceneAssetType::on_unload_data(asset::IAsset* target_asset)
		{
			P<Scene> s = target_asset;
			MutexGuard g(s->meta()->mutex());
			s->clear_entities();
			s->clear_scene_components();
		}
		RP<IVariant> SceneAssetType::on_save_data(asset::IAsset* target_asset, IVariant* params)
		{
			P<Scene> s = target_asset;
			MutexGuard g(s->meta()->mutex());

			auto var = new_var(EVariantType::table);
			auto entities_field = new_var(EVariantType::table);
			auto components_field = new_var(EVariantType::table);

			lutry
			{
				var->set_field(0, intern_name("entities"), entities_field);
				var->set_field(0, intern_name("components"), components_field);	// For scene components.

				for (auto& i : s->m_entities)
				{
					lulet(entity, i.second->serialize());
					entities_field->set_field(0, i.second->name(), entity);
				}

				for (auto& i : s->m_scene_components)
				{
					lulet(component, i->serialize());
					components_field->set_field(0, i->type_object()->type_name(), component);
				}
			}
			lucatchret;

			return var;
		}
		void SceneAssetType::on_dependency_data_load(asset::IAsset* current_asset, asset::IAsset* dependency_asset)
		{
			P<Scene> s = current_asset;
			MutexGuard g(s->meta()->mutex());
			for (auto& i : s->m_entities)
			{
				for (auto& j : i.second->m_components)
				{
					auto assets = j->referred_assets();
					for (auto& k : assets)
					{
						if (k == dependency_asset->meta()->guid())
						{
							j->type_object()->on_dependency_data_load(j, dependency_asset);
							break;
						}
					}
				}
			}
			for (auto& i : s->m_scene_components)
			{
				auto assets = i->referred_assets();
				for (auto& k : assets)
				{
					if (k == dependency_asset->meta()->guid())
					{
						i->type_object()->on_dependency_data_load(i, dependency_asset);
						break;
					}
				}
			}
		}
		void SceneAssetType::on_dependency_data_unload(asset::IAsset* current_asset, asset::IAsset* dependency_asset)
		{
			P<Scene> s = current_asset;
			MutexGuard g(s->meta()->mutex());
			for (auto& i : s->m_entities)
			{
				for (auto& j : i.second->m_components)
				{
					auto assets = j->referred_assets();
					for (auto& k : assets)
					{
						if (k == dependency_asset->meta()->guid())
						{
							j->type_object()->on_dependency_data_unload(j, dependency_asset);
							break;
						}
					}
				}
			}
			for (auto& i : s->m_scene_components)
			{
				auto assets = i->referred_assets();
				for (auto& k : assets)
				{
					if (k == dependency_asset->meta()->guid())
					{
						i->type_object()->on_dependency_data_unload(i, dependency_asset);
						break;
					}
				}
			}
		}
		void SceneAssetType::on_dependency_replace(IAsset* current_asset, const Guid& before, const Guid& after)
		{
			P<Scene> s = current_asset;
			MutexGuard g(s->meta()->mutex());
			for (auto& i : s->m_entities)
			{
				for (auto& j : i.second->m_components)
				{
					auto assets = j->referred_assets();
					for (auto& k : assets)
					{
						if (k == before)
						{
							j->type_object()->on_dependency_replace(j, before, after);
							break;
						}
					}
				}
			}
			for (auto& i : s->m_scene_components)
			{
				auto assets = i->referred_assets();
				for (auto& k : assets)
				{
					if (k == before)
					{
						i->type_object()->on_dependency_replace(i, before, after);
						break;
					}
				}
			}
		}
	}
}