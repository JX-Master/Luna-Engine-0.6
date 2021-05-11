// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ProjectSelector.cpp
* @author JXMaster
* @date 2020/4/20
*/
#include "ProjectSelector.hpp"
#include "StudioHeader.hpp"
#include <Runtime/Time.hpp>
#include <Runtime/Debug.hpp>

namespace Luna
{
	//! Creates project file at the specified directory.
	static R<Path> create_project_dir(const Path& dir_path, const String& project_name, bool create_dir)
	{
		if (project_name.size() == 0)
		{
			return custom_error(BasicError::bad_arguments(), u8"Project name is empty.");
		}
		Path ret_path;
		lutry
		{
			auto project_path = Path();
			project_path.assign(dir_path);
			if (create_dir)
			{
				project_path.push_back(Name(project_name.c_str()));
				luexp(platform_create_dir(project_path.encode().c_str()));
			}

			project_path.push_back(Name("Data"));
			project_path.flags() = project_path.flags() | EPathFlag::diretory;
			luexp(platform_create_dir(project_path.encode().c_str()));
			project_path.pop_back();

			project_path.push_back(Name(project_name.c_str()));
			project_path.flags() = project_path.flags() & ~EPathFlag::diretory;
			project_path.append_extension(u8"lunaproj");

			auto project_var = Variant(EVariantType::table);
			auto mounts_var = Variant(EVariantType::variant);
			// Only one mount registry exists for the initial file.
			{
				auto mount_point_var = Variant(EVariantType::path);
				auto platform_path_var = Variant(EVariantType::path);
				mount_point_var.to_path() = Path(u8"/");
				platform_path_var.to_path() = Path(u8"./Data");
				auto registry_var = Variant(EVariantType::table);
				registry_var.set_field(0, u8"mount_point", mount_point_var);
				registry_var.set_field(0, u8"platform_path", platform_path_var);
				mounts_var.to_var() = registry_var;
			}
			project_var.set_field(0, u8"mounts", mounts_var);
			auto encoder = new_text_encoder();
			lulet(f, platform_open_file(project_path.encode().c_str(), EFileOpenFlag::write, EFileCreationMode::create_always));
			luexp(encoder->encode(project_var, f));
			ret_path = project_path;
		}
		lucatchret;
		return ret_path;
	}

	struct RecentFileRecord
	{
		u64 m_last_use_time;
		Path m_path;
	};

	void read_recents(Vector<RecentFileRecord>& recents)
	{
		lutry
		{
			lulet(f, platform_open_file(u8"Recents.la", EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing));
			auto decoder = new_text_decoder();
			lulet(var, decoder->decode(f));
			usize count = var.length(1);
			lulet(items, var.check_var_buf());
			for (usize i = 0; i < count; ++i)
			{
				auto item = items[i];
				RecentFileRecord rec;
				auto& time_field = item.field(0, u8"last_use_time");
				auto& path_field = item.field(0, u8"path");
				luset(rec.m_last_use_time, time_field.check_u64());
				luset(rec.m_path, path_field.check_path());
				recents.push_back(rec);
			}
		}
		lucatch
		{
			return;
		}
	}

