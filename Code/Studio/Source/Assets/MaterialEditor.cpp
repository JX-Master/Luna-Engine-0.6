// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MaterialEditor.cpp
* @author JXMaster
* @date 2020/5/29
*/
#include "MaterialEditor.hpp"

namespace Luna
{
	namespace editor
	{
		void MaterialEditor::on_render(ImGui::IContext* ctx)
		{
			char title[32];
			sprintf_s(title, "Material Editor###%d", (u32)(usize)this);
			ctx->begin(title, &m_open, ImGui::EWindowFlag::no_collapse | ImGui::EWindowFlag::menu_bar);

			auto mat = m_material.lock();
			if (!mat || (mat->meta()->state() != Asset::EAssetState::loaded))
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
								lulet(r1, mat->meta()->save_meta(Asset::EAssetSaveFormat::ascii));
								lulet(r2, mat->meta()->save_data(Asset::EAssetSaveFormat::ascii));
								r1->wait();
								r2->wait();
								if (r1->result() != 0)
								{
									luthrow(r1->result());
								}
								if (r2->result() != 0)
								{
									luthrow(r2->result());
								}
							}
							lucatch
							{
								auto _ = Gfx::message_box(get_errmsg(lures), "Failed to save asset", Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
							}
						}
						ctx->end_menu();
					}
					ctx->end_menu_bar();
				}

				const char* types[] = { "Lit", "Unlit" };
				E3D::EMeterialType t = mat->material_type();
				ctx->combo("Material Type", (i32*)&t, types, 2);
				mat->set_material_type(t);

				if (t == E3D::EMeterialType::lit)
				{
					Guid base_color = mat->base_color().guid();
					input_asset_entry(ctx, "Base Color", m_base_color_name, base_color, Name("Texture"));
					mat->set_base_color(base_color);

					auto base_color_ass = Asset::fetch_asset(base_color);
					if (succeeded(base_color_ass))
					{
						auto tex = base_color_ass.get().as<Texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}

					Guid roughness = mat->roughness().guid();
					input_asset_entry(ctx, "Roughness", m_roughness_name, roughness, Name("Texture"));
					mat->set_roughness(roughness);

					auto roughness_ass = Asset::fetch_asset(roughness);
					if (succeeded(roughness_ass))
					{
						auto tex = roughness_ass.get().as<Texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}

					Guid normal = mat->normal().guid();
					input_asset_entry(ctx, "Normal", m_normal_name, normal, Name("Texture"));
					mat->set_normal(normal);

					auto normal_ass = Asset::fetch_asset(normal);
					if (succeeded(normal_ass))
					{
						auto tex = normal_ass.get().as<Texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}

					Guid metallic = mat->metallic().guid();
					input_asset_entry(ctx, "Metallic", m_metallic_name, metallic, Name("Texture"));
					mat->set_metallic(metallic);

					auto metallic_ass = Asset::fetch_asset(metallic);
					if (succeeded(metallic_ass))
					{
						auto tex = metallic_ass.get().as<Texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}

					Guid emissive = mat->emissive().guid();
					input_asset_entry(ctx, "Emissive", m_emissive_name, emissive, Name("Texture"));
					mat->set_emissive(emissive);

					auto emissive_ass = Asset::fetch_asset(emissive);
					if (succeeded(emissive_ass))
					{
						auto tex = emissive_ass.get().as<Texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}
				}
				else
				{
					Guid emissive = mat->emissive().guid();
					input_asset_entry(ctx, "Emissive", m_emissive_name, emissive, Name("Texture"));
					mat->set_emissive(emissive);

					auto emissive_ass = Asset::fetch_asset(emissive);
					if (succeeded(emissive_ass))
					{
						auto tex = emissive_ass.get().as<Texture::ITexture>()->texture();
						if (succeeded(tex))
						{
							ctx->image(tex.get(), Float2(100.0f, 100.0f));
						}
					}
				}
			}

			ctx->end();
		}
		void MaterialEditorType::on_draw_tile(ImGui::IContext* ctx, Asset::IAsset* asset, const RectF& draw_rect)
		{
			auto text_sz = ctx->calc_text_size("Material");
			Float2 center = Float2((draw_rect.right + draw_rect.left) / 2.0f, (draw_rect.top + draw_rect.bottom) / 2.0f);
			ctx->set_cursor_pos(center - text_sz / 2.0f);
			ctx->text("Material");
		}
		P<IAssetEditor> MaterialEditorType::new_editor(Asset::IAsset* editing_asset)
		{
			auto edt = newobj<MaterialEditor>();
			edt->m_material = editing_asset;
			return edt;
		}
		void MaterialCreator::on_render(ImGui::IContext* ctx)
		{
			char title[32];
			sprintf_s(title, "Create Material###%d", (u32)(usize)this);

			ctx->begin(title, &m_open, ImGui::EWindowFlag::no_collapse);

			ctx->input_text("Material Asset Name", m_asset_name);
			if (!m_asset_name.empty())
			{
				ctx->text("The Material will be created as: %s%s", m_create_dir.encode().c_str(), m_asset_name.c_str());
				if (ctx->button("Create"))
				{
					lutry
					{
						P<E3D::IMaterial> s;
						luset(s, E3D::new_material());
						s->meta()->load(Asset::EAssetLoadFlag::procedural);
						while (s->meta()->state() == Asset::EAssetState::loading)
						{
							yield_current_thread();
						}
						// Save the asset.
						auto ass_path = m_create_dir;
						ass_path.push_back(m_asset_name.c_str());
						ass_path.flags() = ass_path.flags() & ~EPathFlag::diretory;
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