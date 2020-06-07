// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Entity.cpp
* @author JXMaster
* @date 2020/5/7
*/
#include "Entity.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
namespace luna
{
	namespace scene
	{
		RP<IVariant> Entity::serialize()
		{
			lutsassert();
			P<IVariant> data = new_var(EVariantType::table);
			lutry
			{
				auto components_node = new_var(EVariantType::table);
				for (auto& i : m_components)
				{
					lulet(component_node, i->serialize());
					components_node->set_field(0, i->type_object()->type_name(), component_node);
				}
				data->set_field(0, intern_name("components"), components_node);
			}
			lucatchret;
			return data;
		}

		RV Entity::pre_deserialize(IVariant* obj)
		{
			lutsassert();
			lutry
			{
				clear_components();
				lulet(components_node, obj->field(0, intern_name("components")));
				lulet(components, components_node->fields(0));
				for (auto& i : components)
				{
					luexp(add_component(i));
				}
			}
			lucatchret;
			return s_ok;
		}

		RV Entity::deserialize(IVariant* obj)
		{
			lutsassert();
			lutry
			{
				lulet(components_node, obj->field(0, intern_name("components")));
				lulet(components, components_node->fields(0));
				for (auto& i : components)
				{
					lulet(comp, get_component(i));
					lulet(comp_data, components_node->field(0, i));
					luexp(comp->deserialize(comp_data));
				}
			}
			lucatchret;
			return s_ok;
		}

		P<IScene> Entity::belonging_scene()
		{
			return m_belonging_scene.lock();
		}

		IName* Entity::name()
		{
			return m_name;
		}

		RV Entity::set_name(IName* name)
		{
			lutsassert();
			luassert_usr(name);
			P<Scene> s = m_belonging_scene.lock();
			if (s)
			{
#ifdef LUNA_PROFILE
				TSGuard guard(s->m_tsassert_lock);
#endif
				auto iter = s->m_entities.find(name);
				if (iter != s->m_entities.end())
				{
					return e_item_already_exists;
				}
				s->m_entities.insert(Pair<P<IName>, P<Entity>>(name, this));
				if (m_name)
				{
					s->m_entities.erase(m_name);
				}
			}
			m_name = name;
			return s_ok;
		}

		R<IComponent*> Entity::add_component(IName* component_type)
		{
			lutsassert();
			// Check name.
			for (auto& i : m_components)
			{
				if (i->type_object()->type_name() == component_type)
				{
					return e_item_already_exists;
				}
			}
			// Create component.
			auto iter = g_component_types.get().find(component_type);
			if (iter == g_component_types.get().end())
			{
				return e_type_dismatch;
			}
			auto component = iter->second->new_component(this);
			if (failed(component))
			{
				return component.result();
			}

			// Attach the component to this entity.
			m_components.push_back(component.get());

			// Add asset registry. This is rarely used.
			auto s = m_belonging_scene.lock();
			auto assets = component.get()->referred_assets();
			for (auto& i : assets)
			{
				s->meta()->internal_add_dependency(i);
			}
			return component.get();
		}

		RV Entity::remove_component(IName* component_type)
		{
			lutsassert();
			for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
			{
				if (component_type == iter->get()->type_object()->type_name())
				{
					// Remove asset registry.
					auto s = m_belonging_scene.lock();
					auto assets = iter->get()->referred_assets();
					for (auto& i : assets)
					{
						auto _ = s->meta()->internal_remove_dependency(i);
					}
					m_components.erase(iter);
					return s_ok;
				}
			}
			return e_item_not_exist;
		}

		void Entity::clear_components()
		{
			lutsassert();
			auto components = m_components;
			for (auto& i : components)
			{
				auto _ = remove_component(i->type_object()->type_name());
			}
		}

		R<IComponent*> Entity::get_component(IName* type_name)
		{
			lutsassert();
			for (auto& i : m_components)
			{
				if (i->type_object()->type_name() == type_name)
				{
					return i.get();
				}
			}
			return e_item_not_exist;
		}

		Vector<IComponent*> Entity::components()
		{
			lutsassert();
			Vector<IComponent*> ret(get_module_allocator());
			ret.reserve(m_components.size());
			for (auto& i : m_components)
			{
				ret.push_back(i.get());
			}
			return ret;
		}
	}
}

