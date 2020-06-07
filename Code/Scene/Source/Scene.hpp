// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Scene.hpp
* @author JXMaster
* @date 2020/5/7
*/
#pragma once
#include "SceneHeader.hpp"
#include "Entity.hpp"
#include <Base/HashMap.hpp>
#include <Base/TSAssert.hpp>

namespace luna
{
	namespace scene
	{
		class Scene : public IScene
		{
		public:
			lucid("{7402c29e-780b-4bb8-8de4-ee83a006a3e8}");
			luiimpl(Scene, IScene, asset::IAsset, IObject);
			lutsassert_lock();

			HashMap<P<IName>, P<Entity>> m_entities;
			Vector<P<ISceneComponent>> m_scene_components;

			P<asset::IAssetMeta> m_meta;

			Scene(IAllocator* alloc) :
				luibind(alloc),
				m_entities(alloc),
				m_scene_components(alloc) {}

			virtual asset::IAssetMeta* meta() override
			{
				return m_meta.get();
			}
			virtual void reset() override;
			virtual R<IEntity*> add_entity(IName* entity_name) override;
			virtual RV remove_entity(IName* entity_name) override;
			virtual R<IEntity*> find_entity(IName* name) override;
			virtual Vector<IEntity*> entities() override;
			virtual void clear_entities() override;
			virtual R<ISceneComponent*> add_scene_component(IName* component_type) override;
			virtual RV remove_scene_component(IName* component_type) override;
			virtual void clear_scene_components() override;
			virtual R<ISceneComponent*> get_scene_component(IName* type_name) override;
			virtual Vector<ISceneComponent*> scene_components() override;
		};
	}
}