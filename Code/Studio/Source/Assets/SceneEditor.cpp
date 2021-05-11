// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneEditor.cpp
* @author JXMaster
* @date 2020/5/15
*/
#include "SceneEditor.hpp"
#include "../MainEditor.hpp"
#include <Runtime/Debug.hpp>

namespace Luna
{
	namespace editor
	{
		void SceneEditor::init()
		{
			using namespace Gfx;
			u32 cb_align;
			Renderer::device()->check_feature_support(EDeviceFeature::buffer_data_alignment, &cb_align);
			m_camera_cb = Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::constant_buffer, align_upper(sizeof(CameraCB), cb_align))).get();
			m_scene_cmdbuf = Renderer::main_graphic_queue()->new_command_buffer().get();
			m_grid_views = Renderer::device()->new_view_set(m_type->m_grid_slayout, ViewSetDesc(1, 0, 0, 0)).get();
			m_grid_views->set_cbv(0, m_camera_cb, ConstantBufferViewDesc(0, (u32)align_upper(sizeof(CameraCB), cb_align)));
			m_back_buffer_clear_pass = Renderer::device()->new_render_pass(
				RenderPassDesc({ AttachmentDesc(EResourceFormat::rgba8_unorm, EAttachmentLoadOp::clear, EAttachmentStoreOp::store) }, 
					EResourceFormat::d32_float, EAttachmentLoadOp::clear, EAttachmentStoreOp::store, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, 1, true)).get();

