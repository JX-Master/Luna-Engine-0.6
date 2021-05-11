// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ModelEditor.cpp
* @author JXMaster
* @date 2020/5/27
*/
#include "ModelEditor.hpp"

namespace Luna
{
	namespace editor
	{
		void ModelEditor::on_render(ImGui::IContext* ctx)
		{
			char title[32];
			sprintf_s(title, "Model Editor###%d", (u32)(usize)this);
			ctx->begin(title, &m_open, ImGui::EWindowFlag::no_collapse | ImGui::EWindowFlag::menu_bar);

			auto model = m_model.lock();
			if (!model || (model->meta()->state() != Asset::EAssetState::loaded))
			{
				ctx->text("Model Asset is not loaded.");
			}
			else
			{
				if (ctx->begin_menu_bar())
				{
					if (ctx->begin_menu("File"))
					{
						if (ctx->menu_item("Save"))
						{
							lutry
							{
								lulet(r1, model->meta()->save_meta(Asset::EAssetSaveFormat::ascii));
								lulet(r2, model->meta()->save_data(Asset::EAssetSaveFormat::ascii));
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
								auto _ = Gfx::message_box(get_errmsg(lures), "Failed to save model", Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
							}
						}
						ctx->end_menu();
					}
					ctx->end_menu_bar();
				}

				Guid mesh = model->mesh().guid();
				input_asset_entry(ctx, "Mesh Asset", m_mesh_name, mesh, Name("Mesh"));
				model->set_mesh(mesh);

				auto mesh_ass = Asset::fetch_asset(mesh);
				if (succeeded(mesh_ass))
				{
					ctx->text("This mesh requires %u material(s).", mesh_ass.get().as<E3D::IMesh>()->count_pieces());
				}

				u32 num_mats = (u32)model->count_materials();
				m_mat_names.resize(num_mats);
				i32 remove_index = -1;
				i32 add_index = -1;
				for (u32 i = 0; i < num_mats; ++i)
				{
					Guid mat = model->material(i).guid();
					char mat_name[32];
					sprintf_s(mat_name, "Material slot %u", i);
					input_asset_entry(ctx, mat_name, m_mat_names[i], mat, Name("Material"));
					model->set_material(i, mat);
					ctx->same_line();
					ctx->push_id(i);
					if (ctx->button("Remove current slot"))
					{
						remove_index = i;
					}
					ctx->same_line();
					if (ctx->button("Add before this"))
					{
						add_index = i;
					}
					ctx->pop_id();
				}
				if (remove_index >= 0)
				{
					model->remove_material((usize)remove_index);
				}
				else if (add_index >= 0)
				{
					model->add_material((usize)add_index);
				}
				if (ctx->button("Add a new material slot"))
				{
					model->add_material(model->count_materials());
				}
			}

			ctx->end();
		}
		void ModelEditorType::on_draw_tile(ImGui::IContext* ctx, Asset::IAsset* asset, const RectF& draw_rect)
		{
			auto text_sz = ctx->calc_text_size("Model");
			Float2 center = Float2((draw_rect.right + draw_rect.left) / 2.0f, (draw_rect.top + draw_rect.bottom) / 2.0f);
			ctx->set_cursor_pos(center - text_sz / 2.0f);
			ctx->text("Model");
		}
		P<IAssetEditor> ModelEditorType::new_editor(Asset::IAsset* editing_asset)
		{
			auto edt = newobj<ModelEditor>();
			edt->m_model = editing_asset;
			return edt;
		}
		void ModelCreator::on_render(ImGui::IContext* ctx)
		{
			char title[32];
			sprintf_s(title, "Create Model###%d", (u32)(usize)this);

			ctx->begin(title, &m_open, ImGui::EWindowFlag::no_collapse);

			ctx->input_text("Model Asset Name", m_asset_name);
			if (!m_asset_name.empty())
			{
				ctx->text("The Model will be created as: %s%s", m_create_dir.encode().c_str(), m_asset_name.c_str());
				if (ctx->button("Create"))
				{
					lutry
					{
						P<E3D::IModel> s;
						luset(s, E3D::new_model());
						s->meta()->load(Asset::EAssetLoadFlag::procedural);
						while (s->meta()->state() == Asset::EAssetState::loading)
						{
							yield_current_thread();
						}
						// Save the asset.
						auto ass_path = m_create_dir;
						ass_path.push_back(m_asset_name.c_str());
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
						auto _ = Gfx::message_box(get_errmsg(lures), "Failed to create asset",
										Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
					}
				}
			}

			ctx->end();
		}
	}
}