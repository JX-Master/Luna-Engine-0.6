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

namespace luna
{
	namespace e3d
	{
		void deinit()
		{
			auto _ = scene::unregister_scene_component_type(&g_scene_renderer_type.get());
			_ = scene::unregister_component_type(&g_transform_type.get());
			_ = scene::unregister_component_type(&g_camera_type.get());
			_ = scene::unregister_component_type(&g_directional_light_type.get());
			_ = scene::unregister_component_type(&g_point_light_type.get());
			_ = scene::unregister_component_type(&g_spot_light_type.get());
			_ = asset::unregister_asset_type(intern_name("Mesh"));
			_ = asset::unregister_asset_type(intern_name("Model"));
			_ = asset::unregister_asset_type(intern_name("Material"));
			_ = scene::unregister_component_type(&g_model_renderer_type.get());
		}

		LUNA_3DENGINE_API RV init()
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
				luexp(asset::register_asset_type(&g_mesh_type.get()));
				luexp(scene::register_component_type(&g_transform_type.get()));
				luexp(scene::register_component_type(&g_camera_type.get()));
				luexp(scene::register_component_type(&g_directional_light_type.get()));
				luexp(scene::register_component_type(&g_point_light_type.get()));
				luexp(scene::register_component_type(&g_spot_light_type.get()));
				luexp(scene::register_scene_component_type(&g_scene_renderer_type.get()));
				luexp(asset::register_asset_type(&g_material_type.get()));
				luexp(asset::register_asset_type(&g_model_type.get()));
				luexp(scene::register_component_type(&g_model_renderer_type.get()));
			}
			lucatch
			{
				return lures;
			}

			add_module("3DEngine", deinit);
			return s_ok;
		}

		LUNA_3DENGINE_API RP<IMesh> new_mesh()
		{
			auto mesh = asset::new_asset(intern_name("Mesh"));
			if (failed(mesh))
			{
				return mesh.result();
			}
			return mesh.get();
		}

		LUNA_3DENGINE_API RP<IMaterial> new_material()
		{
			auto mat = asset::new_asset(intern_name("Material"));
			if (failed(mat))
			{
				return mat.result();
			}
			return mat.get();
		}

		LUNA_3DENGINE_API RP<IModel> new_model()
		{
			auto m = asset::new_asset(intern_name("Model"));
			if (failed(m))
			{
				return m.result();
			}
			return m.get();
		}
	}
}