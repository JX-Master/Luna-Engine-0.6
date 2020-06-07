// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ProjectSelector.cpp
* @author JXMaster
* @date 2020/4/20
*/
#include "ProjectSelector.hpp"
#include "StudioHeader.hpp"

namespace luna
{
	//! Creates project file at the specified directory.
	static RP<IPath> create_project_dir(IPath* dir_path, IString* project_name, bool create_dir)
	{
		if (project_name->size() == 0)
		{
			set_err(e_bad_arguments, "Project name is empty.");
			return e_user_failure;
		}
		P<IPath> ret_path;
		lutry
		{
			auto project_path = new_path();
			project_path->assign(dir_path);
			if (create_dir)
			{
				project_path->push_back(intern_name(project_name->c_str()));
				luexp(platform_create_dir(project_path->encode()->c_str()));
			}

			project_path->push_back(intern_name("Data"));
			project_path->set_flags(project_path->flags() | EPathFlag::diretory);
			luexp(platform_create_dir(project_path->encode()->c_str()));
			project_path->pop_back();

			project_path->push_back(intern_name(project_name->c_str()));
			project_path->set_flags(project_path->flags() & ~EPathFlag::diretory);
			project_path->append_extension("lunaproj");

			auto project_var = new_var(EVariantType::table);
			auto mounts_var = new_var(EVariantType::variant);
			project_var->set_field(0, intern_name("mounts"), mounts_var);
			// Only one mount registry exists for the initial file.
			{
				auto mount_point_var = new_var(EVariantType::path);
				auto platform_path_var = new_var(EVariantType::path);
				mount_point_var->path() = new_path("/");
				platform_path_var->path() = new_path("./Data");
				auto registry_var = new_var(EVariantType::table);
				registry_var->set_field(0, intern_name("mount_point"), mount_point_var);
				registry_var->set_field(0, intern_name("platform_path"), platform_path_var);
				mounts_var->var() = registry_var;
			}
			auto encoder = new_text_encoder();
			lulet(f, platform_open_file(project_path->encode()->c_str(), EFileOpenFlag::write, EFileCreationMode::create_always));
			luexp(encoder->encode(project_var, f));
			ret_path = project_path;
		}
		lucatchret;
		return ret_path;
	}

	struct RecentFileRecord
	{
		uint64 m_last_use_time;
		P<IPath> m_path;
	};

	void read_recents(Vector<RecentFileRecord>& recents)
	{
		lutry
		{
			lulet(f, platform_open_file("Recents.la", EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing));
			auto decoder = new_text_decoder();
			lulet(var, decoder->decode(f));
			size_t count = var->length(1);
			lulet(items, var->check_var_buf());
			for (size_t i = 0; i < count; ++i)
			{
				auto item = items[i];
				RecentFileRecord rec;
				lulet(time_field, item->field(0, intern_name("last_use_time")));
				lulet(path_field, item->field(0, intern_name("path")));
				luset(rec.m_last_use_time, time_field->check_u64());
				luset(rec.m_path, path_field->check_path());
				recents.push_back(rec);
			}
		}
		lucatch
		{
			return;
		}
	}

	void write_recents(Vector<RecentFileRecord>& recents, IPath* opened)
	{
		auto iter = recents.begin();
		if (opened)
		{
			bool insert = true;
			while (iter != recents.end())
			{
				if (iter->m_path->equal_to(opened))
				{
					RecentFileRecord rec = *iter;
					rec.m_last_use_time = get_local_timestamp();
					recents.erase(iter);
					iter = recents.begin();
					recents.insert(iter, rec);
					insert = false;
					break;
				}
				++iter;
			}
			if (insert)
			{
				RecentFileRecord rec;
				rec.m_last_use_time = get_local_timestamp();
				rec.m_path = opened;
				recents.insert(recents.begin(), rec);
			}
		}
		lutry
		{
			auto var = new_var1(EVariantType::variant, recents.size());
			auto items = var->var_buf();
			for (size_t i = 0; i < recents.size(); ++i)
			{
				auto item = new_var(EVariantType::table);
				auto time_field = new_var(EVariantType::u64);
				auto path_field = new_var(EVariantType::path);
				time_field->u64() = recents[i].m_last_use_time;
				path_field->path() = recents[i].m_path;
				item->set_field(0, intern_name("last_use_time"), time_field);
				item->set_field(0, intern_name("path"), path_field);
				items[i] = item;
			}
			auto encoder = new_text_encoder();
			lulet(f, platform_open_file("Recents.la", EFileOpenFlag::write | EFileOpenFlag::user_buffering, EFileCreationMode::create_always));
			luexp(encoder->encode(var, f));
		}
		lucatch
		{
			return;
		}
	}

