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
#include <Base/HashMap.hpp>

namespace luna
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
			float32 attenuation_power;
			Float3U direction;
			uint32 type;
			Float3U position;
			float32 spot_attenuation_power;
		};

		struct ToneMappingParams
		{
			float32 exposure = 1.0f;
		};

		struct SkyboxParams
		{
			Float4x4 view_to_world;
			float32 fov;
			uint32 width;
			uint32 height;
		};

		class SceneEditorType;
		class SceneEditor : public IAssetEditor
		{
		public:
			lucid("{c973cc28-78e7-4be5-a391-8c2e5960fa48}");
			luiimpl(SceneEditor, IAssetEditor, IObject);

			SceneEditorType* m_type;

			WP<scene::IScene> m_scene;

			// States for entity list.
			int32 m_new_entity_current_item = 0;
			uint32 m_current_select_entity = 0;
			bool m_name_editing = false;
			P<IStringBuffer> m_name_editing_buf = new_string_buffer();

			// States for component grid.
			WP<scene::IEntity> m_current_entity;
			Vector<P<IComponentEditor>> m_component_editors;

			// States for scene component grid.
			Vector<P<ISceneComponentEditor>> m_scene_component_editors;

			// States for scene viewport.
			CameraCB m_camera_cb_data;
			P<gfx::IResource> m_camera_cb;
			P<gfx::ICommandBuffer> m_scene_cmdbuf;

			P<gfx::IRenderPass> m_back_buffer_clear_pass;
			P<gfx::IFrameBuffer> m_back_buffer_clear_fbo;

			P<gfx::IFrameBuffer> m_grid_fb;
			P<gfx::IViewSet> m_grid_views;

			P<gfx::IResource> m_model_matrices;
			size_t m_num_model_matrices = 0;

			P<gfx::IResource> m_skybox_params;

			P<gfx::IResource> m_lighting_params;
			size_t m_num_lights = 0;

			P<gfx::IResource> m_tone_mapping_offset;
			P<gfx::IResource> m_tone_mapping_params;

			imgui::EGizmoMode m_gizmo_mode = imgui::EGizmoMode::local;
			imgui::EGizmoOperation m_gizmo_op = imgui::EGizmoOperation::translate;

			float32 m_camera_speed = 1.0f;

			bool m_wireframe = false;

			bool m_navigating = false;
			Int2U m_scene_click_pos;	// Stores the click mouse position in screen space.

			bool m_open = true;

			SceneEditor(IAllocator* alloc) :
				luibind(alloc) {}

			void init();

			void draw_entity_list(imgui::IContext* ctx, Vector<scene::IEntity*>& entities);
			void draw_scene_components_grid(imgui::IContext* ctx);
			void draw_scene(imgui::IContext* ctx, Vector<scene::IEntity*>& entities);
			void draw_components_grid(imgui::IContext* ctx, Vector<scene::IEntity*>& entities);

			virtual void on_render(imgui::IContext* ctx) override;
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
			P<IName> m_name;

			HashMap<P<IName>, P<IComponentEditorType>> m_component_editor_types;

			HashMap<P<IName>, P<ISceneComponentEditorType>> m_scene_component_editor_types;

			// Resources for rendering grids.
			P<gfx::IResource> m_grid_vb;
			P<gfx::IShaderInputLayout> m_grid_slayout;
			P<gfx::IRenderPass> m_grid_rp;
			P<gfx::IPipelineState> m_grid_pso;

			// Resources for rendering debug meshes.
			P<gfx::IPipelineState> m_debug_mesh_renderer_pso;
			P<gfx::IRenderPass> m_debug_mesh_renderer_rp;
			P<gfx::IShaderInputLayout> m_debug_mesh_renderer_slayout;

			// Depth Pass.
			P<gfx::IPipelineState> m_depth_pass_pso;
			P<gfx::IRenderPass> m_depth_pass_rp;
			P<gfx::IShaderInputLayout> m_depth_pass_slayout;

			// Sky box Pass.
			P<gfx::IPipelineState> m_skybox_pass_pso;
			P<gfx::IRenderPass> m_skybox_pass_rp;
			P<gfx::IShaderInputLayout> m_skybox_pass_slayout;

			// Lighting Pass.
			P<gfx::IPipelineState> m_lighting_pass_pso;
			P<gfx::IRenderPass> m_lighting_pass_rp;
			P<gfx::IShaderInputLayout> m_lighting_pass_slayout;

			P<gfx::IResource> m_default_base_color;	// 1.0f, 1.0f, 1.0f, 1.0f
			P<gfx::IResource> m_default_roughness;	// 0.5f
			P<gfx::IResource> m_default_normal;		// 0.5f, 0.5f, 1.0f, 1.0f
			P<gfx::IResource> m_default_metallic;	// 0.0f
			P<gfx::IResource> m_default_emissive;	// 0.0f, 0.0f, 0.0f, 0.0f

			// Tone mapping pass.
			P<gfx::IShaderInputLayout> m_first_lum_pass_slayout;
			P<gfx::IPipelineState> m_first_lum_pass_pso;
			P<gfx::IShaderInputLayout> m_lum_pass_slayout;
			P<gfx::IPipelineState> m_lum_pass_pso;
			P<gfx::IShaderInputLayout> m_tone_mapping_pass_slayout;
			P<gfx::IPipelineState> m_tone_mapping_pass_pso;

			SceneEditorType(IAllocator* alloc) :
				luibind(alloc),
				m_name(intern_name("Scene")),
				m_component_editor_types(alloc),
				m_scene_component_editor_types(alloc) {}

			RV init();

			virtual IName* asset_type() override
			{
				return m_name;
			}

			//! Called when the tile is going to be drawn in asset browser.
			virtual void on_draw_tile(imgui::IContext* ctx, asset::IAsset* asset, const RectF& draw_rect) override;

			//! Called when a new editor is requested to be open for the specified asset.
			virtual P<IAssetEditor> new_editor(asset::IAsset* editing_asset) override;

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

			P<IPath> m_create_dir;
			P<IStringBuffer> m_asset_name;
			bool m_open;

			SceneCreator(IAllocator* alloc) :
				luibind(alloc),
				m_open(true) 
			{
				m_asset_name = new_string_buffer();
			}

			virtual void on_render(imgui::IContext* ctx) override;
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

			P<IName> m_name;

			SceneCreatorType(IAllocator* alloc) :
				luibind(alloc),
				m_name(intern_name("Scene")) {}

			virtual IName* name() override
			{
				return m_name;
			}

			virtual P<IAssetEditor> new_importer(IPath* create_dir) override
			{
				auto dialog = box_ptr(new_obj<SceneCreator>(get_module_allocator()));
				dialog->m_create_dir = create_dir->clone();
				return dialog;
			}
		};
	}
}