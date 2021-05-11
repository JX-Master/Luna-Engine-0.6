// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Scene.hpp
* @author JXMaster
* @date 2020/5/7
*/
#pragma once
#include "SceneHeader.hpp"
#include "Entity.hpp"
#include <Runtime/HashMap.hpp>
#include <Runtime/TSAssert.hpp>

namespace Luna
{
	namespace Scene
	{
		class Scene : public IScene
		{
		public:
			lucid("{7402c29e-780b-4bb8-8de4-ee83a006a3e8}");
			luiimpl(Scene, IScene, Asset::IAsset, IObject);
			lutsassert_lock();

			HashMap<Name, P<Entity>> m_entities;
			Vector<P<ISceneComponent>> m_scene_components;

			P<Asset::IAssetMeta> m_meta;

			Scene() {}

			virtual Asset::IAssetMeta* meta() override
			{
				return m_meta.get();
			}
			virtual void reset() override;
			virtual R<IEntity*> add_entity(const Name& entity_name) override;
			virtual RV remove_entity(const Name& entity_name) override;
			virtual R<IEntity*> find_entity(const Name& name) override;
			virtual Vector<IEntity*> entities() override;
			virtual void clear_entities() override;
			virtual R<ISceneComponent*> add_scene_component(const Name& component_type) override;
			virtual RV remove_scene_component(const Name& component_type) override;
			virtual void clear_scene_components() override;
			virtual R<ISceneComponent*> get_scene_component(const Name& type_name) override;
			virtual Vector<ISceneComponent*> scene_components() override;
		};
	}
}