// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MainEditor.cpp
* @author JXMaster
* @date 2020/4/27
*/
#include "MainEditor.hpp"

#include "Assets/TextureEditorType.hpp"
#include "Assets/TextureImporter.hpp"
#include "Assets/ObjImporter.hpp"
#include "Assets/SceneEditor.hpp"
#include "Assets/ModelEditor.hpp"
#include "Assets/MaterialEditor.hpp"

#include "ComponentEditors/TransformComponentEditor.hpp"
#include "ComponentEditors/CameraComponentEditor.hpp"
#include "ComponentEditors/LightComponentEditor.hpp"
#include "ComponentEditors/SceneRendererComponentEditor.hpp"
#include "ComponentEditors/ModelRendererComponentEditor.hpp"

namespace Luna
{
	namespace editor
	{
		MainEditor* g_main_editor;

		static RV load_asset_meta(const Path& search_dir)
		{
			lutry
			{
				lulet(iter, open_dir(search_dir));
				while (iter->valid())
				{
					if ((iter->attribute() & EFileAttributeFlag::directory) != EFileAttributeFlag::none)
					{
						// This is a directory.
						if (!strcmp(iter->filename(), ".") || !strcmp(iter->filename(), ".."))
						{
							iter->move_next();
							continue;
						}
						auto subpath = search_dir;
						subpath.push_back(iter->filename());
						luexp(load_asset_meta(subpath));
					}
					else
					{
						// Ends with ".meta.la" or ".meta.lb"
						const char* name = iter->filename();
						usize name_len = strlen(name);
						if (name_len > 8)
						{
							if ((!strcmp(name + name_len - 8, ".meta.la")) || (!strcmp(name + name_len - 8, ".meta.lb")))
							{
								auto asset_name = Name(name, name_len - 8);
								auto asspath = search_dir;
								asspath.push_back(asset_name);
								asspath.flags() = (asspath.flags() & ~EPathFlag::diretory);
								luexp(Asset::load_asset_meta(asspath));
							}
						}
					}
					iter->move_next();
				}
			}
			lucatchret;
			return RV();
		}