			m_lighting_params = Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::shader_resource, align_upper(sizeof(LightingParams) * 4, cb_align))).get();
			m_num_lights = 4;

			m_tone_mapping_offset = Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::constant_buffer, align_upper(sizeof(Float4) * 16, cb_align))).get();

			m_tone_mapping_params = Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::constant_buffer, align_upper(sizeof(ToneMappingParams), cb_align))).get();
		
			m_skybox_params = Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::constant_buffer, align_upper(sizeof(SkyboxParams), cb_align))).get();
		}
		void SceneEditor::draw_entity_list(ImGui::IContext* ctx, Vector<Scene::IEntity*>& entities)
		{
			auto s = m_scene.lock();

			// Draw entity list.
			ctx->text("Entity List");

			ctx->same_line();

			if (ctx->button("New Entity"))
			{
				char name[64];
				strcpy_s(name, "New_Entity");
				auto entity = s->add_entity(Name(name));
				if (entity.errcode() == BasicError::already_exists())
				{
					u32 index = 0;
					// Append index.
					while (failed(entity))
					{
						sprintf_s(name, "New_Entity_%u", index);
						entity = s->add_entity(Name(name));
						++index;
					}
				}
			}

			auto avail = ctx->get_content_region_avail();

			ctx->push_style_var(ImGui::EStyle::child_rounding, 5.0f);
			ctx->begin_child("Entity List", Float2(avail.x, avail.y / 2.0f), true);

			
			bool any_entity_deleted = false;
			if (entities.empty())
			{
				ctx->text("No entity in the scene.");
			}
			else
			{
				const char* entity_popup_id = "Entity Popup";

				Float2 sel_size{ ctx->get_window_width(), ctx->get_text_line_height() };

				for (u32 i = 0; i < (u32)entities.size(); ++i)
				{
					Float2 sel_pos = ctx->get_cursor_screen_pos();
					if (in_rect(ImGui::io_get_mouse_pos(ctx->get_io()), sel_pos, sel_pos + sel_size) && !ctx->is_popup_open(entity_popup_id) &&
						(ctx->is_mouse_clicked(Input::EMouseKey::lb) || ctx->is_mouse_clicked(Input::EMouseKey::rb)))
					{
						if (i != m_current_select_entity)
						{
							m_current_select_entity = i;

							// Update scene components.
							m_component_editors.clear();
							auto entity = entities[i];
							auto components = entity->components();
							for (auto& i : components)
							{
								auto iter = m_type->m_component_editor_types.find(i->type_object()->type_name());
								if (iter != m_type->m_component_editor_types.end())
								{
									auto editor = iter->second->new_editor(i);
									m_component_editors.push_back(editor);
								}
							}
						}
					}
					if (i == m_current_select_entity && m_name_editing)
					{
						ctx->input_text("###NameEdit", m_name_editing_buf);
						if (!in_rect(ImGui::io_get_mouse_pos(ctx->get_io()), sel_pos, sel_pos + sel_size) && ctx->is_mouse_clicked(Input::EMouseKey::lb))
						{
							for (usize i = 0; i < m_name_editing_buf.size(); ++i)
							{
								if (m_name_editing_buf.data()[i] == ' ')
								{
									m_name_editing_buf.data()[i] = '_';
								}
							}
							auto rename_res = entities[i]->set_name(Name(m_name_editing_buf.c_str()));
							if (failed(rename_res))
							{
								auto _ = Gfx::message_box(get_errmsg(rename_res.errcode()), "Entity Rename Failed.", Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
							}
							m_name_editing = false;
						}
					}
					else
					{
						// Draw highlight.
						if (i == m_current_select_entity)
						{
							auto dl = ctx->get_window_draw_list();
							ImGui::dl_add_rect_filled(dl, sel_pos, sel_pos + sel_size,
								color_to_abgr8(ctx->get_style()->Colors[(u32)ImGui::EColor::button]));
						}
						ctx->text(entities[i]->name().c_str());
					}

					if (in_rect(ImGui::io_get_mouse_pos(ctx->get_io()), sel_pos, sel_pos + sel_size) && ctx->is_mouse_clicked(Input::EMouseKey::rb))
					{
						ctx->open_popup(entity_popup_id);
					}

				}

				if (ctx->begin_popup(entity_popup_id))
				{
					if (ctx->selectable("Rename"))
					{
						m_name_editing = true;
						m_name_editing_buf.assign(entities[m_current_select_entity]->name().c_str());
						ctx->close_current_popup();
					}
					if (ctx->selectable("Remove"))
					{
						auto _ = s->remove_entity(entities[m_current_select_entity]->name());
						any_entity_deleted = true;
						ctx->close_current_popup();
					}

					ctx->end_popup();
				}
			}

			if (any_entity_deleted)
			{
				entities = s->entities();
			}

			ctx->end_child();
			ctx->pop_style_var();
		}
		void SceneEditor::draw_scene_components_grid(ImGui::IContext* ctx)
		{
			ctx->text("Scene Components");

			ctx->push_style_var(ImGui::EStyle::child_rounding, 5.0f);
			ctx->begin_child("Scene Components", Float2(0.0f, 0.0f), true);

			auto s = m_scene.lock();
			auto components = s->scene_components();
			if (components.empty())
			{
				ctx->text("No Components");
			}
			else
			{
				for (auto& i : components)
				{
					if (ctx->collapsing_header(i->type_object()->type_name().c_str(), ImGui::ETreeNodeFlag::default_open))
					{
						bool has_editor = false;
						for (auto& j : m_scene_component_editors)
						{
							if (j->type_name() == i->type_object()->type_name())
							{
								has_editor = true;
								j->on_render(ctx);
								break;
							}
						}
						// Check editor instance.
						if (!has_editor)
						{
							auto iter = m_type->m_scene_component_editor_types.find(i->type_object()->type_name());
							if (iter != m_type->m_scene_component_editor_types.end())
							{
								m_scene_component_editors.push_back(iter->second->new_editor(i));
								has_editor = true;
							}
						}
						if (!has_editor)
						{
							ctx->text_wrapped("The scene component editor type is not registered. The scene component cannot be edited.");
						}
						ctx->push_id(i);

						if (ctx->button("Remove"))
						{
							// Remove editor if any.
							for (auto iter = m_scene_component_editors.begin(); iter != m_scene_component_editors.end(); ++iter)
							{
								if ((*iter)->type_name() == i->type_object()->type_name())
								{
									m_scene_component_editors.erase(iter);
									break;
								}
							}
							auto _ = s->remove_scene_component(i->type_object()->type_name());
						}

						ctx->pop_id();
					}
				}
			}

			const char* new_comp_popup = "NewSceneCompPopup";

			if (ctx->button("New Scene Component"))
			{
				ctx->open_popup(new_comp_popup);
			}

			if (ctx->begin_popup(new_comp_popup))
			{
				auto component_types = Scene::scene_component_types();

				for (auto& i : component_types)
				{
					auto name = i->type_name();
					auto exists = s->get_scene_component(name);
					if (failed(exists))
					{
						// Show enabled.
						if (ctx->selectable(name.c_str()))
						{
							auto res = s->add_scene_component(name);
							if (failed(res))
							{
								debug_printf(get_errmsg(res.errcode()));
							}
							ctx->close_current_popup();
						}
					}
					else
					{
						// Show disabled.
						ctx->selectable(name.c_str(), false, ImGui::ESelectableFlag::disabled);
					}
				}
				ctx->end_popup();
			}

			ctx->end_child();

			ctx->pop_style_var();
		}

		void SceneEditor::draw_scene(ImGui::IContext* ctx, Vector<Scene::IEntity*>& entities)
		{
			ctx->text("Scene");

			auto s = m_scene.lock();

			auto _scene_renderer = s->get_scene_component(Name("Scene Renderer"));
			if (failed(_scene_renderer))
			{
				ctx->text("Please add Scene Renderer Component to the scene.");
				return;
			}
			P<E3D::ISceneRenderer> scene_renderer = _scene_renderer.get();
			auto render_tex = scene_renderer->screen_buffer();
			auto depth_tex = scene_renderer->depth_buffer();

			// Fetch camera and transform component.
			auto camera_entity = scene_renderer->camera_entity();
			if (!camera_entity)
			{
				ctx->text("Camera Entity is not set in Scene Renderer Component.");
				return;
			}

			auto _transform_component = camera_entity->get_component(Name("Transform"));
			auto _camera_component = camera_entity->get_component(Name("Camera"));

			if (failed(_transform_component) || failed(_camera_component))
			{
				ctx->text("Transform and Camera Component must be set to the Camera Entity set in Scene Renderer Component.");
				return;
			}

			P<E3D::ICamera> camera = _camera_component.get();
			P<E3D::ITransform> transform = _transform_component.get();

			ctx->begin_child("Scene Viewport", Float2(0.0f, 0.0f), false, ImGui::EWindowFlag::no_move | ImGui::EWindowFlag::no_scrollbar);

			ctx->set_next_item_width(100.0f);
			ctx->slider_float("Camera Speed", &m_camera_speed, 0.1f, 10.0f, "%.3f", 3.3f);
			ctx->same_line();
			{
				// Draw gizmo mode combo.
				ctx->text("Gizmo Mode");
				ctx->same_line();
				auto mode = m_gizmo_mode;
				if (m_gizmo_mode != ImGui::EGizmoMode::local)
				{
					ctx->push_style_color(ImGui::EColor::text, Float4(1.0f, 1.0f, 1.0f, 0.5f));
				}
				if (ctx->button("Local"))
				{
					mode = ImGui::EGizmoMode::local;
				}
				if (m_gizmo_mode != ImGui::EGizmoMode::local)
				{
					ctx->pop_style_color();
				}
				ctx->same_line(0);
				if (m_gizmo_mode != ImGui::EGizmoMode::world)
				{
					ctx->push_style_color(ImGui::EColor::text, Float4(1.0f, 1.0f, 1.0f, 0.5f));
				}
				if (ctx->button("World"))
				{
					mode = ImGui::EGizmoMode::world;
				}
				if (m_gizmo_mode != ImGui::EGizmoMode::world)
				{
					ctx->pop_style_color();
				}
				m_gizmo_mode = mode;
				ctx->same_line();

				// Draw gizmo operation combo.
				ctx->text("Gizmo Operation");
				ctx->same_line();
				auto op = m_gizmo_op;
				if (m_gizmo_op != ImGui::EGizmoOperation::translate)
				{
					ctx->push_style_color(ImGui::EColor::text, Float4(1.0f, 1.0f, 1.0f, 0.5f));
				}
				if (ctx->button("Translate"))
				{
					op = ImGui::EGizmoOperation::translate;
				}
				if (m_gizmo_op != ImGui::EGizmoOperation::translate)
				{
					ctx->pop_style_color();
				}
				ctx->same_line(0);
				if (m_gizmo_op != ImGui::EGizmoOperation::rotate)
				{
					ctx->push_style_color(ImGui::EColor::text, Float4(1.0f, 1.0f, 1.0f, 0.5f));
				}
				if (ctx->button("Rotate"))
				{
					op = ImGui::EGizmoOperation::rotate;
				}
				if (m_gizmo_op != ImGui::EGizmoOperation::rotate)
				{
					ctx->pop_style_color();
				}
				ctx->same_line(0);
				if (m_gizmo_op != ImGui::EGizmoOperation::scale)
				{
					ctx->push_style_color(ImGui::EColor::text, Float4(1.0f, 1.0f, 1.0f, 0.5f));
				}
				if (ctx->button("Scale"))
				{
					op = ImGui::EGizmoOperation::scale;
				}
				if (m_gizmo_op != ImGui::EGizmoOperation::scale)
				{
					ctx->pop_style_color();
				}
				m_gizmo_op = op;
			}

			ctx->same_line();
			ctx->checkbox("Wireframe", &m_wireframe);

			Float2 scene_sz = ctx->get_content_region_avail();
			Float2 scene_pos = ctx->get_cursor_screen_pos();
			scene_sz.x -= 1.0f;
			scene_sz.y -= 5.0f;

			auto render_desc = render_tex->desc();
			if (render_desc.width != scene_sz.x || render_desc.height != scene_sz.y)
			{
				auto _ = scene_renderer->resize_screen_buffer(UInt2U((u32)scene_sz.x, (u32)scene_sz.y));
				render_tex = scene_renderer->screen_buffer();
				depth_tex = scene_renderer->depth_buffer();
			}

			render_desc = render_tex->desc();
			camera->set_aspect_ratio((f32)render_desc.width / (f32)render_desc.height);
			
			// Update and upload camera data.
			m_camera_cb_data.world_to_view = transform->world_to_local_matrix();
			m_camera_cb_data.view_to_proj = camera->projection_matrix();
			m_camera_cb_data.world_to_proj = mul(m_camera_cb_data.world_to_view, m_camera_cb_data.view_to_proj);
			m_camera_cb_data.view_to_world = transform->local_to_world_matrix();
			Float3 env_color = scene_renderer->environment_color();
			m_camera_cb_data.env_color = Float4(env_color.x, env_color.y, env_color.z, 1.0f);
			auto mapped = m_camera_cb->map_subresource(0, false, 1, 0);
			luassert_always(succeeded(mapped));
			memcpy(mapped.get(), &m_camera_cb_data, sizeof(CameraCB));
			m_camera_cb->unmap_subresource(0, false, 0, sizeof(CameraCB));

			// Draw Scene.
			{
				using namespace Gfx;

				// Clear render and stencil pass.
				{
					ResourceBarrierDesc ts[2] = {
						ResourceBarrierDesc::as_transition(render_tex, EResourceState::render_target),
						ResourceBarrierDesc::as_transition(depth_tex, EResourceState::depth_stencil_write)
					};
					m_scene_cmdbuf->resource_barriers(2, ts);
					m_back_buffer_clear_fbo = Renderer::device()->new_frame_buffer(m_back_buffer_clear_pass, 1, &render_tex, nullptr, depth_tex, nullptr).get();
					auto clear_color = Float4U(0.0f, 0.0f, 0.0f, 1.0f);
					m_scene_cmdbuf->begin_render_pass(m_back_buffer_clear_pass, m_back_buffer_clear_fbo, 1, &clear_color, 1.0f, 0xFF);
					m_scene_cmdbuf->end_render_pass();
				}

				// Fetch meshes to draw.
				Vector<P<E3D::ITransform>> ts;
				Vector<P<E3D::IModelRenderer>> rs;
				for (auto& i : entities)
				{
					auto t = i->get_component(Name("Transform"));
					auto r = i->get_component(Name("Model Renderer"));
					if (succeeded(t) && succeeded(r))
					{
						P<E3D::IModelRenderer> ren = r.get();
						auto model = ren->model().lock();
						if (!model)
						{
							continue;
						}
						auto mesh = model->mesh().lock();
						if (!mesh)
						{
							continue;
						}
						ts.push_back(t.get());
						rs.push_back(r.get());
					}
				}

				// Upload mesh matrices.
				{
					if (m_num_model_matrices < ts.size())
					{
						m_model_matrices = Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::shader_resource, (u64)sizeof(Float4x4) * 2 * (u64)ts.size())).get();
						m_num_model_matrices = ts.size();
					}
					if (!ts.empty())
					{
						void* mapped = m_model_matrices->map_subresource(0, false, 1, 0).get();
						for (usize i = 0; i < ts.size(); ++i)
						{
							Float4x4 m2w = ts[i]->local_to_world_matrix();
							Float4x4 w2m = ts[i]->world_to_local_matrix();
							memcpy((Float4x4*)mapped + i * 2, &m2w._11, sizeof(Float4x4));
							memcpy((Float4x4*)mapped + (i * 2 + 1), &w2m._11, sizeof(Float4x4));
						}
						m_model_matrices->unmap_subresource(0, false, 0, sizeof(Float4x4) * 2 * ts.size());
					}
				}

				// Fetches lights to draw.
				Vector<P<E3D::ITransform>> light_ts;
				Vector<P<E3D::ILight>> light_rs;
				for (auto& i : entities)
				{
					auto t = i->get_component(Name("Transform"));
					auto r = i->get_component(Name("Directional Light"));
					if (failed(r))
					{
						r = i->get_component(Name("Point Light"));
						if (failed(r))
						{
							r = i->get_component(Name("Spot Light"));
						}
					}
					if (succeeded(t) && succeeded(r))
					{
						light_ts.push_back(t.get());
						light_rs.push_back(r.get());
					}
				}

				// Upload lighting params.
				{
					if (m_num_lights < light_ts.size())
					{
						m_lighting_params = Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::shader_resource, sizeof(LightingParams) * light_ts.size())).get();
						m_num_lights = light_ts.size();
					}
					void* mapped = m_lighting_params->map_subresource(0, false, 1, 0).get();
					for (usize i = 0; i < light_ts.size(); ++i)
					{
						LightingParams p;
						P<E3D::IDirectionalLight> directional = light_rs[i];
						if (directional)
						{
							p.strength = directional->intensity();
							p.attenuation_power = 1.0f;
							p.direction = Float4x4::make_rotation_quaternion(light_ts[i]->world_rotation()).forward();
							p.type = 0;
							p.position = light_ts[i]->world_position();
							p.spot_attenuation_power = 0.0f;
						}
						else
						{
							P<E3D::IPointLight> point = light_rs[i];
							if (point)
							{
								p.strength = point->intensity();
								p.attenuation_power = point->attenuation_power();
								p.direction = Float3U(0.0f, 0.0f, 1.0f);
								p.type = 1;
								p.position = light_ts[i]->world_position();
								p.spot_attenuation_power = 0.0f;
							}
							else
							{
								P<E3D::ISpotLight> spot = light_rs[i];
								if (spot)
								{
									p.strength = spot->intensity();
									p.attenuation_power = spot->attenuation_power();
									p.direction = Float4x4::make_rotation_quaternion(light_ts[i]->world_rotation()).forward();
									p.type = 2;
									p.position = light_ts[i]->world_position();
									p.spot_attenuation_power = spot->spot_power();
								}
								else
								{
									lupanic_always();
								}
							}
						}
						memcpy((LightingParams*)mapped + i, &p, sizeof(LightingParams));
					}
					// Adds one fake light if there is no light so the SRV is not empty (which is invalid).
					if (light_ts.empty())
					{
						LightingParams p;
						p.strength = { 0.0f, 0.0f, 0.0f };
						p.attenuation_power = 1.0f;
						p.direction = { 0.0f, 0.0f, 1.0f };
						p.type = 0;
						p.position = { 0.0f, 0.0f, 0.0f };
						p.spot_attenuation_power = 0.0f;
						memcpy((LightingParams*)mapped, &p, sizeof(LightingParams));
					}
					m_lighting_params->unmap_subresource(0, false, 0, light_ts.size() * sizeof(LightingParams));
				}

				u32 cb_align;
				Renderer::device()->check_feature_support(EDeviceFeature::buffer_data_alignment, &cb_align);

				if (m_wireframe)
				{
					// Debug wireframe pass.
					auto debug_renderer_fbo = Renderer::device()->new_frame_buffer(m_type->m_debug_mesh_renderer_rp, 1, &render_tex, nullptr, nullptr, nullptr).get();
					m_scene_cmdbuf->begin_render_pass(m_type->m_debug_mesh_renderer_rp, debug_renderer_fbo, 0, nullptr, 1.0f, 0xFF);
					m_scene_cmdbuf->attach_graphic_object(debug_renderer_fbo);
					m_scene_cmdbuf->set_graphic_shader_input_layout(m_type->m_debug_mesh_renderer_slayout);
					m_scene_cmdbuf->set_pipeline_state(m_type->m_debug_mesh_renderer_pso);
					m_scene_cmdbuf->set_primitive_topology(EPrimitiveTopology::triangle_list);
					m_scene_cmdbuf->set_viewport(Viewport(0.0f, 0.0f, (f32)render_desc.width, (f32)render_desc.height, 0.0f, 1.0f));
					m_scene_cmdbuf->set_scissor_rect(RectI(0, 0, (i32)render_desc.width, (i32)render_desc.height));
					// Draw Meshes.
					for (usize i = 0; i < ts.size(); ++i)
					{
						auto vs = Renderer::device()->new_view_set(m_type->m_debug_mesh_renderer_slayout, ViewSetDesc(1, 1, 0, 0)).get();
						vs->set_cbv(0, m_camera_cb, ConstantBufferViewDesc(0, (u32)align_upper(sizeof(CameraCB), cb_align)));
						vs->set_srv(0, m_model_matrices, &ShaderResourceViewDesc::as_buffer(EResourceFormat::unknown, i, 1, sizeof(Float4x4) * 2, false));
						m_scene_cmdbuf->set_graphic_view_set(vs);
						m_scene_cmdbuf->attach_graphic_object(vs);

						// Draw pieces.
						auto mesh = rs[i]->model().lock()->mesh().lock();
						m_scene_cmdbuf->set_vertex_buffers(0, 1, &VertexBufferViewDesc(mesh->vertex_buffer(), 0,
							mesh->count_vertices() * sizeof(E3D::Vertex), sizeof(E3D::Vertex)));
						m_scene_cmdbuf->set_index_buffer(mesh->index_buffer(), 0, mesh->count_indices() * sizeof(u32), EResourceFormat::r32_uint);

						u32 num_pieces = mesh->count_pieces();
						for (u32 j = 0; j < num_pieces; ++j)
						{
							m_scene_cmdbuf->draw_indexed(mesh->piece_count_indices(j), mesh->piece_first_index_offset(j), 0);
						}
					}
					m_scene_cmdbuf->end_render_pass();
				}
				else
				{
					// Depth pass.
					{
						auto fbo = Renderer::device()->new_frame_buffer(m_type->m_depth_pass_rp, 0, nullptr, nullptr, depth_tex, nullptr).get();
						m_scene_cmdbuf->begin_render_pass(m_type->m_depth_pass_rp, fbo, 0, nullptr, 1.0f, 0xFF);
						//m_scene_cmdbuf->attach_graphic_object(fbo);
						//m_scene_cmdbuf->set_graphic_shader_input_layout(m_type->m_depth_pass_slayout);
						//m_scene_cmdbuf->set_pipeline_state(m_type->m_depth_pass_pso);
						//m_scene_cmdbuf->set_primitive_topology(EPrimitiveTopology::triangle_list);
						//m_scene_cmdbuf->set_viewport(Viewport(0.0f, 0.0f, render_desc.width, render_desc.height, 0.0f, 1.0f));
						//m_scene_cmdbuf->set_scissor_rect(RectI(0, 0, render_desc.width, render_desc.height));
						//// Draw Meshes.
						//for (size_t i = 0; i < ts.size(); ++i)
						//{
						//	auto vs = renderer::device()->new_view_set(m_type->m_depth_pass_slayout, ViewSetDesc(1, 1, 0, 0)).get();
						//	vs->set_cbv(0, m_camera_cb, ConstantBufferViewDesc(0, align_upper(sizeof(CameraCB), cb_align)));
						//	vs->set_srv(0, m_model_matrices, &ShaderResourceViewDesc::as_buffer(EResourceFormat::unknown, i, 1, sizeof(Float4x4), false));
						//	m_scene_cmdbuf->set_graphic_view_set(vs);
						//	m_scene_cmdbuf->attach_graphic_object(vs);

						//	// Draw pieces.
						//	auto mesh = rs[i]->model().lock()->mesh().lock();
						//	m_scene_cmdbuf->set_vertex_buffers(0, 1, &VertexBufferViewDesc(mesh->vertex_buffer(), 0,
						//		mesh->count_vertices() * sizeof(e3d::Vertex), sizeof(e3d::Vertex)));
						//	m_scene_cmdbuf->set_index_buffer(mesh->index_buffer(), 0, mesh->count_indices() * sizeof(u32), EResourceFormat::r32_uint);

						//	size_t num_pieces = mesh->count_pieces();
						//	for (size_t j = 0; j < num_pieces; ++j)
						//	{
						//		m_scene_cmdbuf->draw_indexed(mesh->piece_count_indices(j), mesh->piece_first_index_offset(j), 0);
						//	}
						//}
						m_scene_cmdbuf->end_render_pass();
					}

					// Sky Box Pass.
					// Clears the lighting buffer to a skybox or black of the skybox is not present.
					{
						auto skybox = scene_renderer->skybox_texture().lock();
						P<IResource> skybox_tex;
						if (skybox)
						{
							auto tex = skybox->texture();
							if (succeeded(tex))
							{
								skybox_tex = tex.get();
							}
						}
						if (skybox_tex && camera->camera_type() == E3D::ECameraType::perspective)
						{
							// Draw skybox.
							auto view_to_world = transform->local_to_world_matrix();;
							SkyboxParams* mapped = (SkyboxParams*)m_skybox_params->map_subresource(0, false, 1, 0).get();
							auto camera_forward_dir = mul(Float4(0.0f, 0.0f, 1.0f, 0.0f), transform->local_to_world_matrix());
							memcpy(&mapped->view_to_world, &view_to_world, sizeof(Float4x4));
							mapped->fov = camera->field_of_view();
							mapped->width = (u32)scene_sz.x;
							mapped->height = (u32)scene_sz.y;
							m_skybox_params->unmap_subresource(0, false, 0, sizeof(SkyboxParams));

							m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(scene_renderer->lighting_buffer(), EResourceState::unordered_access));
							m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(skybox_tex, EResourceState::shader_resource_non_pixel));
							m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(m_skybox_params, EResourceState::vertex_and_constant_buffer));
							m_scene_cmdbuf->set_compute_shader_input_layout(m_type->m_skybox_pass_slayout);
							m_scene_cmdbuf->set_pipeline_state(m_type->m_skybox_pass_pso);
							auto vs = Renderer::device()->new_view_set(m_type->m_skybox_pass_slayout, ViewSetDesc(1, 1, 1, 1)).get();
							vs->set_cbv(0, m_skybox_params, ConstantBufferViewDesc(0, (u32)align_upper(sizeof(SkyboxParams), cb_align)));
							vs->set_srv(0, skybox_tex);
							vs->set_uav(0, scene_renderer->lighting_buffer());
							vs->set_sampler(0, SamplerDesc(EFilter::min_mag_mip_linear, ETextureAddressMode::warp, ETextureAddressMode::warp, ETextureAddressMode::warp));
							m_scene_cmdbuf->set_compute_view_set(vs);
							m_scene_cmdbuf->attach_graphic_object(vs);
							m_scene_cmdbuf->dispatch(max<u32>((u32)scene_sz.x / 8, 1), max<u32>((u32)scene_sz.y / 8, 1), 1);
						}
						else
						{
							// Clears to black.
							m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(scene_renderer->lighting_buffer(), EResourceState::render_target));
							auto lighting_rt = scene_renderer->lighting_buffer();
							auto fbo = Renderer::device()->new_frame_buffer(m_type->m_skybox_pass_rp, 1, &lighting_rt, nullptr, nullptr, nullptr).get();
							Float4U clear_value = { 0.0f, 0.0f, 0.0f, 0.0f };
							m_scene_cmdbuf->begin_render_pass(m_type->m_skybox_pass_rp, fbo, 1, &clear_value, 1.0f, 0xFF);
							m_scene_cmdbuf->attach_graphic_object(fbo);
							m_scene_cmdbuf->end_render_pass();
						}
					}

					// Lighting Pass.
					{
						auto lighting_rt = scene_renderer->lighting_buffer();
						auto fbo = Renderer::device()->new_frame_buffer(m_type->m_lighting_pass_rp, 1, &lighting_rt, nullptr, depth_tex, nullptr).get();
						m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(lighting_rt, EResourceState::render_target));
						m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(depth_tex, EResourceState::depth_stencil_write));
						m_scene_cmdbuf->begin_render_pass(m_type->m_lighting_pass_rp, fbo, 0, nullptr, 0.0f, 0xFF);
						m_scene_cmdbuf->attach_graphic_object(fbo);
						m_scene_cmdbuf->set_graphic_shader_input_layout(m_type->m_lighting_pass_slayout);
						m_scene_cmdbuf->set_pipeline_state(m_type->m_lighting_pass_pso);
						m_scene_cmdbuf->set_primitive_topology(EPrimitiveTopology::triangle_list);
						m_scene_cmdbuf->set_viewport(Viewport(0.0f, 0.0f, (f32)render_desc.width, (f32)render_desc.height, 0.0f, 1.0f));
						m_scene_cmdbuf->set_scissor_rect(RectI(0, 0, (i32)render_desc.width, (i32)render_desc.height));

						// Draw Meshes.
						for (usize i = 0; i < ts.size(); ++i)
						{
							auto model = rs[i]->model().lock();
							auto mesh = model->mesh().lock();
							m_scene_cmdbuf->set_vertex_buffers(0, 1, &VertexBufferViewDesc(mesh->vertex_buffer(), 0,
								mesh->count_vertices() * sizeof(E3D::Vertex), sizeof(E3D::Vertex)));
							m_scene_cmdbuf->set_index_buffer(mesh->index_buffer(), 0, mesh->count_indices() * sizeof(u32), EResourceFormat::r32_uint);

							u32 num_pieces = mesh->count_pieces();

							for (u32 j = 0; j < num_pieces; ++j)
							{
								P<Gfx::IResource> base_color_tex = m_type->m_default_base_color;
								P<Gfx::IResource> roughness_tex = m_type->m_default_roughness;
								P<Gfx::IResource> normal_tex = m_type->m_default_normal;
								P<Gfx::IResource> metallic_tex = m_type->m_default_metallic;
								P<Gfx::IResource> emissive_tex = m_type->m_default_emissive;
								P<Gfx::IResource> sky_tex = m_type->m_default_emissive;

								if (j < model->count_materials())
								{
									auto mat = model->material(j).lock();
									if (mat)
									{
										// Set material for this piece.
										P<Texture::ITexture> mat_base_color_tex = mat->base_color().lock();
										P<Texture::ITexture> mat_roughness_tex = mat->roughness().lock();
										P<Texture::ITexture> mat_normal_tex = mat->normal().lock();
										P<Texture::ITexture> mat_metallic_tex = mat->metallic().lock();
										P<Texture::ITexture> mat_emissive_tex = mat->emissive().lock();
										if (mat_base_color_tex)
										{
											auto _base_color_tex = mat_base_color_tex->texture();
											if (succeeded(_base_color_tex))
											{
												base_color_tex = _base_color_tex.get();
											}
										}
										if (mat_roughness_tex)
										{
											auto _roughness_tex = mat_roughness_tex->texture();
											if (succeeded(_roughness_tex))
											{
												roughness_tex = _roughness_tex.get();
											}
										}
										if (mat_normal_tex)
										{
											auto _normal_tex = mat_normal_tex->texture();
											if (succeeded(_normal_tex))
											{
												normal_tex = _normal_tex.get();
											}
										}
										if (mat_metallic_tex)
										{
											auto _metallic_tex = mat_metallic_tex->texture();
											if (succeeded(_metallic_tex))
											{
												metallic_tex = _metallic_tex.get();
											}
										}
										if (mat_emissive_tex)
										{
											auto _emissive_tex = mat_emissive_tex->texture();
											if (succeeded(_emissive_tex))
											{
												emissive_tex = _emissive_tex.get();
											}
										}
									}
								}

								auto skybox = scene_renderer->skybox_texture().lock();
								P<IResource> skybox_tex;
								if (skybox)
								{
									auto tex = skybox->texture();
									if (succeeded(tex))
									{
										skybox_tex = tex.get();
									}
								}
								if (skybox_tex)
								{
									sky_tex = skybox_tex;
								}

								auto vs = Renderer::device()->new_view_set(m_type->m_depth_pass_slayout, ViewSetDesc(1, 8, 0, 1)).get();
								vs->set_cbv(0, m_camera_cb, ConstantBufferViewDesc(0, (u32)align_upper(sizeof(CameraCB), cb_align)));
								vs->set_srv(0, m_model_matrices, &ShaderResourceViewDesc::as_buffer(EResourceFormat::unknown, i, 1, sizeof(Float4x4) * 2, false));
								if (light_ts.empty())
								{
									// Adds one fake light.
									vs->set_srv(1, m_lighting_params, &ShaderResourceViewDesc::as_buffer(EResourceFormat::unknown, 0, 1, sizeof(LightingParams)));
								}
								else
								{
									vs->set_srv(1, m_lighting_params, &ShaderResourceViewDesc::as_buffer(EResourceFormat::unknown, 0, (u32)light_ts.size(), sizeof(LightingParams)));
								}
								// Set material texture: base_color(t2), roughness(t3), normal(t4), metallic(t5), emissive(t6).
								vs->set_srv(2, base_color_tex);
								vs->set_srv(3, roughness_tex);
								vs->set_srv(4, normal_tex);
								vs->set_srv(5, metallic_tex);
								vs->set_srv(6, emissive_tex);
								vs->set_srv(7, sky_tex);
								vs->set_sampler(0, SamplerDesc(EFilter::min_mag_mip_linear, ETextureAddressMode::warp, ETextureAddressMode::warp, ETextureAddressMode::warp));
								m_scene_cmdbuf->set_graphic_view_set(vs);
								m_scene_cmdbuf->attach_graphic_object(vs);
								m_scene_cmdbuf->draw_indexed(mesh->piece_count_indices(j), mesh->piece_first_index_offset(j), 0);
							}
						}
						m_scene_cmdbuf->end_render_pass();
					}

					// Bloom Pass.
					// The Bloom Pass is added to the lighting texture before it is tone-mapped.
					// The original light texture will also be scaled down a little bit.
					{
						
					}

					// Tone mapping pass.
					{
						// First Lum Pass.
						{
							m_scene_cmdbuf->set_compute_shader_input_layout(m_type->m_first_lum_pass_slayout);
							m_scene_cmdbuf->set_pipeline_state(m_type->m_first_lum_pass_pso);
							Float4 offsets[16];
							{
								// How much texels are covered by one sample pixel?
								f32 TexelsCoveredPerSampleW = scene_sz.x / 1024.0f;
								f32 TexelsCoveredPerSampleH = scene_sz.y / 1024.0f;
								// The offset of one texel in uv-space.
								f32 NormalizedWidthPerTexel = 1.0f / scene_sz.x;
								f32 NormalizedHeightPerTexel = 1.0f / scene_sz.y;
								for (i32 i = 0; i < 4; i++)
								{
									for (i32 j = 0; j < 4; j++)
									{
										offsets[4 * i + j] = Float4{
											NormalizedWidthPerTexel * TexelsCoveredPerSampleW / 8.0f * (float)(2 * j - 3) ,
											NormalizedHeightPerTexel * TexelsCoveredPerSampleH / 8.0f * (float)(2 * i - 3) ,0.0f,0.0f };
									}
								}
							}
							auto mapped = m_tone_mapping_offset->map_subresource(0, false, 1, 0).get();
							memcpy(mapped, offsets, sizeof(Float4) * 16);
							m_tone_mapping_offset->unmap_subresource(0, true);
							m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(scene_renderer->lighting_buffer(), EResourceState::shader_resource_non_pixel, 0));
							m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(scene_renderer->tone_mapping_buffer(0), EResourceState::unordered_access, 0));
							m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(m_tone_mapping_offset, EResourceState::vertex_and_constant_buffer, 0));
							auto vs = Renderer::device()->new_view_set(m_type->m_first_lum_pass_slayout, ViewSetDesc(1, 1, 1, 1)).get();
							vs->set_srv(0, scene_renderer->lighting_buffer());
							vs->set_uav(0, scene_renderer->tone_mapping_buffer(0));
							vs->set_sampler(0, SamplerDesc(EFilter::min_mag_mip_linear, ETextureAddressMode::warp, ETextureAddressMode::warp, ETextureAddressMode::warp));
							vs->set_cbv(0, m_tone_mapping_offset, ConstantBufferViewDesc(0, (u32)align_upper(sizeof(Float4) * 16, cb_align)));
							m_scene_cmdbuf->set_compute_view_set(vs);
							m_scene_cmdbuf->attach_graphic_object(vs);
							m_scene_cmdbuf->dispatch(128, 128, 1);
						}

						// Lum passes.
						{
							m_scene_cmdbuf->set_compute_shader_input_layout(m_type->m_lum_pass_slayout);
							m_scene_cmdbuf->set_pipeline_state(m_type->m_lum_pass_pso);
							for (u32 i = 0; i < 10; ++i)
							{
								m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(scene_renderer->tone_mapping_buffer(i), EResourceState::shader_resource_non_pixel, 0));
								m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(scene_renderer->tone_mapping_buffer(i + 1), EResourceState::unordered_access, 0));
								auto vs = Renderer::device()->new_view_set(m_type->m_first_lum_pass_slayout, ViewSetDesc(0, 1, 1, 0)).get();
								vs->set_srv(0, scene_renderer->tone_mapping_buffer(i));
								vs->set_uav(0, scene_renderer->tone_mapping_buffer(i + 1));
								m_scene_cmdbuf->set_compute_view_set(vs);
								m_scene_cmdbuf->attach_graphic_object(vs);
								u32 dispatches = max(64 >> i, 1);
								m_scene_cmdbuf->dispatch(dispatches, dispatches, 1);
							}
						}
						
						// Tone Mapping Pass.
						{
							auto mapped = m_tone_mapping_params->map_subresource(0, false, 1, 0).get();
							ToneMappingParams params;
							params.exposure = scene_renderer->exposure();
							memcpy(mapped, &params, sizeof(ToneMappingParams));
							m_tone_mapping_params->unmap_subresource(0, true);
							m_scene_cmdbuf->set_compute_shader_input_layout(m_type->m_tone_mapping_pass_slayout);
							m_scene_cmdbuf->set_pipeline_state(m_type->m_tone_mapping_pass_pso);
							m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(scene_renderer->tone_mapping_buffer(10), EResourceState::shader_resource_non_pixel));
							m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(scene_renderer->lighting_buffer(), EResourceState::shader_resource_non_pixel));
							m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(scene_renderer->screen_buffer(), EResourceState::unordered_access));
							m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(m_tone_mapping_params, EResourceState::vertex_and_constant_buffer));
							auto vs = Renderer::device()->new_view_set(m_type->m_first_lum_pass_slayout, ViewSetDesc(1, 2, 1, 0)).get();
							vs->set_cbv(0, m_tone_mapping_params, ConstantBufferViewDesc(0, (u32)align_upper(sizeof(ToneMappingParams), cb_align)));
							vs->set_srv(0, scene_renderer->lighting_buffer());
							vs->set_srv(1, scene_renderer->tone_mapping_buffer(10));
							vs->set_uav(0, scene_renderer->screen_buffer());
							m_scene_cmdbuf->set_compute_view_set(vs);
							m_scene_cmdbuf->attach_graphic_object(vs);
							m_scene_cmdbuf->dispatch(max<u32>((u32)scene_sz.x / 8, 1), max<u32>((u32)scene_sz.y / 8, 1), 1);
						}
					}

				}
			}

			// Draw Overlays.
			{
				// Draw Grid.
				using namespace Gfx;
				m_grid_fb = Renderer::device()->new_frame_buffer(m_type->m_grid_rp, 1, &render_tex, nullptr, nullptr, nullptr).get();
				m_scene_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(render_tex, EResourceState::render_target));
				m_scene_cmdbuf->begin_render_pass(m_type->m_grid_rp, m_grid_fb, 0, nullptr, 0.0f, 0xFF);
				m_scene_cmdbuf->set_graphic_shader_input_layout(m_type->m_grid_slayout);
				m_scene_cmdbuf->set_pipeline_state(m_type->m_grid_pso);
				m_scene_cmdbuf->set_vertex_buffers(0, 1, &VertexBufferViewDesc(m_type->m_grid_vb.get(), 0, sizeof(Float4) * 44, sizeof(Float4)));
				m_scene_cmdbuf->set_primitive_topology(EPrimitiveTopology::line_list);
				m_scene_cmdbuf->set_graphic_view_set(m_grid_views);
				m_scene_cmdbuf->set_viewport(Viewport(0.0f, 0.0f, (f32)render_desc.width, (f32)render_desc.height, 0.0f, 1.0f));
				m_scene_cmdbuf->set_scissor_rect(RectI(0, 0, (i32)render_desc.width, (i32)render_desc.height));
				m_scene_cmdbuf->draw(44, 0);
			}

			luassert_always(succeeded(m_scene_cmdbuf->submit()));

			ctx->image(render_tex, scene_sz);

			// Draw GUI Overlays.
			{
				// Draw gizmo.
				if (m_current_select_entity < entities.size())
				{
					auto& e = entities[m_current_select_entity];
					if (e != camera_entity.get())
					{
						auto _et = e->get_component(Name("Transform"));
						if (succeeded(_et))
						{
							P<E3D::ITransform> et = _et.get();
							Float4x4 world_mat = et->local_to_world_matrix();
							bool edited = false;
							ctx->gizmo(world_mat, m_camera_cb_data.world_to_view, m_camera_cb_data.view_to_proj,
								RectF(scene_pos.x, scene_pos.y, scene_sz.x, scene_sz.y), m_gizmo_op, m_gizmo_mode, 0.0f, true, false, nullptr, nullptr, &edited);
							if (edited)
							{
								et->set_local_to_world_matrix(world_mat);
							}
						}
					}
				}
				

				// Draw scene debug info.
				auto backup_pos = ctx->get_cursor_pos();
				ctx->set_cursor_screen_pos(scene_pos);

				ctx->text("FPS: %f", get_fps_averaged());


				ctx->set_cursor_pos(backup_pos);
			}

			if (ctx->is_mouse_clicked(Input::EMouseKey::rb) && in_rect(ImGui::io_get_mouse_pos(ctx->get_io()), scene_pos, scene_pos + scene_sz))
			{
				m_navigating = true;
				m_scene_click_pos = Input::get_device(Name("mouse")).get().as<Input::IMouse>()->pos();
			}

			if (m_navigating && ctx->is_mouse_released(Input::EMouseKey::rb))
			{
				m_navigating = false;
			}

			if (m_navigating)
			{
				auto mouse = Input::get_device(Name("mouse")).get().as<Input::IMouse>();
				auto mouse_pos = mouse->pos();
				auto mouse_delta = mouse_pos - m_scene_click_pos;
				auto _ = mouse->set_pos(m_scene_click_pos.x, m_scene_click_pos.y);
				// Rotate camera based on mouse delta.
				auto rot = transform->local_rotation();
				auto rot_mat = Float4x4::make_rotation_quaternion(rot);

				// Key control.
				auto left = rot_mat.left();
				auto forward = rot_mat.forward();
				auto up = rot_mat.up();
				auto io = ctx->get_io();

				f32 camera_speed = m_camera_speed;
				if (ImGui::io_is_key_down(io, Input::EKeyCode::shift))
				{
					camera_speed *= 2.0f;
				}
				
				if (ImGui::io_is_key_down(io, Input::EKeyCode::w))
				{
					transform->set_local_position(transform->local_position() + forward * 0.1f * camera_speed);
				}
				if (ImGui::io_is_key_down(io, Input::EKeyCode::a))
				{
					transform->set_local_position(transform->local_position() + left * 0.1f * camera_speed);
				}
				if (ImGui::io_is_key_down(io, Input::EKeyCode::s))
				{
					transform->set_local_position(transform->local_position() - forward * 0.1f * camera_speed);
				}
				if (ImGui::io_is_key_down(io, Input::EKeyCode::d))
				{
					transform->set_local_position(transform->local_position() - left * 0.1f * camera_speed);
				}
				if (ImGui::io_is_key_down(io, Input::EKeyCode::q))
				{
					transform->set_local_position(transform->local_position() - up * 0.1f * camera_speed);
				}
				if (ImGui::io_is_key_down(io, Input::EKeyCode::e))
				{
					transform->set_local_position(transform->local_position() + up * 0.1f * camera_speed);
				}
				auto eular = rot_mat.euler_angles();
				eular += {deg_to_rad((f32)mouse_delta.y / 10.0f), deg_to_rad((f32)mouse_delta.x / 10.0f), 0.0f};
				eular.x = clamp(eular.x, deg_to_rad(-85.0f), deg_to_rad(85.0f));
				transform->set_local_rotation(Quaternion::from_roll_pitch_yaw(eular.x, eular.y, eular.z));

			}


			m_scene_cmdbuf->wait();
			luassert_always(succeeded(m_scene_cmdbuf->reset()));

			ctx->end_child();
		}

		void SceneEditor::draw_components_grid(ImGui::IContext* ctx, Vector<Scene::IEntity*>& entities)
		{
			// Draw component property grid.

			ctx->text("Components Grid");

			ctx->push_style_var(ImGui::EStyle::child_rounding, 5.0f);
			ctx->begin_child("Components Grid", Float2(0.0f, 0.0f), true);

			auto current_entity = m_current_entity.lock();

			if (m_current_select_entity < entities.size())
			{
				auto current_entity = m_current_entity.lock();
				if (current_entity.get() != entities[m_current_select_entity])
				{
					// Refresh panel.
					m_component_editors.clear();
					current_entity = entities[m_current_select_entity];
					m_current_entity = current_entity;
				}

				auto components = current_entity->components();
				if (components.empty())
				{
					ctx->text("No components");
				}
				else
				{
					for (auto& i : components)
					{
						if (ctx->collapsing_header(i->type_object()->type_name().c_str(), ImGui::ETreeNodeFlag::default_open))
						{
							bool has_editor = false;
							for (auto& j : m_component_editors)
							{
								if (j->type_name() == i->type_object()->type_name())
								{
									has_editor = true;
									j->on_render(ctx);
									break;
								}
							}
							// Check editor instance.
							if (!has_editor)
							{
								auto iter = m_type->m_component_editor_types.find(i->type_object()->type_name());
								if (iter != m_type->m_component_editor_types.end())
								{
									m_component_editors.push_back(iter->second->new_editor(i));
									has_editor = true;
								}
							}
							if (!has_editor)
							{
								ctx->text_wrapped("The component editor type is not registered. The component cannot be edited.");
							}
							ctx->push_id(i);

							if (ctx->button("Remove"))
							{
								for (auto iter = m_component_editors.begin(); iter != m_component_editors.end(); ++iter)
								{
									if ((*iter)->type_name() == i->type_object()->type_name())
									{
										m_component_editors.erase(iter);
										break;
									}
								}
								auto _ = current_entity->remove_component(i->type_object()->type_name());
							}

							ctx->pop_id();
						}
					}
				}

				const char* new_comp_popup = "NewCompPopup";

				if (ctx->button("New Component"))
				{
					ctx->open_popup(new_comp_popup);
				}

				if (ctx->begin_popup(new_comp_popup))
				{
					auto component_types = Scene::component_types();

					for (auto& i : component_types)
					{
						auto name = i->type_name();
						auto exists = current_entity->get_component(name);
						if (failed(exists))
						{
							// Show enabled.
							if (ctx->selectable(name.c_str()))
							{
								auto res = current_entity->add_component(name);
								if (failed(res))
								{
									debug_printf(get_errmsg(res.errcode()));
								}
								ctx->close_current_popup();
							}
						}
						else
						{
							// Show disabled.
							ctx->selectable(name.c_str(), false, ImGui::ESelectableFlag::disabled);
						}
					}
					ctx->end_popup();
				}
			}
			else
			{
				ctx->text("Select an entity to see components.");
			}

			ctx->end_child();
			ctx->pop_style_var();
		}
		void SceneEditor::on_render(ImGui::IContext* ctx)
		{
			char title[32];
			sprintf_s(title, "Scene Editor###%d", (u32)(usize)this);
			ctx->set_next_window_size(Float2(1000, 500), ImGui::ECondition::first_use_ever);
			ctx->begin(title, &m_open, ImGui::EWindowFlag::no_collapse | ImGui::EWindowFlag::menu_bar);
			auto s = m_scene.lock();
			if (!s)
			{
				ctx->text("Asset Unloaded");
				ctx->end();
				return;
			}
			if (s->meta()->state() == Asset::EAssetState::unloaded)
			{
				s->meta()->load(Asset::EAssetLoadFlag::none);
			}
			if (s->meta()->state() != Asset::EAssetState::loaded)
			{
				ctx->text("Scene Loading");
				ctx->end();
				return;
			}

			if (ctx->begin_menu_bar())
			{
				if (ctx->begin_menu("File"))
				{
					if (ctx->menu_item("Save"))
					{
						lutry
						{
							lulet(r1, s->meta()->save_meta(Asset::EAssetSaveFormat::ascii));
							lulet(r2, s->meta()->save_data(Asset::EAssetSaveFormat::ascii));
							r1->wait();
							r2->wait();
							if (r1->result())
							{
								luthrow(r1->result());
							}
							if (r2->result())
							{
								luthrow(r2->result());
							}
						}
						lucatch
						{
							auto _ = Gfx::message_box(get_errmsg(lures), "Failed to save scene", Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
						}
					}
					ctx->end_menu();
				}
				ctx->end_menu_bar();
			}

			ctx->columns(3, nullptr, true);

			auto entities = s->entities();

			draw_entity_list(ctx, entities);

			draw_scene_components_grid(ctx);

			ctx->next_column();

			draw_scene(ctx, entities);

			ctx->next_column();

			draw_components_grid(ctx, entities);

			ctx->next_column();

			ctx->end();
		}
		RV SceneEditorType::init()
		{
			//! Initialize Grid data.
			Float4 grids[44];
			for (i32 i = -5; i <= 5; ++i) // 0 - 21
			{
				grids[(i + 5) * 2] = Float4((f32)i, 0.0f, 5.0f, 1.0f);
				grids[(i + 5) * 2 + 1] = Float4((f32)i, 0.0f, -5.0f, 1.0f);
			}
			for (i32 i = -5; i <= 5; ++i) // 22 - 43
			{
				grids[(i + 5) * 2 + 22] = Float4(-5.0f, 0.0f, (f32)i, 1.0f);
				grids[(i + 5) * 2 + 23] = Float4(5.0f, 0.0f, (f32)i, 1.0f);
			}

			lutry
			{
				using namespace Gfx;
				{
					luset(m_grid_vb, Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::gpu_local, EResourceUsageFlag::vertex_buffer, sizeof(grids))));
					luset(m_default_base_color, Renderer::device()->new_resource(
						ResourceDesc::tex2d(EResourceFormat::rgba8_unorm, EAccessType::gpu_local, EResourceUsageFlag::shader_resource, 1, 1, 1, 1)));
					luset(m_default_roughness, Renderer::device()->new_resource(
						ResourceDesc::tex2d(EResourceFormat::r8_unorm, EAccessType::gpu_local, EResourceUsageFlag::shader_resource, 1, 1, 1, 1)));
					luset(m_default_normal, Renderer::device()->new_resource(
						ResourceDesc::tex2d(EResourceFormat::rgba8_unorm, EAccessType::gpu_local, EResourceUsageFlag::shader_resource, 1, 1, 1, 1)));
					luset(m_default_metallic, Renderer::device()->new_resource(
						ResourceDesc::tex2d(EResourceFormat::r8_unorm, EAccessType::gpu_local, EResourceUsageFlag::shader_resource, 1, 1, 1, 1)));
					luset(m_default_emissive, Renderer::device()->new_resource(
						ResourceDesc::tex2d(EResourceFormat::rgba8_unorm, EAccessType::gpu_local, EResourceUsageFlag::shader_resource, 1, 1, 1, 1)));

					ShaderInputGroupDesc group(EShaderInputGroupType::cbv, 0, 1, EShaderVisibility::vertex);
					luset(m_grid_slayout, Renderer::device()->new_shader_input_layout(ShaderInputLayoutDesc(&group, 1,
						EShaderInputLayoutFlag::allow_input_assembler_input_layout |
						EShaderInputLayoutFlag::deny_domain_shader_access |
						EShaderInputLayoutFlag::deny_geometry_shader_access |
						EShaderInputLayoutFlag::deny_hull_shader_access)));
					luset(m_grid_rp, Renderer::device()->new_render_pass(RenderPassDesc({ AttachmentDesc(EResourceFormat::rgba8_unorm,
						EAttachmentLoadOp::dont_care, EAttachmentStoreOp::store) }, EResourceFormat::unknown,
						EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, 1, false)));
					static const char* vertexShader =
						"cbuffer vertexBuffer : register(b0) \
						{\
							float4x4 world_to_view; \
							float4x4 view_to_proj; \
							float4x4 world_to_proj; \
							float4x4 view_to_world; \
						};\
						struct VS_INPUT\
						{\
						  float4 pos : POSITION;\
						};\
						\
						struct PS_INPUT\
						{\
						  float4 pos : SV_POSITION;\
						};\
						\
						PS_INPUT main(VS_INPUT input)\
						{\
						  PS_INPUT output;\
						  output.pos = mul(world_to_proj, input.pos);\
						  return output;\
						}";
					lulet(vs_blob, Gfx::compile_shader(Gfx::ShaderCompileDesc(vertexShader, strlen(vertexShader),
						"GridVS", "main", Gfx::EShaderSourceType::hlsl, Gfx::EShaderTargetType::dx_bytecode,
						Gfx::EShaderType::vertex, Gfx::EShaderModel::sm_5_0, Gfx::EShaderOptimizationLevel::full,
						Gfx::EShaderCompileFlag::none)));
					static const char* pixelShader =
						"struct PS_INPUT\
						{\
						  float4 pos : SV_POSITION;\
						};\
						\
						float4 main(PS_INPUT input) : SV_Target\
						{\
						  return float4(1.0f, 1.0f, 1.0f, 1.0f); \
						}";
					lulet(ps_blob, Gfx::compile_shader(Gfx::ShaderCompileDesc(pixelShader, strlen(pixelShader),
						"GridPS", "main", Gfx::EShaderSourceType::hlsl, Gfx::EShaderTargetType::dx_bytecode,
						Gfx::EShaderType::pixel, Gfx::EShaderModel::sm_5_0, Gfx::EShaderOptimizationLevel::full,
						Gfx::EShaderCompileFlag::none)));

					GraphicsPipelineStateDesc ps_desc;
					memzero(&ps_desc, sizeof(GraphicsPipelineStateDesc));
					ps_desc.primitive_topology_type = EPrimitiveTopologyType::line;
					ps_desc.sample_mask = u32_max;
					ps_desc.sample_quality = 0;
					ps_desc.blend_state = BlendDesc(false, false, { RenderTargetBlendDesc(true, false, EBlendFactor::src_alpha,
						EBlendFactor::inv_src_alpha, EBlendOp::add, EBlendFactor::inv_src_alpha, EBlendFactor::zero, EBlendOp::add, ELogicOp::noop, EColorWriteMask::all) });
					ps_desc.rasterizer_state = RasterizerDesc(EFillMode::wireframe, ECullMode::none, 0, 0.0f, 0.0f, 1, false, true, false, true, false);
					ps_desc.depth_stencil_state = DepthStencilDesc(false, false, EComparisonFunc::always, false, 0x00, 0x00, DepthStencilOpDesc(Default()), DepthStencilOpDesc(Default()));
					ps_desc.ib_strip_cut_value = EIndexBufferStripCutValue::disabled;
					InputElementDesc input_elements[1] = {
						InputElementDesc("POSITION", 0, EResourceFormat::rgba32_float),
					};
					ps_desc.input_layout.input_elements = input_elements;
					ps_desc.input_layout.num_elements = 1;
					ps_desc.vs.code = vs_blob.data();
					ps_desc.vs.length = vs_blob.size();
					ps_desc.ps.code = ps_blob.data();
					ps_desc.ps.length = ps_blob.size();
					luset(m_grid_pso, Renderer::device()->new_graphics_pipeline_state(m_grid_slayout, m_grid_rp, ps_desc));
				}

				// Upload grid vertex data.
				lulet(grid_vb, Renderer::device()->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::none, sizeof(grids))));
				lulet(mapped, grid_vb->map_subresource(0, false, 1, 0));
				memcpy(mapped, grids, sizeof(grids));
				grid_vb->unmap_subresource(0, true);
				lulet(cmdbuf, Renderer::main_copy_queue()->new_command_buffer());
				cmdbuf->copy_resource(m_grid_vb, grid_vb);

				// Upload default texture data.
				lulet(base_color_data, Renderer::device()->new_resource(
					ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::none, 4)));
				lulet(roughness_data, Renderer::device()->new_resource(
					ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::none, 1)));
				lulet(normal_data, Renderer::device()->new_resource(
					ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::none, 4)));
				lulet(metallic_data, Renderer::device()->new_resource(
					ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::none, 1)));
				lulet(emissive_data, Renderer::device()->new_resource(
					ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::none, 4)));
				lulet(base_color_data_mapped, base_color_data->map_subresource(0, false, 1, 0));
				lulet(roughness_data_mapped, roughness_data->map_subresource(0, false, 1, 0));
				lulet(normal_data_mapped, normal_data->map_subresource(0, false, 1, 0));
				lulet(metallic_data_mapped, metallic_data->map_subresource(0, false, 1, 0));
				lulet(emissive_data_mapped, emissive_data->map_subresource(0, false, 1, 0));
				((u8*)base_color_data_mapped)[0] = 255;
				((u8*)base_color_data_mapped)[1] = 255;
				((u8*)base_color_data_mapped)[2] = 255;
				((u8*)base_color_data_mapped)[3] = 255;
				((u8*)roughness_data_mapped)[0] = 127;
				((u8*)normal_data_mapped)[0] = 127;
				((u8*)normal_data_mapped)[1] = 127;
				((u8*)normal_data_mapped)[2] = 255;
				((u8*)normal_data_mapped)[3] = 255;
				((u8*)metallic_data_mapped)[0] = 0;
				((u8*)emissive_data_mapped)[0] = 0;
				((u8*)emissive_data_mapped)[1] = 0;
				((u8*)emissive_data_mapped)[2] = 0;
				((u8*)emissive_data_mapped)[3] = 0;
				base_color_data->unmap_subresource(0, true);
				roughness_data->unmap_subresource(0, true);
				normal_data->unmap_subresource(0, true);
				metallic_data->unmap_subresource(0, true);
				emissive_data->unmap_subresource(0, true);

				u32 tex_data_pitch_align;
				Renderer::device()->check_feature_support(EDeviceFeature::texture_data_pitch_alignment, &tex_data_pitch_align);

				cmdbuf->copy_texture_region(TextureCopyLocation::as_subresource_index(m_default_base_color, 0), 0, 0, 0,
					TextureCopyLocation::as_placed_foorprint(base_color_data, 0, EResourceFormat::rgba8_unorm, 1, 1, 1, tex_data_pitch_align));
				cmdbuf->copy_texture_region(TextureCopyLocation::as_subresource_index(m_default_roughness, 0), 0, 0, 0,
					TextureCopyLocation::as_placed_foorprint(roughness_data, 0, EResourceFormat::r8_unorm, 1, 1, 1, tex_data_pitch_align));
				cmdbuf->copy_texture_region(TextureCopyLocation::as_subresource_index(m_default_normal, 0), 0, 0, 0,
					TextureCopyLocation::as_placed_foorprint(normal_data, 0, EResourceFormat::rgba8_unorm, 1, 1, 1, tex_data_pitch_align));
				cmdbuf->copy_texture_region(TextureCopyLocation::as_subresource_index(m_default_metallic, 0), 0, 0, 0,
					TextureCopyLocation::as_placed_foorprint(metallic_data, 0, EResourceFormat::r8_unorm, 1, 1, 1, tex_data_pitch_align));
				cmdbuf->copy_texture_region(TextureCopyLocation::as_subresource_index(m_default_emissive, 0), 0, 0, 0,
					TextureCopyLocation::as_placed_foorprint(emissive_data, 0, EResourceFormat::rgba8_unorm, 1, 1, 1, tex_data_pitch_align));


				luexp(cmdbuf->submit());

				static const char* vertexShaderCommon =
					"cbuffer vertexBuffer : register(b0) \
						{\
							float4x4 world_to_view; \
							float4x4 view_to_proj; \
							float4x4 world_to_proj; \
							float4x4 view_to_world; \
						};\
						struct MeshBuffer	\
						{\
							float4x4 model_to_world;	\
							float4x4 world_to_model;	\
						};\
						StructuredBuffer<MeshBuffer> g_MeshBuffer : register(t0);\
						struct VS_INPUT\
						{\
							float3 position : POSITION;	\
							float3 normal : NORMAL;	\
							float3 tangent : TANGENT;	\
							float2 texcoord : TEXCOORD;	\
							float4 color : COLOR;	\
						};\
						\
						struct PS_INPUT\
						{\
							float4 position : SV_POSITION;	\
							float3 normal : NORMAL;	\
							float3 tangent : TANGENT;	\
							float2 texcoord : TEXCOORD;	\
							float4 color : COLOR;	\
							float3 world_position : POSITION;	\
						};\
						\
						PS_INPUT main(VS_INPUT input)\
						{\
							PS_INPUT output;\
							output.world_position = mul(g_MeshBuffer[0].model_to_world, float4(input.position, 1.0f)).xyz;\
							output.position = mul(world_to_proj, float4(output.world_position, 1.0f));\
							output.normal = mul(float4(input.normal, 0.0f), g_MeshBuffer[0].world_to_model).xyz;\
							output.tangent = mul(float4(input.tangent, 0.0f), g_MeshBuffer[0].world_to_model).xyz;\
							output.texcoord = input.texcoord;	\
							output.color = input.color;	\
							return output;\
						}";

				lulet(vs_blob, Gfx::compile_shader(Gfx::ShaderCompileDesc(vertexShaderCommon, strlen(vertexShaderCommon),
					"MeshDebugVS", "main", Gfx::EShaderSourceType::hlsl, Gfx::EShaderTargetType::dx_bytecode,
					Gfx::EShaderType::vertex, Gfx::EShaderModel::sm_5_0, Gfx::EShaderOptimizationLevel::full,
					Gfx::EShaderCompileFlag::none)));

				InputElementDesc input_elements_common[] = {
						InputElementDesc("POSITION", 0, EResourceFormat::rgb32_float),
						InputElementDesc("NORMAL", 0, EResourceFormat::rgb32_float),
						InputElementDesc("TANGENT", 0, EResourceFormat::rgb32_float),
						InputElementDesc("TEXCOORD", 0, EResourceFormat::rg32_float),
						InputElementDesc("COLOR", 0, EResourceFormat::rgba32_float),
				};

				InputLayoutDesc input_layout_common(5, input_elements_common);

				// Create Resources for debug mesh renderer.
				{
					ShaderInputGroupDesc groups[] = {
						ShaderInputGroupDesc(EShaderInputGroupType::cbv, 0, 1, EShaderVisibility::vertex),
						ShaderInputGroupDesc(EShaderInputGroupType::srv, 0, 1, EShaderVisibility::vertex),
					};
					luset(m_debug_mesh_renderer_slayout, Renderer::device()->new_shader_input_layout(ShaderInputLayoutDesc(groups, 2,
						EShaderInputLayoutFlag::allow_input_assembler_input_layout |
						EShaderInputLayoutFlag::deny_domain_shader_access |
						EShaderInputLayoutFlag::deny_geometry_shader_access |
						EShaderInputLayoutFlag::deny_hull_shader_access)));
					luset(m_debug_mesh_renderer_rp, Renderer::device()->new_render_pass(RenderPassDesc({ AttachmentDesc(EResourceFormat::rgba8_unorm,
						EAttachmentLoadOp::dont_care, EAttachmentStoreOp::store) }, EResourceFormat::unknown,
						EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, 1, false)));
					static const char* pixelShader =
						"struct PS_INPUT\
						{\
							float4 position : SV_POSITION;	\
							float3 normal : NORMAL;	\
							float3 tangent : TANGENT;	\
							float2 texcoord : TEXCOORD;	\
							float4 color : COLOR;	\
							float3 world_position : POSITION;	\
						}; \
						\
						float4 main(PS_INPUT input) : SV_Target\
						{\
						  return float4(1.0f, 1.0f, 1.0f, 1.0f); \
						}";
					lulet(ps_blob, Gfx::compile_shader(Gfx::ShaderCompileDesc(pixelShader, strlen(pixelShader),
						"MeshDebugPS", "main", Gfx::EShaderSourceType::hlsl, Gfx::EShaderTargetType::dx_bytecode,
						Gfx::EShaderType::pixel, Gfx::EShaderModel::sm_5_0, Gfx::EShaderOptimizationLevel::full,
						Gfx::EShaderCompileFlag::none)));

					GraphicsPipelineStateDesc ps_desc;
					memzero(&ps_desc, sizeof(GraphicsPipelineStateDesc));
					ps_desc.primitive_topology_type = EPrimitiveTopologyType::triangle;
					ps_desc.sample_mask = u32_max;
					ps_desc.sample_quality = 0;
					ps_desc.blend_state = BlendDesc(false, false, { RenderTargetBlendDesc(true, false, EBlendFactor::src_alpha,
						EBlendFactor::inv_src_alpha, EBlendOp::add, EBlendFactor::inv_src_alpha, EBlendFactor::zero, EBlendOp::add, ELogicOp::noop, EColorWriteMask::all) });
					ps_desc.rasterizer_state = RasterizerDesc(EFillMode::wireframe, ECullMode::none, 0, 0.0f, 0.0f, 0, false, true, false, true, false);
					ps_desc.depth_stencil_state = DepthStencilDesc(false, false, EComparisonFunc::always, false, 0x00, 0x00, DepthStencilOpDesc(Default()), DepthStencilOpDesc(Default()));
					ps_desc.ib_strip_cut_value = EIndexBufferStripCutValue::disabled;
					ps_desc.input_layout = input_layout_common;
					ps_desc.vs.code = vs_blob.data();
					ps_desc.vs.length = vs_blob.size();
					ps_desc.ps.code = ps_blob.data();
					ps_desc.ps.length = ps_blob.size();
					luset(m_debug_mesh_renderer_pso, Renderer::device()->new_graphics_pipeline_state(m_debug_mesh_renderer_slayout, m_debug_mesh_renderer_rp, ps_desc));
				}

				// Depth Pass.
				{
					ShaderInputGroupDesc groups[] = {
						ShaderInputGroupDesc(EShaderInputGroupType::cbv, 0, 1, EShaderVisibility::vertex),
						ShaderInputGroupDesc(EShaderInputGroupType::srv, 0, 1, EShaderVisibility::vertex),
					};
					lulet(slayout, Renderer::device()->new_shader_input_layout(ShaderInputLayoutDesc(groups, 2,
						EShaderInputLayoutFlag::allow_input_assembler_input_layout |
						EShaderInputLayoutFlag::deny_domain_shader_access |
						EShaderInputLayoutFlag::deny_geometry_shader_access |
						EShaderInputLayoutFlag::deny_hull_shader_access |
						EShaderInputLayoutFlag::deny_pixel_shader_access)));
					lulet(rp, Renderer::device()->new_render_pass(RenderPassDesc({}, EResourceFormat::d32_float,
						EAttachmentLoadOp::load, EAttachmentStoreOp::store, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, 1, true)));

					GraphicsPipelineStateDesc ps_desc;
					memzero(&ps_desc, sizeof(GraphicsPipelineStateDesc));
					ps_desc.primitive_topology_type = EPrimitiveTopologyType::triangle;
					ps_desc.sample_mask = u32_max;
					ps_desc.sample_quality = 0;
					ps_desc.blend_state = BlendDesc(false, false, {});
					ps_desc.rasterizer_state = RasterizerDesc(EFillMode::solid, ECullMode::back, 0, 0.0f, 0.0f, 0, false, true, false, false, false);
					ps_desc.depth_stencil_state = DepthStencilDesc(true, true, EComparisonFunc::less , false, 0x00, 0x00, DepthStencilOpDesc(Default()), DepthStencilOpDesc(Default()));
					ps_desc.ib_strip_cut_value = EIndexBufferStripCutValue::disabled;
					ps_desc.input_layout = input_layout_common;
					ps_desc.vs.code = vs_blob.data();
					ps_desc.vs.length = vs_blob.size();
					lulet(pso, Renderer::device()->new_graphics_pipeline_state(slayout, rp, ps_desc));

					m_depth_pass_slayout = slayout;
					m_depth_pass_rp = rp;
					m_depth_pass_pso = pso;
				}

				// Skybox pass.
				{
					ShaderInputGroupDesc groups[] = {
						ShaderInputGroupDesc(EShaderInputGroupType::cbv, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::srv, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::uav, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::sampler, 0, 1, EShaderVisibility::all)
					};

					lulet(slayout, Renderer::device()->new_shader_input_layout(ShaderInputLayoutDesc(groups, 4,
						EShaderInputLayoutFlag::deny_vertex_shader_access |
						EShaderInputLayoutFlag::deny_domain_shader_access |
						EShaderInputLayoutFlag::deny_geometry_shader_access |
						EShaderInputLayoutFlag::deny_hull_shader_access |
						EShaderInputLayoutFlag::deny_pixel_shader_access)));

					lulet(rp, Renderer::device()->new_render_pass(RenderPassDesc({ AttachmentDesc(EResourceFormat::rgba32_float, EAttachmentLoadOp::clear, EAttachmentStoreOp::store) }, EResourceFormat::unknown,
						EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, 1, true)));

					lulet(psf, platform_open_file("SkyboxCS.cso", EFileOpenFlag::read, EFileCreationMode::open_existing));
					auto file_size = psf->size();
					auto cs_blob = Blob((usize)file_size);
					luexp(psf->read(cs_blob.data(), cs_blob.size()));
					psf = nullptr;
					ComputePipelineStateDesc ps_desc;
					ps_desc.cs.code = cs_blob.data();
					ps_desc.cs.length = cs_blob.size();
					lulet(pso, Renderer::device()->new_compute_pipline_state(slayout, ps_desc));
					m_skybox_pass_slayout = slayout;
					m_skybox_pass_rp = rp;
					m_skybox_pass_pso = pso;
				}

				// Lighting Pass.
				{
					ShaderInputGroupDesc groups[] = {
						ShaderInputGroupDesc(EShaderInputGroupType::cbv, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::srv, 0, 8, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::sampler, 0, 1, EShaderVisibility::pixel)
					};
					lulet(slayout, Renderer::device()->new_shader_input_layout(ShaderInputLayoutDesc(groups, 3,
						EShaderInputLayoutFlag::allow_input_assembler_input_layout |
						EShaderInputLayoutFlag::deny_domain_shader_access |
						EShaderInputLayoutFlag::deny_geometry_shader_access |
						EShaderInputLayoutFlag::deny_hull_shader_access)));
					lulet(rp, Renderer::device()->new_render_pass(RenderPassDesc({AttachmentDesc(EResourceFormat::rgba32_float, EAttachmentLoadOp::load, EAttachmentStoreOp::store)}, EResourceFormat::d32_float,
						EAttachmentLoadOp::load, EAttachmentStoreOp::store, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, 1, true)));

					
					lulet(psf, platform_open_file("LightingPassPixel.cso", EFileOpenFlag::read, EFileCreationMode::open_existing));
					auto file_size = psf->size();
					auto ps_blob = Blob((usize)file_size);
					luexp(psf->read(ps_blob.data(), ps_blob.size()));
					psf = nullptr;

					GraphicsPipelineStateDesc ps_desc;
					memzero(&ps_desc, sizeof(GraphicsPipelineStateDesc));
					ps_desc.primitive_topology_type = EPrimitiveTopologyType::triangle;
					ps_desc.sample_mask = u32_max;
					ps_desc.sample_quality = 0;
					ps_desc.blend_state = BlendDesc(false, false, { RenderTargetBlendDesc(false, false, EBlendFactor::src_alpha,
						EBlendFactor::inv_src_alpha, EBlendOp::add, EBlendFactor::inv_src_alpha, EBlendFactor::zero, EBlendOp::add, ELogicOp::noop, EColorWriteMask::all) });
					ps_desc.rasterizer_state = RasterizerDesc(EFillMode::solid, ECullMode::back, 0, 0.0f, 0.0f, 0, false, true, false, false, false);
					ps_desc.depth_stencil_state = DepthStencilDesc(true, true, EComparisonFunc::less_equal, false, 0x00, 0x00, DepthStencilOpDesc(Default()), DepthStencilOpDesc(Default()));
					ps_desc.ib_strip_cut_value = EIndexBufferStripCutValue::disabled;
					ps_desc.input_layout = input_layout_common;
					ps_desc.vs.code = vs_blob.data();
					ps_desc.vs.length = vs_blob.size();
					ps_desc.ps.code = ps_blob.data();
					ps_desc.ps.length = ps_blob.size();
					lulet(pso, Renderer::device()->new_graphics_pipeline_state(slayout, rp, ps_desc));

					m_lighting_pass_slayout = slayout;
					m_lighting_pass_rp = rp;
					m_lighting_pass_pso = pso;
				}

				//First Lum Pass.
				{
					ShaderInputGroupDesc groups[] = {
						ShaderInputGroupDesc(EShaderInputGroupType::cbv, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::srv, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::uav, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::sampler, 0, 1, EShaderVisibility::all)
					};
					lulet(slayout, Renderer::device()->new_shader_input_layout(ShaderInputLayoutDesc(groups, 4,
						EShaderInputLayoutFlag::deny_vertex_shader_access |
						EShaderInputLayoutFlag::deny_domain_shader_access |
						EShaderInputLayoutFlag::deny_geometry_shader_access |
						EShaderInputLayoutFlag::deny_hull_shader_access |
						EShaderInputLayoutFlag::deny_pixel_shader_access)));

					lulet(psf, platform_open_file("LumFirstCS.cso", EFileOpenFlag::read, EFileCreationMode::open_existing));
					auto file_size = psf->size();
					auto cs_blob = Blob((usize)file_size);
					luexp(psf->read(cs_blob.data(), cs_blob.size()));
					psf = nullptr;
					ComputePipelineStateDesc ps_desc;
					ps_desc.cs.code = cs_blob.data();
					ps_desc.cs.length = cs_blob.size();
					lulet(pso, Renderer::device()->new_compute_pipline_state(slayout, ps_desc));
					m_first_lum_pass_slayout = slayout;
					m_first_lum_pass_pso = pso;
				}

				//Lum Pass.
				{
					ShaderInputGroupDesc groups[] = {
						ShaderInputGroupDesc(EShaderInputGroupType::srv, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::uav, 0, 1, EShaderVisibility::all)
					};
					lulet(slayout, Renderer::device()->new_shader_input_layout(ShaderInputLayoutDesc(groups, 2,
						EShaderInputLayoutFlag::deny_vertex_shader_access |
						EShaderInputLayoutFlag::deny_domain_shader_access |
						EShaderInputLayoutFlag::deny_geometry_shader_access |
						EShaderInputLayoutFlag::deny_hull_shader_access |
						EShaderInputLayoutFlag::deny_pixel_shader_access)));

					lulet(psf, platform_open_file("LumCS.cso", EFileOpenFlag::read, EFileCreationMode::open_existing));
					auto file_size = psf->size();
					auto cs_blob = Blob((usize)file_size);
					luexp(psf->read(cs_blob.data(), cs_blob.size()));
					psf = nullptr;
					ComputePipelineStateDesc ps_desc;
					ps_desc.cs.code = cs_blob.data();
					ps_desc.cs.length = cs_blob.size();
					lulet(pso, Renderer::device()->new_compute_pipline_state(slayout, ps_desc));
					m_lum_pass_slayout = slayout;
					m_lum_pass_pso = pso;
				}

				//Tone Mapping Pass.
				{
					ShaderInputGroupDesc groups[] = {
						ShaderInputGroupDesc(EShaderInputGroupType::cbv, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::srv, 0, 2, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::uav, 0, 1, EShaderVisibility::all)
					};
					lulet(slayout, Renderer::device()->new_shader_input_layout(ShaderInputLayoutDesc(groups, 3,
						EShaderInputLayoutFlag::deny_vertex_shader_access |
						EShaderInputLayoutFlag::deny_domain_shader_access |
						EShaderInputLayoutFlag::deny_geometry_shader_access |
						EShaderInputLayoutFlag::deny_hull_shader_access |
						EShaderInputLayoutFlag::deny_pixel_shader_access)));

					lulet(psf, platform_open_file("ToneMappingCS.cso", EFileOpenFlag::read, EFileCreationMode::open_existing));
					auto file_size = psf->size();
					auto cs_blob = Blob((usize)file_size);
					luexp(psf->read(cs_blob.data(), cs_blob.size()));
					psf = nullptr;
					ComputePipelineStateDesc ps_desc;
					ps_desc.cs.code = cs_blob.data();
					ps_desc.cs.length = cs_blob.size();
					lulet(pso, Renderer::device()->new_compute_pipline_state(slayout, ps_desc));
					m_tone_mapping_pass_slayout = slayout;
					m_tone_mapping_pass_pso = pso;
				}

				cmdbuf->wait();
			}
			lucatchret;
			return RV();
		}
		void SceneEditorType::on_draw_tile(ImGui::IContext* ctx, Asset::IAsset* asset, const RectF& draw_rect)
		{
			auto text_sz = ctx->calc_text_size("Scene");
			Float2 center = Float2((draw_rect.right + draw_rect.left) / 2.0f, (draw_rect.top + draw_rect.bottom) / 2.0f);
			ctx->set_cursor_pos(center - text_sz / 2.0f);
			ctx->text("Scene");
		}
		P<IAssetEditor> SceneEditorType::new_editor(Asset::IAsset* editing_asset)
		{
			auto edit = newobj<SceneEditor>();
			edit->m_type = this;
			edit->m_scene = P<Scene::IScene>(editing_asset);
			edit->init();
			return edit;
		}
		void SceneEditorType::register_component_editor_type(IComponentEditorType* component_type)
		{
			m_component_editor_types.insert(Pair<Name, P<IComponentEditorType>>(component_type->type(), component_type));
		}
		void SceneEditorType::register_scene_component_editor_type(ISceneComponentEditorType* component_type)
		{
			m_scene_component_editor_types.insert(Pair<Name, P<IComponentEditorType>>(component_type->type(), component_type));
		}
		void SceneCreator::on_render(ImGui::IContext* ctx)
		{
			char title[32];
			sprintf_s(title, "Create Scene###%d", (u32)(usize)this);

			ctx->begin(title, &m_open, ImGui::EWindowFlag::no_collapse);

			ctx->input_text("Scene Asset Name", m_asset_name);
			if (!m_asset_name.empty())
			{
				ctx->text("The Scene will be created as: %s%s", m_create_dir.encode().c_str(), m_asset_name.c_str());
				if (ctx->button("Create"))
				{
					lutry
					{
						P<Scene::IScene> s;
						luset(s, Scene::new_scene());
						s->reset();
						while (s->meta()->state() == Asset::EAssetState::loading)
						{
							yield_current_thread();
						}
						// Save the asset.
						auto ass_path = m_create_dir;
						ass_path.push_back(Name(m_asset_name.c_str()));
						ass_path.flags() = (ass_path.flags() & ~EPathFlag::diretory);
						luexp(s->meta()->set_meta_path(ass_path));
						s->meta()->set_data_path(ass_path);
						lulet(r1, s->meta()->save_meta(Asset::EAssetSaveFormat::ascii));
						lulet(r2, s->meta()->save_data(Asset::EAssetSaveFormat::ascii));
						r1->wait();
						r2->wait();
					}
					lucatch
					{
						auto _ = Gfx::message_box(get_errmsg(lures), "Failed to create scene asset",
										Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
					}
				}
			}

			ctx->end();
		}
	}
}