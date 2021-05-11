// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneEditor.hpp
* @author JXMaster
* @date 2020/5/15
*/
#pragma once
#include "../IAssetEditor.hpp"
#include "../IAssetEditorType.hpp"
#include "../IAssetImporterType.hpp"
#include "../IComponentEditorType.hpp"
#include "../ISceneComponentEditorType.hpp"
#include <Runtime/HashMap.hpp>

namespace Luna
{
	namespace editor
	{
		struct CameraCB
		{
			Float4x4 world_to_view;
			Float4x4 view_to_proj;
			Float4x4 world_to_proj;
			Float4x4 view_to_world;
			Float4 env_color;
		};

		struct LightingParams
		{
			Float3U strength;
			f32 attenuation_power;
			Float3U direction;
			u32 type;
			Float3U position;
			f32 spot_attenuation_power;
		};

		struct ToneMappingParams
		{
			f32 exposure = 1.0f;
		};

		struct SkyboxParams
		{
			Float4x4 view_to_world;
			f32 fov;
			u32 width;
			u32 height;
		};

		class SceneEditorType;
		class SceneEditor : public IAssetEditor
		{
		public:
			lucid("{c973cc28-78e7-4be5-a391-8c2e5960fa48}");
			luiimpl(SceneEditor, IAssetEditor, IObject);

			SceneEditorType* m_type;

			WP<Scene::IScene> m_scene;

			// States for entity list.
			i32 m_new_entity_current_item = 0;
			u32 m_current_select_entity = 0;
			bool m_name_editing = false;
			String m_name_editing_buf = String();

			// States for component grid.
			WP<Scene::IEntity> m_current_entity;
			Vector<P<IComponentEditor>> m_component_editors;

			// States for scene component grid.
			Vector<P<ISceneComponentEditor>> m_scene_component_editors;

			// States for scene viewport.
			CameraCB m_camera_cb_data;
			P<Gfx::IResource> m_camera_cb;
			P<Gfx::ICommandBuffer> m_scene_cmdbuf;

			P<Gfx::IRenderPass> m_back_buffer_clear_pass;
			P<Gfx::IFrameBuffer> m_back_buffer_clear_fbo;

			P<Gfx::IFrameBuffer> m_grid_fb;
			P<Gfx::IViewSet> m_grid_views;

			P<Gfx::IResource> m_model_matrices;
			usize m_num_model_matrices = 0;

			P<Gfx::IResource> m_skybox_params;

			P<Gfx::IResource> m_lighting_params;
			usize m_num_lights = 0;

			P<Gfx::IResource> m_tone_mapping_offset;
			P<Gfx::IResource> m_tone_mapping_params;

			ImGui::EGizmoMode m_gizmo_mode = ImGui::EGizmoMode::local;
			ImGui::EGizmoOperation m_gizmo_op = ImGui::EGizmoOperation::translate;

			f32 m_camera_speed = 1.0f;

			bool m_wireframe = false;

			bool m_navigating = false;
			Int2U m_scene_click_pos;	// Stores the click mouse position in screen space.

			bool m_open = true;

			SceneEditor() {}

			void init();

			void draw_entity_list(ImGui::IContext* ctx, Vector<Scene::IEntity*>& entities);
			void draw_scene_components_grid(ImGui::IContext* ctx);
			void draw_scene(ImGui::IContext* ctx, Vector<Scene::IEntity*>& entities);
			void draw_components_grid(ImGui::IContext* ctx, Vector<Scene::IEntity*>& entities);

			virtual void on_render(ImGui::IContext* ctx) override;
			virtual bool closed() override
			{
				return !m_open;
			}
		};

		class SceneEditorType : public IAssetEditorType
		{
		public:
			lucid("{5b4aea33-e61a-4042-ba91-1f4ec84f8194}");
			luiimpl(SceneEditorType, IAssetEditorType, IObject);
			Name m_name;

			HashMap<Name, P<IComponentEditorType>> m_component_editor_types;

			HashMap<Name, P<ISceneComponentEditorType>> m_scene_component_editor_types;

