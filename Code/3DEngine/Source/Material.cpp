// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Material.cpp
* @author JXMaster
* @date 2020/5/25
*/
#include "Material.hpp"

namespace luna
{
	namespace e3d
	{
		Unconstructed<MaterialType> g_material_type;

		void Material::set_base_color(asset::PAsset<texture::ITexture> tex)
		{
			asset::notify_asset_change(m_meta, m_base_color.guid(), tex.guid());
			m_base_color = tex;
		}
		void Material::set_roughness(asset::PAsset<texture::ITexture> tex)
		{
			asset::notify_asset_change(m_meta, m_roughness.guid(), tex.guid());
			m_roughness = tex;
		}
		void Material::set_normal(asset::PAsset<texture::ITexture> tex)
		{
			asset::notify_asset_change(m_meta, m_normal.guid(), tex.guid());
			m_normal = tex;
		}
		void Material::set_metallic(asset::PAsset<texture::ITexture> tex)
		{
			asset::notify_asset_change(m_meta, m_metallic.guid(), tex.guid());
			m_metallic = tex;
		}
		void Material::set_emissive(asset::PAsset<texture::ITexture> tex)
		{
			asset::notify_asset_change(m_meta, m_emissive.guid(), tex.guid());
			m_emissive = tex;
		}

		RV MaterialType::on_load_data(asset::IAsset* target_asset, IVariant* data, IVariant* params)
		{
			P<Material> mat = target_asset;
			lutry
			{
				lulet(type, data->field(0, intern_name("material_type")));
				lulet(type_name, type->check_name());
				if (type_name.get() == intern_name("lit"))
				{
					mat->m_material_type = EMeterialType::lit;
				}
				else
				{
					mat->m_material_type = EMeterialType::unlit;
				}
				auto base_color = data->field(0, intern_name("base_color"));
				if (succeeded(base_color))
				{
					lulet(base_color_buf, base_color.get()->check_u64_buf());
					mat->m_base_color = Guid(base_color_buf[0], base_color_buf[1]);
				}
				else
				{
					mat->m_base_color = Guid(0, 0);
				}

				auto roughness = data->field(0, intern_name("roughness"));
				if (succeeded(roughness))
				{
					lulet(roughness_buf, roughness.get()->check_u64_buf());
					mat->m_roughness = Guid(roughness_buf[0], roughness_buf[1]);
				}
				else
				{
					mat->m_roughness = Guid(0, 0);
				}

				auto normal = data->field(0, intern_name("normal"));
				if (succeeded(normal))
				{
					lulet(normal_buf, normal.get()->check_u64_buf());
					mat->m_normal = Guid(normal_buf[0], normal_buf[1]);
				}
				else
				{
					mat->m_normal = Guid(0, 0);
				}

				auto metallic = data->field(0, intern_name("metallic"));
				if (succeeded(metallic))
				{
					lulet(metallic_buf, metallic.get()->check_u64_buf());
					mat->m_metallic = Guid(metallic_buf[0], metallic_buf[1]);
				}
				else
				{
					mat->m_metallic = Guid(0, 0);
				}

				auto emissive = data->field(0, intern_name("emissive"));
				if (succeeded(emissive))
				{
					lulet(emissive_buf, emissive.get()->check_u64_buf());
					mat->m_emissive = Guid(emissive_buf[0], emissive_buf[1]);
				}
				else
				{
					mat->m_emissive = Guid(0, 0);
				}
			}
			lucatchret;

			return s_ok;
		}

