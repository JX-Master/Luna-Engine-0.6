// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneRenderer.cpp
* @author JXMaster
* @date 2020/5/21
*/
#include "SceneRenderer.hpp"
#include <Renderer/Renderer.hpp>

namespace Luna
{
	namespace E3D
	{
		Unconstructed<SceneRendererType> g_scene_renderer_type;

		R<Variant> SceneRenderer::serialize()
		{
			lutsassert();
			auto var = Variant(EVariantType::table);
			auto camera_entity = m_camera_entity.lock();
			if (camera_entity)
			{
				auto camera_entity_field = Variant(EVariantType::name);
				camera_entity_field.to_name() = camera_entity->name();
				var.set_field(0, Name("camera_entity"), camera_entity_field);

				auto exposure = Variant(EVariantType::f32);
				exposure.to_f32() = m_exposure;
				var.set_field(0, Name("exposure"), exposure);

				auto environment_color = Variant(EVariantType::f32, 3);
				environment_color.to_f32_buf()[0] = m_environment_color.x;
				environment_color.to_f32_buf()[1] = m_environment_color.y;
				environment_color.to_f32_buf()[2] = m_environment_color.z;
				var.set_field(0, Name("environment_color"), environment_color);

				if (m_skybox.guid() != Guid(0, 0))
				{
					auto skybox = Variant(EVariantType::u64, 2);
					skybox.to_u64_buf()[0] = m_skybox.guid().high;
					skybox.to_u64_buf()[1] = m_skybox.guid().low;
					var.set_field(0, Name("skybox_texture"), skybox);
				}

				auto skybox_rotation = Variant(EVariantType::f32);
				skybox_rotation.to_f32() = m_skybox_rotation;
				var.set_field(0, Name("skybox_rotation"), skybox_rotation);
			}
			return var;
		}

		RV SceneRenderer::deserialize(const Variant& obj)
		{
			lutsassert();
			lutry
			{
				auto& camera_entity = obj.field(0, "camera_entity");
				lulet(camera_entity_name, camera_entity.check_name());
				auto entity = m_belonging_scene.lock()->find_entity(camera_entity_name.get());
				if (succeeded(entity))
				{
					m_camera_entity = entity.get();
				}
				auto& expo = obj.field(0, "exposure");
				if (expo.type() != EVariantType::null)
				{
					lulet(expo_value, expo.check_f32());
					m_exposure = expo_value;
				}
				else
				{
					m_exposure = 1.0f;
				}
				auto& environment_color = obj.field(0, "environment_color");
				if (environment_color.type() != EVariantType::null)
				{
					lulet(environment_color_value, environment_color.check_f32_buf());
					m_environment_color.x = environment_color_value[0];
					m_environment_color.y = environment_color_value[1];
					m_environment_color.z = environment_color_value[2];
				}
				else
				{
					m_environment_color = Float3(0.0f);
				}
				auto& skybox_rotation = obj.field(0, "skybox_rotation");
				if (skybox_rotation.type() != EVariantType::null)
				{
					lulet(rot, skybox_rotation.check_f32());
					m_skybox_rotation = rot;
				}
				else
				{
					m_skybox_rotation = 0.0f;
				}
				auto& skybox_tex = obj.field(0, "skybox_texture");
				if (skybox_tex.type() != EVariantType::null)
				{
					lulet(tex, skybox_tex.check_u64_buf());
					m_skybox = Guid(tex[0], tex[1]);
				}
				else
				{
					m_skybox = Guid(0, 0);
				}
			}
			lucatchret;
			return RV();
		}

		Scene::ISceneComponentType* SceneRenderer::type_object()
		{
			return &g_scene_renderer_type.get();
		}

		RV SceneRenderer::resize_screen_buffer(const UInt2U& size)
		{
			lutsassert();
			lutry
			{
				using namespace Gfx;

				auto desc = m_screen_buffer->desc();
				desc.width = size.x;
				desc.height = size.y;
				f32 clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				luset(m_screen_buffer, Renderer::device()->new_resource(desc, &ClearValue::as_color(EResourceFormat::rgba8_unorm, clear_color)));

				desc = m_depth_buffer->desc();
				desc.width = size.x;
				desc.height = size.y;
				luset(m_depth_buffer, Renderer::device()->new_resource(desc, &ClearValue::as_depth_stencil(EResourceFormat::d32_float, 1.0f, 0xFF)));

				desc = m_lighting_buffer->desc();
				desc.width = size.x;
				desc.height = size.y;
				luset(m_lighting_buffer, Renderer::device()->new_resource(desc, &ClearValue::as_color(EResourceFormat::rgba32_float, clear_color)));
			}
			lucatchret;
			return RV();
		}

		RP<Scene::ISceneComponent> SceneRendererType::new_component(Scene::IScene* belonging_scene)
		{
			auto r = newobj<SceneRenderer>();
			r->m_belonging_scene = belonging_scene;
			f32 clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			using namespace Gfx;

			lutry
			{
				luset(r->m_screen_buffer, Renderer::device()->new_resource(ResourceDesc::tex2d(
					EResourceFormat::rgba8_unorm, 
					EAccessType::gpu_local,
					EResourceUsageFlag::render_target | EResourceUsageFlag::unordered_access, 1024, 768, 1, 1), 
					&ClearValue::as_color(EResourceFormat::rgba8_unorm, clear_color)));

				luset(r->m_depth_buffer, Renderer::device()->new_resource(ResourceDesc::tex2d(
					EResourceFormat::d32_float,
					EAccessType::gpu_local,
					EResourceUsageFlag::depth_stencil,
					1024, 768, 1, 1), &ClearValue::as_depth_stencil(EResourceFormat::d32_float, 1.0f, 0xFF)));

				luset(r->m_lighting_buffer, Renderer::device()->new_resource(ResourceDesc::tex2d(
					EResourceFormat::rgba32_float,
					EAccessType::gpu_local,
					EResourceUsageFlag::render_target | EResourceUsageFlag::shader_resource | EResourceUsageFlag::unordered_access,
					1024, 768, 1, 1), &ClearValue::as_color(EResourceFormat::rgba32_float, clear_color)));

				u32 sz = 1024;
				for (u32 i = 0; i < 11; ++i)
				{
					luset(r->m_lighting_accms[i], Renderer::device()->new_resource(ResourceDesc::tex2d(
						EResourceFormat::r32_float,
						EAccessType::gpu_local,
						EResourceUsageFlag::unordered_access | EResourceUsageFlag::shader_resource,
						sz, sz, 1, 1)));

					sz >>= 1;
				}
			}
			lucatchret;
			return r;
		}
	}
}