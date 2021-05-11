// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneRenderer.hpp
* @author JXMaster
* @date 2020/5/21
*/
#pragma once
#include "3DEngineHeader.hpp"
#include <Runtime/TSAssert.hpp>
namespace Luna
{
	namespace E3D
	{
		class SceneRenderer : public ISceneRenderer
		{
		public:
			lucid("{3ccba39c-7c6b-4e8f-a563-299837c4c227}");
			luiimpl(SceneRenderer, ISceneRenderer, Scene::ISceneComponent, ISerializable, IObject);
			lutsassert_lock();

			WP<Scene::IScene> m_belonging_scene;
			WP<Scene::IEntity> m_camera_entity;

			P<Gfx::IResource> m_depth_buffer;	// D32_FLOAT

			P<Gfx::IResource> m_screen_buffer;	// RGBA8_UNORM

			P<Gfx::IResource> m_lighting_buffer; // RGB32_FLOAT

			P<Gfx::IResource> m_lighting_accms[11];		// R32_FLOAT from 1024.

			Asset::PAsset<Texture::ITexture> m_skybox;

			Float3 m_environment_color = Float3(0.0f, 0.0f, 0.0f);

			f32 m_skybox_rotation = 0.0f;
			f32 m_exposure = 1.0f;
				
			SceneRenderer() {}

			virtual R<Variant> serialize() override;
			virtual RV deserialize(const Variant& obj) override;
			virtual Scene::ISceneComponentType* type_object() override;
			virtual P<Scene::IScene> belonging_scene() override
			{
				return m_belonging_scene.lock();
			}
			virtual Vector<Guid> referred_assets() override
			{
				return Vector<Guid>();
			}
			virtual P<Scene::IEntity> camera_entity() override
			{
				lutsassert();
				return m_camera_entity.lock();
			}
			virtual void set_camera_entity(Scene::IEntity* entity) override
			{
				lutsassert();
				m_camera_entity = entity;
			}
			virtual Float3 environment_color() override
			{
				lutsassert();
				return m_environment_color;
			}
			virtual void set_environment_color(const Float3& value) override
			{
				lutsassert();
				m_environment_color = value;
			}
			virtual Gfx::IResource* screen_buffer() override
			{
				lutsassert();
				return m_screen_buffer;
			}
			virtual Gfx::IResource* depth_buffer() override
			{
				lutsassert();
				return m_depth_buffer;
			}
			virtual Gfx::IResource* lighting_buffer() override
			{
				lutsassert();
				return m_lighting_buffer;
			}
			virtual Gfx::IResource* tone_mapping_buffer(u32 index) override
			{
				lutsassert();
				lucheck(index < 11);
				return m_lighting_accms[index];
			}

			virtual f32 exposure() override
			{
				return m_exposure;
			}
			virtual void set_exposure(f32 value) override
			{
				m_exposure = value;
			}

			virtual Asset::PAsset<Texture::ITexture> skybox_texture() override
			{
				return m_skybox;
			}
			virtual void set_skybox_texture(Asset::PAsset<Texture::ITexture> texture) override
			{
				m_skybox = texture;
			}
			virtual f32 skybox_rotation() override
			{
				return m_skybox_rotation;
			}
			virtual void set_skybox_rotation(f32 value) override
			{
				m_skybox_rotation = value;
			}

			virtual RV resize_screen_buffer(const UInt2U& size) override;
		};


		class SceneRendererType : public Scene::ISceneComponentType
		{
		public:
			lucid("{2ad6b522-d209-4ea6-8828-62349e637a03}");
			luiimpl_static(SceneRendererType, Scene::ISceneComponentType, IObject);

			Name m_type_name;

			SceneRendererType() :
				m_type_name(Name("Scene Renderer")) {}

			virtual Name type_name() override
			{
				return m_type_name;
			}
			virtual RP<Scene::ISceneComponent> new_component(Scene::IScene* belonging_scene) override;
			virtual void on_dependency_data_load(Scene::ISceneComponent* component, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(Scene::ISceneComponent* component, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(Scene::ISceneComponent* component, const Guid& before, const Guid& after) override {}
		};

		extern Unconstructed<SceneRendererType> g_scene_renderer_type;
	}
}