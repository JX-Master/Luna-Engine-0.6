// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ISceneRenderer.hpp
* @author JXMaster
* @date 2020/5/21
*/
#pragma once
#include <Scene/Scene.hpp>
#include <Gfx/Gfx.hpp>
#include <Texture/Texture.hpp>
namespace Luna
{
	namespace E3D
	{
		struct ISceneRenderer : public Scene::ISceneComponent
		{
			luiid("{c9d3db8b-cf58-4900-b6db-87b618bf3fa8}");

			//! Gets the entity that has the active camera.
			virtual P<Scene::IEntity> camera_entity() = 0;

			//! Sets the entity that has the active camera.
			virtual void set_camera_entity(Scene::IEntity* entity) = 0;

			//! Gets the environment color.
			virtual Float3 environment_color() = 0;

			//! Sets the environment color.
			virtual void set_environment_color(const Float3& value) = 0;

			//! Gets the screen back buffer. (RGBA8_UNORM)
			virtual Gfx::IResource* screen_buffer() = 0;

			//! Gets the screen depth buffer. (D32_FLOAT)
			virtual Gfx::IResource* depth_buffer() = 0;

			//! Gets the lighting buffer. (RGBA32_FLOAT)
			virtual Gfx::IResource* lighting_buffer() = 0;

			//! Gets the accumulation buffer for tone mapping. (R32_FLOAT)
			//! index = [0, 11).
			virtual Gfx::IResource* tone_mapping_buffer(u32 index) = 0;

			//! Gets the exposure factor.
			virtual f32 exposure() = 0;

			//! Sets the exposure factor.
			virtual void set_exposure(f32 value) = 0;

			//! Gets the sky box texture.
			virtual Asset::PAsset<Texture::ITexture> skybox_texture() = 0;

			//! Sets the sky box texture.
			virtual void set_skybox_texture(Asset::PAsset<Texture::ITexture> texture) = 0;

			//! Gets the sky box rotation factor.
			virtual f32 skybox_rotation() = 0;

			//! Sets the sky box rotation factor.
			virtual void set_skybox_rotation(f32 value) = 0;

			//! Resizes the screen back buffer.
			virtual RV resize_screen_buffer(const UInt2U& size) = 0;
		};
	}
}