	void write_recents(Vector<RecentFileRecord>& recents, const Path& opened)
	{
		auto iter = recents.begin();
		if (!opened.empty())
		{
			bool insert = true;
			while (iter != recents.end())
			{
				if (iter->m_path.equal_to(opened))
				{
					RecentFileRecord rec = *iter;
					rec.m_last_use_time = get_timestamp();
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
				rec.m_last_use_time = get_timestamp();
				rec.m_path = opened;
				recents.insert(recents.begin(), rec);
			}
		}
		lutry
		{
			auto var = Variant(EVariantType::variant, recents.size());
			auto items = var.to_var_buf();
			for (usize i = 0; i < recents.size(); ++i)
			{
				auto item = Variant(EVariantType::table);
				auto time_field = Variant(EVariantType::u64);
				auto path_field = Variant(EVariantType::path);
				time_field.to_u64() = recents[i].m_last_use_time;
				path_field.to_path() = recents[i].m_path;
				item.set_field(0, Name("last_use_time"), time_field);
				item.set_field(0, Name("path"), path_field);
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

	R<Path> select_project()
	{
		Path path;
		lutry
		{
			lulet(window, Gfx::new_window("Luna Studio Project Selector", Gfx::window_default_v, Gfx::window_default_v, 1000, 500));
			lulet(swap_chain, Gfx::new_swap_chain(Renderer::main_graphic_queue(), window, Gfx::SwapChainDesc(0, 0, Gfx::EResourceFormat::rgba8_unorm, 2, true)));
			lulet(cmdbuf, Renderer::main_graphic_queue()->new_command_buffer());

			// Create back buffer.
			P<Gfx::IResource> back_buffer;
			u32 w = 0, h = 0;

			// Create ImGui context.
			lulet(ctx, ImGui::new_context(Renderer::device(), cmdbuf, Gfx::EResourceFormat::rgba8_unorm, window->dpi_scale_factor()));
			luexp(ctx->attach_system_window(window));

			lulet(rp, Renderer::device()->new_render_pass(Gfx::RenderPassDesc({ Gfx::AttachmentDesc(Gfx::EResourceFormat::rgba8_unorm, Gfx::EAttachmentLoadOp::dont_care, Gfx::EAttachmentStoreOp::store) },
				Gfx::EResourceFormat::unknown, Gfx::EAttachmentLoadOp::dont_care, Gfx::EAttachmentStoreOp::dont_care, Gfx::EAttachmentLoadOp::dont_care, Gfx::EAttachmentStoreOp::dont_care, 1, false)));
			P<Gfx::IFrameBuffer> back_buffer_fbo;

			auto new_solution_name = String();

			Vector<RecentFileRecord> recents;
			read_recents(recents);

			bool create_dir = true;

			while (true)
			{
				new_frame();
				Input::update();

				if (window->closed())
				{
					break;
				}

				// Recreate the back buffer if needed.
				auto sz = window->size();
				if (sz.x && sz.y && (!back_buffer || sz.x != w || sz.y != h))
				{
					luexp(swap_chain->resize_buffers(2, sz.x, sz.y, Gfx::EResourceFormat::unknown));
					f32 clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
					luset(back_buffer, Renderer::device()->new_resource(Gfx::ResourceDesc::tex2d(Gfx::EResourceFormat::rgba8_unorm, Gfx::EAccessType::gpu_local, Gfx::EResourceUsageFlag::render_target, sz.x, sz.y, 1, 1),
						&Gfx::ClearValue::as_color(Gfx::EResourceFormat::rgba8_unorm, clear_color)));
					w = sz.x;
					h = sz.y;
					luset(back_buffer_fbo, Renderer::device()->new_frame_buffer(rp, 1, back_buffer.get_address_of(), nullptr, nullptr, nullptr));
				}

				ctx->new_frame();

				ctx->set_next_window_pos(Float2(0.0f, 0.0f));
				ctx->set_next_window_size(Float2((f32)sz.x, (f32)sz.y));
				ctx->begin("Luna Studio Project Selector", nullptr, ImGui::EWindowFlag::no_title_bar | ImGui::EWindowFlag::no_resize | ImGui::EWindowFlag::no_move | ImGui::EWindowFlag::no_collapse);

				if (ctx->collapsing_header("New Project", ImGui::ETreeNodeFlag::default_open))
				{
					ctx->input_text("Project Name", new_solution_name);
					ctx->checkbox("Create Project Folder", &create_dir);
					if (ctx->button("Create New Project"))
					{
						auto rpath = Gfx::open_dir_dialog("Select Project Folder");
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
								auto _ = Gfx::message_box(get_errmsg(res2.errcode()), "Project Creation Failed", Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
							}
						}
					}
				}

				if (ctx->collapsing_header("Open Existing Project", ImGui::ETreeNodeFlag::default_open))
				{
					if (ctx->button("Browse Project File"))
					{
						auto rpath = Gfx::open_file_dialog("Luna Project File\0*.lunaproj\0\0", "Select Project File");
						if (succeeded(rpath) && !rpath.get().empty())
						{
							path = rpath.get()[0];
							break;
						}
					}

					
					if(!recents.empty())
					{
						// Show recent files.
						ctx->push_style_var(ImGui::EStyle::child_rounding, 5.0f);
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
							DateTime dt = timestamp_to_local_datetime(iter->m_last_use_time);
							ctx->text(iter->m_path.encode().c_str());
							ctx->next_column();
							ctx->text("%hu/%hu/%hu %02hu:%02hu", dt.year, dt.month, dt.day, dt.hour, dt.minute);
							ctx->next_column();
							ctx->push_id(&(iter->m_path));
							if (ctx->button("Open"))
							{
								path = iter->m_path;
							}
							ctx->next_column();
							if (ctx->button("Delete"))
							{
								iter = recents.erase(iter);
								write_recents(recents, Path());
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

						if (!path.empty())
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
			if (path.empty())
			{
				return BasicError::failure();
			}

			// Write to the recents.
			write_recents(recents, path);
		}
		lucatchret;
		return path;
	}
}