	RP<IPath> select_project()
	{
		P<IPath> path;
		lutry
		{
			lulet(window, gfx::new_window("Luna Studio Project Selector", gfx::window_default_v, gfx::window_default_v, 1000, 500));
			lulet(swap_chain, gfx::new_swap_chain(renderer::main_graphic_queue(), window, gfx::SwapChainDesc(0, 0, gfx::EResourceFormat::rgba8_unorm, 2, true)));
			lulet(cmdbuf, renderer::main_graphic_queue()->new_command_buffer());

			// Create back buffer.
			P<gfx::IResource> back_buffer;
			uint32 w = 0, h = 0;

			// Create ImGui context.
			lulet(ctx, imgui::new_context(renderer::device(), cmdbuf, gfx::EResourceFormat::rgba8_unorm, window->dpi_scale_factor()));
			luexp(ctx->attach_system_window(window));

			lulet(rp, renderer::device()->new_render_pass(gfx::RenderPassDesc({ gfx::AttachmentDesc(gfx::EResourceFormat::rgba8_unorm, gfx::EAttachmentLoadOp::dont_care, gfx::EAttachmentStoreOp::store) },
				gfx::EResourceFormat::unknown, gfx::EAttachmentLoadOp::dont_care, gfx::EAttachmentStoreOp::dont_care, gfx::EAttachmentLoadOp::dont_care, gfx::EAttachmentStoreOp::dont_care, 1, false)));
			P<gfx::IFrameBuffer> back_buffer_fbo;

			auto new_solution_name = new_string_buffer();

			Vector<RecentFileRecord> recents(get_module_allocator());
			read_recents(recents);

			bool create_dir = true;

			while (true)
			{
				new_frame();
				input::update();

				if (window->closed())
				{
					break;
				}

				// Recreate the back buffer if needed.
				auto sz = window->size();
				if (sz.x && sz.y && (!back_buffer || sz.x != w || sz.y != h))
				{
					luexp(swap_chain->resize_buffers(2, sz.x, sz.y, gfx::EResourceFormat::unknown));
					float32 clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
					luset(back_buffer, renderer::device()->new_resource(gfx::ResourceDesc::tex2d(gfx::EResourceFormat::rgba8_unorm, gfx::EAccessType::gpu_local, gfx::EResourceUsageFlag::render_target, sz.x, sz.y, 1, 1),
						&gfx::ClearValue::as_color(gfx::EResourceFormat::rgba8_unorm, clear_color)));
					w = sz.x;
					h = sz.y;
					luset(back_buffer_fbo, renderer::device()->new_frame_buffer(rp, 1, back_buffer.get_address_of(), nullptr, nullptr, nullptr));
				}

				ctx->new_frame();

				ctx->set_next_window_pos(Float2(0.0f, 0.0f));
				ctx->set_next_window_size(Float2((float32)sz.x, (float32)sz.y));
				ctx->begin("Luna Studio Project Selector", nullptr, imgui::EWindowFlag::no_title_bar | imgui::EWindowFlag::no_resize | imgui::EWindowFlag::no_move | imgui::EWindowFlag::no_collapse);

				if (ctx->collapsing_header("New Project", imgui::ETreeNodeFlag::default_open))
				{
					ctx->input_text("Project Name", new_solution_name);
					ctx->checkbox("Create Project Folder", &create_dir);
					if (ctx->button("Create New Project"))
					{
						auto rpath = gfx::open_dir_dialog("Select Project Folder");
						if (succeeded(rpath))
						{
							auto res2 = create_project_dir(rpath.get(), new_solution_name, create_dir);
							if (succeeded(res2))
							{
								path = res2.get();
								break;
							}
							else
							{
								auto _ = gfx::message_box(explain(res2.result()), "Project Creation Failed", gfx::EMessageBoxType::ok, gfx::EMessageBoxIcon::error);
							}
						}
					}
				}

				if (ctx->collapsing_header("Open Existing Project", imgui::ETreeNodeFlag::default_open))
				{
					if (ctx->button("Browse Project File"))
					{
						auto rpath = gfx::open_file_dialog("Luna Project File\0*.lunaproj\0\0", "Select Project File");
						if (succeeded(rpath) && !rpath.get().empty())
						{
							path = rpath.get()[0];
							break;
						}
					}

					
					if(!recents.empty())
					{
						// Show recent files.
						ctx->push_style_var(imgui::EStyle::child_rounding, 5.0f);
						ctx->begin_child("Recent Projects", Float2(0.0f, 0.0f), true);

						ctx->text("Recent Projects");

						ctx->columns(4);

						ctx->text("Path");
						ctx->next_column();
						ctx->text("Last Assess Date");
						ctx->next_column();
						ctx->next_column();
						ctx->next_column();

						ctx->set_column_width(0, ctx->get_window_content_region_width() - 430);
						ctx->set_column_width(1, 250);
						ctx->set_column_width(2, 80);
						ctx->set_column_width(3, 100);

						auto iter = recents.begin();
						while (iter != recents.end())
						{
							DateTime dt;
							timestamp_to_data_time(iter->m_last_use_time, dt);
							ctx->text(iter->m_path->encode()->c_str());
							ctx->next_column();
							ctx->text("%hu/%hu/%hu %02hu:%02hu", dt.year, dt.month, dt.day, dt.hour, dt.minute);
							ctx->next_column();
							ctx->push_id(iter->m_path);
							if (ctx->button("Open"))
							{
								path = iter->m_path;
							}
							ctx->next_column();
							if (ctx->button("Delete"))
							{
								iter = recents.erase(iter);
								write_recents(recents, nullptr);
							}
							else
							{
								++iter;
							}
							ctx->pop_id();
							ctx->next_column();
						}

						ctx->end_child();
						ctx->pop_style_var();

						if (path)
						{
							break;
						}
					}
				}

				ctx->end();

				ctx->end_frame();
				Float4U clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
				cmdbuf->begin_render_pass(rp, back_buffer_fbo, 1, &clear_color, 0.0f, 0);
				cmdbuf->clear_render_target_view(0, clear_color.m, 0, nullptr);
				cmdbuf->end_render_pass();
				luexp(ctx->render(cmdbuf, back_buffer));
				luexp(cmdbuf->submit());
				cmdbuf->wait();
				luexp(cmdbuf->reset());
				luexp(swap_chain->present(back_buffer, 0, 1));
				swap_chain->wait();
			}
			if (!path)
			{
				return e_failure;
			}

			// Write to the recents.
			write_recents(recents, path);
		}
		lucatchret;
		return path;
	}
}