		RV MainEditor::init(const Path& project_path)
		{
			lutry
			{
				char title[64];
				auto name = project_path[project_path.size() - 1];

				// Load .lunaproj file.
				lulet(f, platform_open_file(project_path.encode(EPathSeparator::system_preferred).c_str(), EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing));
				auto decoder = new_text_decoder();
				lulet(var, decoder->decode(f));
				auto& mount_field = var.field(0, "mounts");
				lulet(mount_items, mount_field.check_var_buf());
				usize num_mounts = mount_field.length(1);
				for (usize i = 0; i < num_mounts; ++i)
				{
					auto mount_item = mount_items[i];
					auto& mount_point = mount_item.field(0, "mount_point");
					auto& platform_path = mount_item.field(0, "platform_path");
					lulet(mount_point_path, mount_point.check_path());
					lulet(platform_path_path_relative, platform_path.check_path());
					auto platform_path_abs = project_path;
					platform_path_abs.pop_back();
					platform_path_abs.append(platform_path_path_relative.get());
					luexp(mount_platfrom_path(mount_point_path.get(), platform_path_abs));
				}

				// Load all asset metadata.
				luexp(load_asset_meta("/"));

				// Create window and render objects.
				auto name_no_ext = Path(name.c_str());
				name_no_ext.replace_extension(nullptr);
				sprintf_s(title, "%s - Luna Studio", name_no_ext[name_no_ext.size() - 1].c_str());
				luset(m_window, Gfx::new_window(title));
				luset(m_swap_chain, Gfx::new_swap_chain(Renderer::main_graphic_queue(), m_window, Gfx::SwapChainDesc(0, 0, Gfx::EResourceFormat::rgba8_unorm, 2, true)));
				luset(m_cmdbuf, Renderer::main_graphic_queue()->new_command_buffer());

				// Create back buffer.
				P<Gfx::IResource> back_buffer;
				u32 w = 0, h = 0;

				// Create ImGui context.
				luset(m_ctx, ImGui::new_context(Renderer::device(), m_cmdbuf, Gfx::EResourceFormat::rgba8_unorm, m_window->dpi_scale_factor()));
				luexp(m_ctx->attach_system_window(m_window));

				luset(m_render_pass, Renderer::device()->new_render_pass(Gfx::RenderPassDesc({ Gfx::AttachmentDesc(Gfx::EResourceFormat::rgba8_unorm, Gfx::EAttachmentLoadOp::dont_care, Gfx::EAttachmentStoreOp::store) },
					Gfx::EResourceFormat::unknown, Gfx::EAttachmentLoadOp::dont_care, Gfx::EAttachmentStoreOp::dont_care, Gfx::EAttachmentLoadOp::dont_care, Gfx::EAttachmentStoreOp::dont_care, 1, false)));
			
				// Create asset browser instance.
				P<AssetBrowser> browser = newobj<AssetBrowser>();
				browser->m_editor = this;
				//browser->m_index = m_next_asset_browser_index;
				//++m_next_asset_browser_index;
				browser->m_path = "/";
				auto his_path = browser->m_path;
				browser->m_histroy_paths.push_back(his_path);
				m_asset_browser = browser;

				// Register editor types.
				auto texture_type = newobj<TextureEditorType>();
				register_asset_editor_type(texture_type);
				auto scene_type = newobj<SceneEditorType>();
				luexp(scene_type->init());
				register_asset_editor_type(scene_type);
				auto model_editor_type = newobj<ModelEditorType>();
				register_asset_editor_type(model_editor_type);
				auto material_editor_type = newobj<MaterialEditorType>();
				register_asset_editor_type(material_editor_type);

				// Register importer types.
				auto texture_importer_type = newobj<TextureImporterType>();
				luexp(texture_importer_type->init());
				register_asset_importer_type(texture_importer_type);
				auto obj_importer_type = newobj<ObjImporterType>();
				register_asset_importer_type(obj_importer_type);
				auto scene_creator_type = newobj<SceneCreatorType>();
				register_asset_importer_type(scene_creator_type);
				auto model_creator_type = newobj<ModelCreatorType>();
				register_asset_importer_type(model_creator_type);
				auto material_creator_type = newobj<MaterialCreatorType>();
				register_asset_importer_type(material_creator_type);

				// Register component editor types.
				auto transform_component_editor_type = newobj<TransformComponentEditorType>();
				scene_type->register_component_editor_type(transform_component_editor_type);
				auto camera_component_editor_type = newobj<CameraComponentEditorType>();
				scene_type->register_component_editor_type(camera_component_editor_type);
				auto dir_light_component_editor_type = newobj<DirectionalLightComponentEditorType>();
				scene_type->register_component_editor_type(dir_light_component_editor_type);
				auto point_light_component_editor_type = newobj<PointLightComponentEditorType>();
				scene_type->register_component_editor_type(point_light_component_editor_type);
				auto spot_light_component_editor_type = newobj<SpotLightComponentEditorType>();
				scene_type->register_component_editor_type(spot_light_component_editor_type);
				auto model_renderer_component_editor_type = newobj<ModelRendererComponentEditorType>();
				scene_type->register_component_editor_type(model_renderer_component_editor_type);

				auto scene_renderer_component_editor_type = newobj<SceneRendererComponentEditorType>();
				scene_type->register_scene_component_editor_type(scene_renderer_component_editor_type);
			}
			lucatchret;
			return RV();
		}