		RV MaterialType::on_load_procedural_data(asset::IAsset* target_asset, IVariant* params)
		{
			// Resets dependencies.
			P<Material> mat = target_asset;
			mat->meta()->internal_remove_all_dependencies();
			// Resets textures.
			mat->m_material_type = EMeterialType::lit;
			mat->m_base_color = Guid(0, 0);
			mat->m_roughness = Guid(0, 0);
			mat->m_normal = Guid(0, 0);
			mat->m_metallic = Guid(0, 0);
			mat->m_emissive = Guid(0, 0);
			if (!params)
			{
				return s_ok;
			}
			lutry
			{
				lulet(type, params->field(0, intern_name("material_type")));
				lulet(type_name, type->check_name());
				if (type_name.get() == intern_name("lit"))
				{
					mat->m_material_type = EMeterialType::lit;
				}
				else
				{
					mat->m_material_type = EMeterialType::unlit;
				}
				auto base_color = params->field(0, intern_name("base_color"));
				if (succeeded(base_color))
				{
					lulet(base_color_buf, base_color.get()->check_u64_buf());
					mat->set_base_color(Guid(base_color_buf[0], base_color_buf[1]));
				}

				auto roughness = params->field(0, intern_name("roughness"));
				if (succeeded(roughness))
				{
					lulet(roughness_buf, roughness.get()->check_u64_buf());
					mat->set_roughness(Guid(roughness_buf[0], roughness_buf[1]));
				}

				auto normal = params->field(0, intern_name("normal"));
				if (succeeded(normal))
				{
					lulet(normal_buf, normal.get()->check_u64_buf());
					mat->set_normal(Guid(normal_buf[0], normal_buf[1]));
				}

				auto metallic = params->field(0, intern_name("metallic"));
				if (succeeded(metallic))
				{
					lulet(metallic_buf, metallic.get()->check_u64_buf());
					mat->set_metallic(Guid(metallic_buf[0], metallic_buf[1]));
				}

				auto emissive = params->field(0, intern_name("emissive"));
				if (succeeded(emissive))
				{
					lulet(emissive_buf, emissive.get()->check_u64_buf());
					mat->set_emissive(Guid(emissive_buf[0], emissive_buf[1]));
				}
			}
			lucatchret;

			return s_ok;
		}

		void MaterialType::on_unload_data(asset::IAsset* target_asset)
		{
			P<Material> mat = target_asset;
			mat->m_base_color = Guid(0, 0);
			mat->m_roughness = Guid(0, 0);
			mat->m_normal = Guid(0, 0);
			mat->m_metallic = Guid(0, 0);
			mat->m_emissive = Guid(0, 0);
		}

		RP<IVariant> MaterialType::on_save_data(asset::IAsset* target_asset, IVariant* params)
		{
			P<Material> mat = target_asset;
			auto var = new_var(EVariantType::table);
			auto type = new_var(EVariantType::name);
			if (mat->material_type() == EMeterialType::lit)
			{
				type->name() = intern_name("lit");
			}
			else
			{
				type->name() = intern_name("unlit");
			}
			var->set_field(0, intern_name("material_type"), type);

			Guid tex_guid;

			tex_guid = mat->m_base_color.guid();
			if (tex_guid != Guid(0, 0))
			{
				auto tex = new_var1(EVariantType::u64, 2);
				auto buf = tex->u64_buf();
				buf[0] = tex_guid.high;
				buf[1] = tex_guid.low;
				var->set_field(0, intern_name("base_color"), tex);
			}

			tex_guid = mat->m_roughness.guid();
			if (tex_guid != Guid(0, 0))
			{
				auto tex = new_var1(EVariantType::u64, 2);
				auto buf = tex->u64_buf();
				buf[0] = tex_guid.high;
				buf[1] = tex_guid.low;
				var->set_field(0, intern_name("roughness"), tex);
			}

			tex_guid = mat->m_normal.guid();
			if (tex_guid != Guid(0, 0))
			{
				auto tex = new_var1(EVariantType::u64, 2);
				auto buf = tex->u64_buf();
				buf[0] = tex_guid.high;
				buf[1] = tex_guid.low;
				var->set_field(0, intern_name("normal"), tex);
			}

			tex_guid = mat->m_metallic.guid();
			if (tex_guid != Guid(0, 0))
			{
				auto tex = new_var1(EVariantType::u64, 2);
				auto buf = tex->u64_buf();
				buf[0] = tex_guid.high;
				buf[1] = tex_guid.low;
				var->set_field(0, intern_name("metallic"), tex);
			}

			tex_guid = mat->m_emissive.guid();
			if (tex_guid != Guid(0, 0))
			{
				auto tex = new_var1(EVariantType::u64, 2);
				auto buf = tex->u64_buf();
				buf[0] = tex_guid.high;
				buf[1] = tex_guid.low;
				var->set_field(0, intern_name("emissive"), tex);
			}

			return var;
		}

		void MaterialType::on_dependency_replace(asset::IAsset* current_asset, const Guid& before, const Guid& after)
		{
			P<Material> mat = current_asset;
			if (mat->m_base_color.guid() == before)
			{
				mat->m_base_color = after;
			}
			if (mat->m_roughness.guid() == before)
			{
				mat->m_roughness = after;
			}
			if (mat->m_normal.guid() == before)
			{
				mat->m_normal = after;
			}
			if (mat->m_metallic.guid() == before)
			{
				mat->m_metallic = after;
			}
			if (mat->m_emissive.guid() == before)
			{
				mat->m_emissive = after;
			}
		}
	}
}