			// Resources for rendering grids.
			P<Gfx::IResource> m_grid_vb;
			P<Gfx::IShaderInputLayout> m_grid_slayout;
			P<Gfx::IRenderPass> m_grid_rp;
			P<Gfx::IPipelineState> m_grid_pso;

			// Resources for rendering debug meshes.
			P<Gfx::IPipelineState> m_debug_mesh_renderer_pso;
			P<Gfx::IRenderPass> m_debug_mesh_renderer_rp;
			P<Gfx::IShaderInputLayout> m_debug_mesh_renderer_slayout;

			// Depth Pass.
			P<Gfx::IPipelineState> m_depth_pass_pso;
			P<Gfx::IRenderPass> m_depth_pass_rp;
			P<Gfx::IShaderInputLayout> m_depth_pass_slayout;

			// Sky box Pass.
			P<Gfx::IPipelineState> m_skybox_pass_pso;
			P<Gfx::IRenderPass> m_skybox_pass_rp;
			P<Gfx::IShaderInputLayout> m_skybox_pass_slayout;

			// Lighting Pass.
			P<Gfx::IPipelineState> m_lighting_pass_pso;
			P<Gfx::IRenderPass> m_lighting_pass_rp;
			P<Gfx::IShaderInputLayout> m_lighting_pass_slayout;

			P<Gfx::IResource> m_default_base_color;	// 1.0f, 1.0f, 1.0f, 1.0f
			P<Gfx::IResource> m_default_roughness;	// 0.5f
			P<Gfx::IResource> m_default_normal;		// 0.5f, 0.5f, 1.0f, 1.0f
			P<Gfx::IResource> m_default_metallic;	// 0.0f
			P<Gfx::IResource> m_default_emissive;	// 0.0f, 0.0f, 0.0f, 0.0f

			// Tone mapping pass.
			P<Gfx::IShaderInputLayout> m_first_lum_pass_slayout;
			P<Gfx::IPipelineState> m_first_lum_pass_pso;
			P<Gfx::IShaderInputLayout> m_lum_pass_slayout;
			P<Gfx::IPipelineState> m_lum_pass_pso;
			P<Gfx::IShaderInputLayout> m_tone_mapping_pass_slayout;
			P<Gfx::IPipelineState> m_tone_mapping_pass_pso;

			SceneEditorType() :
				m_name(Name("Scene")) {}

			RV init();

			virtual Name asset_type() override
			{
				return m_name;
			}

			//! Called when the tile is going to be drawn in asset browser.
			virtual void on_draw_tile(ImGui::IContext* ctx, Asset::IAsset* asset, const RectF& draw_rect) override;

			//! Called when a new editor is requested to be open for the specified asset.
			virtual P<IAssetEditor> new_editor(Asset::IAsset* editing_asset) override;

			//! Registers one component editor type.
			void register_component_editor_type(IComponentEditorType* component_type);

			//! Registers one scene component editor type.
			void register_scene_component_editor_type(ISceneComponentEditorType* component_type);
		};

		class SceneCreator : public IAssetEditor
		{
		public:
			lucid("{06301335-bcf6-4926-8bee-7c10a6de1c84}");
			luiimpl(SceneCreator, IAssetEditor, IObject);

			Path m_create_dir;
			String m_asset_name;
			bool m_open;

			SceneCreator() :
				m_open(true) 
			{
				m_asset_name = String();
			}

			virtual void on_render(ImGui::IContext* ctx) override;
			virtual bool closed() override
			{
				return !m_open;
			}
		};

		class SceneCreatorType : public IAssetImporterType
		{
		public:
			lucid("{c28db50c-5a2f-45c4-9200-44f7c8327feb}");
			luiimpl(SceneCreatorType, IAssetImporterType, IObject);

			Name m_name;

			SceneCreatorType() :
				m_name(Name("Scene")) {}

			virtual Name name() override
			{
				return m_name;
			}

			virtual P<IAssetEditor> new_importer(const Path& create_dir) override
			{
				auto dialog = newobj<SceneCreator>();
				dialog->m_create_dir = create_dir;
				return dialog;
			}
		};
	}
}