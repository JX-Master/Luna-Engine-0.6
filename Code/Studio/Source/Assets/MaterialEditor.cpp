// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MaterialEditor.cpp
* @author JXMaster
* @date 2020/5/29
*/
#include "MaterialEditor.hpp"

namespace luna
{
	namespace editor
	{
		void MaterialEditor::on_render(imgui::IContext* ctx)
		{
			char title[32];
			sprintf_s(title, "Material Editor###%d", (uint32)this);
			ctx->begin(title, &m_open, imgui::EWindowFlag::no_collapse | imgui::EWindowFlag::menu_bar);

			auto mat = m_material.lock();
			if (!mat || (mat->meta()->state() != asset::EAssetState::loaded))
			{
				ctx->text("Material Asset is not loaded.");
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
								lulet(r1, mat->meta()->save_meta(asset::EAssetSaveFormat::ascii));
								lulet(r2, mat->meta()->save_data(asset::EAssetSaveFormat::ascii));
								r1->wait();
								r2->wait();
								if (failed(r1->result()))
								{
									luthrow(r1->result());
								}
								if (failed(r2->result()))
								{
									luthrow(r2->result());
								}
							}
							lucatch
							{
								auto _ = gfx::message_box(explain(lures), "Failed to save asset", gfx::EMessageBoxType::ok, gfx::EMessageBoxIcon::error);
							}
						}
						ctx->end_menu();
					}
					ctx->end_menu_bar();
				}

				const char* types[] = { "Lit", "Unlit" };
				e3d::EMeterialType t = mat->material_type();
				ctx->combo("Material Type", (int32*)&t, types, 2);
				mat->set_material_type(t);

				if (t == e3d::EMeterialType::lit)
				{
					Guid base_color = mat->base_color().guid();
					input_asset_entry(ctx, "Base Color", m_base_color_name, base_color, intern_name("Texture"));
					mat->set_base_color(base_color);

					auto base_color_ass = asset::fetch_asset(base_color);
					if (succeeded(base_color_ass))
					{
						auto tex = base_color_ass.get().as<texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}

					Guid roughness = mat->roughness().guid();
					input_asset_entry(ctx, "Roughness", m_roughness_name, roughness, intern_name("Texture"));
					mat->set_roughness(roughness);

					auto roughness_ass = asset::fetch_asset(roughness);
					if (succeeded(roughness_ass))
					{
						auto tex = roughness_ass.get().as<texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}

					Guid normal = mat->normal().guid();
					input_asset_entry(ctx, "Normal", m_normal_name, normal, intern_name("Texture"));
					mat->set_normal(normal);

					auto normal_ass = asset::fetch_asset(normal);
					if (succeeded(normal_ass))
					{
						auto tex = normal_ass.get().as<texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}

					Guid metallic = mat->metallic().guid();
					input_asset_entry(ctx, "Metallic", m_metallic_name, metallic, intern_name("Texture"));
					mat->set_metallic(metallic);

					auto metallic_ass = asset::fetch_asset(metallic);
					if (succeeded(metallic_ass))
					{
						auto tex = metallic_ass.get().as<texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}

					Guid emissive = mat->emissive().guid();
					input_asset_entry(ctx, "Emissive", m_emissive_name, emissive, intern_name("Texture"));
					mat->set_emissive(emissive);

					auto emissive_ass = asset::fetch_asset(emissive);
					if (succeeded(emissive_ass))
					{
						auto tex = emissive_ass.get().as<texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}
				}
				else
				{
					Guid emissive = mat->emissive().guid();
					input_asset_entry(ctx, "Emissive", m_emissive_name, emissive, intern_name("Texture"));
					mat->set_emissive(emissive);

					auto emissive_ass = asset::fetch_asset(emissive);
					if (succeeded(emissive_ass))
					{
						auto tex = emissive_ass.get().as<texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}
				}
			}

			ctx->end();
		}
		void MaterialEditorType::on_draw_tile(imgui::IContext* ctx, asset::IAsset* asset, const RectF& draw_rect)
		{
			auto text_sz = ctx->calc_text_size("Material");
			Float2 center = Float2((draw_rect.right + draw_rect.left) / 2.0f, (draw_rect.top + draw_rect.bottom) / 2.0f);
			ctx->set_cursor_pos(center - text_sz / 2.0f);
			ctx->text("Material");
		}
		P<IAssetEditor> MaterialEditorType::new_editor(asset::IAsset* editing_asset)
		{
			auto edt = box_ptr(new_obj<MaterialEditor>(get_module_allocator()));
			edt->m_material = editing_asset;
			return edt;
		}
		void MaterialCreator::on_render(imgui::IContext* ctx)
		{
			char title[32];
			sprintf_s(title, "Create Material###%d", (uint32)this);

			ctx->begin(title, &m_open, imgui::EWindowFlag::no_collapse);

			ctx->input_text("Material Asset Name", m_asset_name);
			if (!m_asset_name->empty())
			{
				ctx->text("The Material will be created as: %s%s", m_create_dir->encode()->c_str(), m_asset_name->c_str());
				if (ctx->button("Create"))
				{
					lutry
					{
						P<e3d::IMaterial> s;
						luset(s, e3d::new_material());
						s->meta()->load(asset::EAssetLoadFlag::procedural);
						while (s->meta()->state() == asset::EAssetState::loading)
						{
							yield_current_thread();
						}
						// Save the asset.
						auto ass_path = new_path();
						ass_path->assign(m_create_dir);
						ass_path->push_back(intern_name(m_asset_name->c_str()));
						ass_path->set_flags(ass_path->flags() & ~EPathFlag::diretory);
						luexp(s->meta()->set_meta_path(ass_path));
						s->meta()->set_data_path(ass_path);
						lulet(r1, s->meta()->save_meta(asset::EAssetSaveFormat::ascii));
						lulet(r2, s->meta()->save_data(asset::EAssetSaveFormat::ascii));
						r1->wait();
						r2->wait();
					}
					lucatch
					{
						auto _ = gfx::message_box(explain(lures), "Failed to create asset",
										gfx::EMessageBoxType::ok, gfx::EMessageBoxIcon::error);
					}
				}
				
			}

			ctx->end();
		}
	}
}