		RV MainEditor::update()
		{
			new_frame();
			Input::update();

			if (m_window->closed())
			{
				m_exiting = true;
				return RV();
			}

			lutry
			{
				// Recreate the back buffer if needed.
				auto sz = m_window->size();
				if (sz.x && sz.y && (!m_back_buffer || sz.x != m_main_window_width || sz.y != m_main_window_height))
				{
					luexp(m_swap_chain->resize_buffers(2, sz.x, sz.y, Gfx::EResourceFormat::unknown));
					f32 clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
					luset(m_back_buffer, Renderer::device()->new_resource(Gfx::ResourceDesc::tex2d(Gfx::EResourceFormat::rgba8_unorm, Gfx::EAccessType::gpu_local, Gfx::EResourceUsageFlag::render_target, sz.x, sz.y, 1, 1),
						&Gfx::ClearValue::as_color(Gfx::EResourceFormat::rgba8_unorm, clear_color)));
					m_main_window_width = sz.x;
					m_main_window_height = sz.y;
					luset(m_back_buffer_fbo, Renderer::device()->new_frame_buffer(m_render_pass, 1, m_back_buffer.get_address_of(), nullptr, nullptr, nullptr));
				}

				m_ctx->new_frame();

				// Main window GUI code goes here.

				//m_ctx->show_demo_window();

				// Dock space.
				m_ctx->set_next_window_pos(Float2(0.0f, 0.0f));
				m_ctx->set_next_window_size(Float2((f32)m_main_window_width, (f32)m_main_window_height));
				m_ctx->push_style_var(ImGui::EStyle::window_rounding, 0.0f);
				m_ctx->push_style_var(ImGui::EStyle::window_border_size, 0.0f);
				m_ctx->push_style_var(ImGui::EStyle::window_padding, Float2(0.0f, 0.0f));
				m_ctx->begin("DockSpace", nullptr,  ImGui::EWindowFlag::no_collapse | ImGui::EWindowFlag::no_resize |
					ImGui::EWindowFlag::no_move | ImGui::EWindowFlag::no_bring_to_front_on_focus | ImGui::EWindowFlag::no_nav_focus | ImGui::EWindowFlag::no_docking);

				m_ctx->dock_space(m_ctx->get_id("DockSpace Context"));

				m_ctx->end();
				m_ctx->pop_style_var(3);

				// Draw Asset Browser.
				if (m_ctx->begin_main_menu_bar())
				{
					if (m_ctx->begin_menu("Window"))
					{
						
						m_ctx->end_menu();
					}
					m_ctx->end_main_menu_bar();
				}

				m_asset_browser->render();

				// Draw Editors.
				auto iter = m_editors.begin();
				while (iter != m_editors.end())
				{
					if ((*iter)->closed())
					{
						iter = m_editors.erase(iter);
					}
					else
					{
						(*iter)->on_render(m_ctx);
						++iter;
					}
				}

				m_ctx->end_frame();
				Float4U clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
				m_cmdbuf->begin_render_pass(m_render_pass, m_back_buffer_fbo, 1, &clear_color, 0.0f, 0);
				m_cmdbuf->clear_render_target_view(0, clear_color.m, 0, nullptr);
				m_cmdbuf->end_render_pass();
				luexp(m_ctx->render(m_cmdbuf, m_back_buffer));
				luexp(m_cmdbuf->submit());
				m_cmdbuf->wait();
				luexp(m_cmdbuf->reset());
				luexp(m_swap_chain->present(m_back_buffer, 0, 1));
				m_swap_chain->wait();
			}
			lucatchret;
			return RV();
		}

		void run_main_editor(const Path& project_path)
		{
			P<MainEditor> main_editor = newobj<MainEditor>();
			g_main_editor = main_editor;
			if (!main_editor)
			{
				return;
			}
			lutry
			{
				luexp(main_editor->init(project_path));
				while (!main_editor->m_exiting)
				{
					luexp(main_editor->update());
				}
			}
			lucatch
			{
				auto _ = Gfx::message_box(get_errmsg(lures), "Editor Crashed.", Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
				return;
			}
		}
	}
}