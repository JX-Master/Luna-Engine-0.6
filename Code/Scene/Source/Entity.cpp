// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Entity.cpp
* @author JXMaster
* @date 2020/5/7
*/
#include "Entity.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
namespace Luna
{
	namespace Scene
	{
		R<Variant> Entity::serialize()
		{
			lutsassert();
			Variant data = Variant(EVariantType::table);
			lutry
			{
				auto components_node = Variant(EVariantType::table);
				for (auto& i : m_components)
				{
					lulet(component_node, i->serialize());
					components_node.set_field(0, i->type_object()->type_name(), component_node);
				}
				data.set_field(0, Name(u8"components"), components_node);
			}
			lucatchret;
			return data;
		}

		RV Entity::pre_deserialize(const Variant& obj)
		{
			lutsassert();
			lutry
			{
				clear_components();
				auto& components_node = obj.field(0, u8"components");
				auto components = components_node.fields(0);
				for (auto& i : components)
				{
					luexp(add_component(i));
				}
			}
			lucatchret;
			return RV();
		}

		RV Entity::deserialize(const Variant& obj)
		{
			lutsassert();
			lutry
			{
				auto& components_node = obj.field(0, u8"components");
				auto components = components_node.fields(0);
				for (auto& i : components)
				{
					lulet(comp, get_component(i));
					auto& comp_data = components_node.field(0, i);
					luexp(comp->deserialize(comp_data));
				}
			}
			lucatchret;
			return RV();
		}

		P<IScene> Entity::belonging_scene()
		{
			return m_belonging_scene.lock();
		}

		Name Entity::name()
		{
			return m_name;
		}

		RV Entity::set_name(const Name& name)
		{
			lutsassert();
			lucheck(name);
			P<Scene> s = m_belonging_scene.lock();
			if (s)
			{
#ifdef LUNA_PROFILE
				TSGuard guard(s->m_tsassert_lock);
#endif
				auto iter = s->m_entities.find(name);
				if (iter != s->m_entities.end())
				{
					return BasicError::already_exists();
				}
				s->m_entities.insert(Pair<Name, P<Entity>>(name, this));
				if (m_name)
				{
					s->m_entities.erase(m_name);
				}
			}
			m_name = name;
			return RV();
		}

		R<IComponent*> Entity::add_component(const Name& component_type)
		{
			lutsassert();
			// Check name.
			for (auto& i : m_components)
			{
				if (i->type_object()->type_name() == component_type)
				{
					return BasicError::already_exists();
				}
			}
			// Create component.
			auto iter = g_component_types.get().find(component_type);
			if (iter == g_component_types.get().end())
			{
				return BasicError::not_found();
			}
			auto component = iter->second->new_component(this);
			if (failed(component))
			{
				return component.errcode();
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

		RV Entity::remove_component(const Name& component_type)
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
					return RV();
				}
			}
			return BasicError::not_found();
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

		R<IComponent*> Entity::get_component(const Name& type_name)
		{
			lutsassert();
			for (auto& i : m_components)
			{
				if (i->type_object()->type_name() == type_name)
				{
					return i.get();
				}
			}
			return BasicError::not_found();
		}

		Vector<IComponent*> Entity::components()
		{
			lutsassert();
			Vector<IComponent*> ret;
			ret.reserve(m_components.size());
			for (auto& i : m_components)
			{
				ret.push_back(i.get());
			}
			return ret;
		}
	}
}

