// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneRenderer.hpp
* @author JXMaster
* @date 2020/5/21
*/
#pragma once
#include "3DEngineHeader.hpp"
#include <Base/TSAssert.hpp>
namespace luna
{
	namespace e3d
	{
		class SceneRenderer : public ISceneRenderer
		{
		public:
			lucid("{3ccba39c-7c6b-4e8f-a563-299837c4c227}");
			luiimpl(SceneRenderer, ISceneRenderer, scene::ISceneComponent, ISerializable, IObject);
			lutsassert_lock();

			WP<scene::IScene> m_belonging_scene;
			WP<scene::IEntity> m_camera_entity;

			P<gfx::IResource> m_depth_buffer;	// D32_FLOAT

			P<gfx::IResource> m_screen_buffer;	// RGBA8_UNORM

			P<gfx::IResource> m_lighting_buffer; // RGB32_FLOAT

			P<gfx::IResource> m_lighting_accms[11];		// R32_FLOAT from 1024.

			asset::PAsset<texture::ITexture> m_skybox;

			Float3 m_environment_color = Float3(0.0f, 0.0f, 0.0f);

			float32 m_skybox_rotation = 0.0f;
			float32 m_exposure = 1.0f;
				
			SceneRenderer(IAllocator* alloc) :
				luibind(alloc) {}

			virtual RP<IVariant> serialize() override;
			virtual RV deserialize(IVariant* obj) override;
			virtual scene::ISceneComponentType* type_object() override;
			virtual P<scene::IScene> belonging_scene() override
			{
				return m_belonging_scene.lock();
			}
			virtual Vector<Guid> referred_assets() override
			{
				return Vector<Guid>();
			}
			virtual P<scene::IEntity> camera_entity() override
			{
				lutsassert();
				return m_camera_entity.lock();
			}
			virtual void set_camera_entity(scene::IEntity* entity) override
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
			virtual gfx::IResource* screen_buffer() override
			{
				lutsassert();
				return m_screen_buffer;
			}
			virtual gfx::IResource* depth_buffer() override
			{
				lutsassert();
				return m_depth_buffer;
			}
			virtual gfx::IResource* lighting_buffer() override
			{
				lutsassert();
				return m_lighting_buffer;
			}
			virtual gfx::IResource* tone_mapping_buffer(uint32 index) override
			{
				lutsassert();
				luassert_usr(index < 11);
				return m_lighting_accms[index];
			}

			virtual float32 exposure() override
			{
				return m_exposure;
			}
			virtual void set_exposure(float32 value) override
			{
				m_exposure = value;
			}

			virtual asset::PAsset<texture::ITexture> skybox_texture() override
			{
				return m_skybox;
			}
			virtual void set_skybox_texture(asset::PAsset<texture::ITexture> texture) override
			{
				m_skybox = texture;
			}
			virtual float32 skybox_rotation() override
			{
				return m_skybox_rotation;
			}
			virtual void set_skybox_rotation(float32 value) override
			{
				m_skybox_rotation = value;
			}

			virtual RV resize_screen_buffer(const UInt2U& size) override;
		};


		class SceneRendererType : public scene::ISceneComponentType
		{
		public:
			lucid("{2ad6b522-d209-4ea6-8828-62349e637a03}");
			luiimpl_static(SceneRendererType, scene::ISceneComponentType, IObject);

			P<IName> m_type_name;

			SceneRendererType() :
				m_type_name(intern_name("Scene Renderer")) {}

			virtual IName* type_name() override
			{
				return m_type_name;
			}
			virtual RP<scene::ISceneComponent> new_component(scene::IScene* belonging_scene) override;
			virtual void on_dependency_data_load(scene::ISceneComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(scene::ISceneComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(scene::ISceneComponent* component, const Guid& before, const Guid& after) override {}
		};

		extern Unconstructed<SceneRendererType> g_scene_renderer_type;
	}
}