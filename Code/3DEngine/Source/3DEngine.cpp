// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file 3DEngine.cpp
* @author JXMaster
* @date 2020/5/15
*/
#include "3DEngineHeader.hpp"
#include "Mesh.hpp"
#include "Transform.hpp"
#include "Camera.hpp"
#include "Lights.hpp"
#include "SceneRenderer.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "ModelRenderer.hpp"
#include <Runtime/Module.hpp>

namespace Luna
{
	namespace E3D
	{
		void deinit()
		{
			auto _ = Scene::unregister_scene_component_type(&g_scene_renderer_type.get());
			_ = Scene::unregister_component_type(&g_transform_type.get());
			_ = Scene::unregister_component_type(&g_camera_type.get());
			_ = Scene::unregister_component_type(&g_directional_light_type.get());
			_ = Scene::unregister_component_type(&g_point_light_type.get());
			_ = Scene::unregister_component_type(&g_spot_light_type.get());
			_ = Asset::unregister_asset_type(Name("Mesh"));
			_ = Asset::unregister_asset_type(Name("Model"));
			_ = Asset::unregister_asset_type(Name("Material"));
			_ = Scene::unregister_component_type(&g_model_renderer_type.get());
		}

		RV init()
		{
			g_mesh_type.construct();
			g_transform_type.construct();
			g_camera_type.construct();

			g_directional_light_type.construct();
			g_point_light_type.construct();
			g_spot_light_type.construct();

			g_scene_renderer_type.construct();

			g_material_type.construct();

			g_model_type.construct();
			g_model_renderer_type.construct();

			auto p1 = box_ptr(&g_mesh_type.get());
			auto p2 = box_ptr(&g_transform_type.get());
			auto p3 = box_ptr(&g_camera_type.get());
			auto p4 = box_ptr(&g_directional_light_type.get());
			auto p5 = box_ptr(&g_point_light_type.get());
			auto p6 = box_ptr(&g_spot_light_type.get());
			auto p7 = box_ptr(&g_scene_renderer_type.get());
			auto p8 = box_ptr(&g_material_type.get());
			auto p9 = box_ptr(&g_model_type.get());
			auto p10 = box_ptr(&g_model_renderer_type.get());

			lutry
			{
				luexp(Asset::register_asset_type(&g_mesh_type.get()));
				luexp(Scene::register_component_type(&g_transform_type.get()));
				luexp(Scene::register_component_type(&g_camera_type.get()));
				luexp(Scene::register_component_type(&g_directional_light_type.get()));
				luexp(Scene::register_component_type(&g_point_light_type.get()));
				luexp(Scene::register_component_type(&g_spot_light_type.get()));
				luexp(Scene::register_scene_component_type(&g_scene_renderer_type.get()));
				luexp(Asset::register_asset_type(&g_material_type.get()));
				luexp(Asset::register_asset_type(&g_model_type.get()));
				luexp(Scene::register_component_type(&g_model_renderer_type.get()));
			}
			lucatch
			{
				return lures;
			}
			return RV();
		}

		StaticRegisterModule m("3DEngine", "Asset;Core;Gfx;Input;Renderer;Scene", init, deinit);

		LUNA_3DENGINE_API RP<IMesh> new_mesh()
		{
			auto mesh = Asset::new_asset(Name("Mesh"));
			if (failed(mesh))
			{
				return mesh.errcode();
			}
			return mesh.get();
		}

		LUNA_3DENGINE_API RP<IMaterial> new_material()
		{
			auto mat = Asset::new_asset(Name("Material"));
			if (failed(mat))
			{
				return mat.errcode();
			}
			return mat.get();
		}

		LUNA_3DENGINE_API RP<IModel> new_model()
		{
			auto m = Asset::new_asset(Name("Model"));
			if (failed(m))
			{
				return m.errcode();
			}
			return m.get();
		}
	}
}