// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Material.hpp
* @author JXMaster
* @date 2020/5/25
*/
#pragma once
#include "3DEngineHeader.hpp"
namespace Luna
{
	namespace E3D
	{
		class Material : public IMaterial
		{
		public:
			lucid("{a3554be6-8866-4c7e-8139-9a28708df995}");
			luiimpl(Material, IMaterial, Asset::IAsset, IObject);

			Asset::PAsset<Texture::ITexture> m_base_color;
			Asset::PAsset<Texture::ITexture> m_roughness;
			Asset::PAsset<Texture::ITexture> m_normal;
			Asset::PAsset<Texture::ITexture> m_metallic;
			Asset::PAsset<Texture::ITexture> m_emissive;

			P<Asset::IAssetMeta> m_meta;

			EMeterialType m_material_type = EMeterialType::lit;

			Material() {}

			virtual Asset::IAssetMeta* meta() override
			{
				return m_meta;
			}

			virtual EMeterialType material_type() override
			{
				return m_material_type;
			}

			virtual void set_material_type(EMeterialType type) override
			{
				m_material_type = type;
			}

			virtual Asset::PAsset<Texture::ITexture> base_color() override
			{
				return m_base_color;
			}

			virtual Asset::PAsset<Texture::ITexture> roughness() override
			{
				return m_roughness;
			}

			virtual Asset::PAsset<Texture::ITexture> normal() override
			{
				return m_normal;
			}

			virtual Asset::PAsset<Texture::ITexture> metallic() override
			{
				return m_metallic;
			}

			virtual Asset::PAsset<Texture::ITexture> emissive() override
			{
				return m_emissive;
			}

			virtual void set_base_color(Asset::PAsset<Texture::ITexture> tex) override;

			virtual void set_roughness(Asset::PAsset<Texture::ITexture> tex) override;

			virtual void set_normal(Asset::PAsset<Texture::ITexture> tex) override;

			virtual void set_metallic(Asset::PAsset<Texture::ITexture> tex) override;

			virtual void set_emissive(Asset::PAsset<Texture::ITexture> tex) override;
		};

		class MaterialType : public Asset::IAssetType
		{
		public:
			lucid("{2658d625-d151-4c43-8a5f-5b168244031c}");
			luiimpl_static(MaterialType, Asset::IAssetType, IObject);

			Name m_type_name;

			MaterialType() :
				m_type_name(Name("Material")) {}

			virtual Name type_name() override
			{
				return m_type_name;
			}
			virtual P<Asset::IAsset> on_new_asset(Asset::IAssetMeta* meta) override
			{
				P<Material> r = newobj<Material>();
				r->m_meta = meta;
				return r;
			}

			virtual RV on_load_data(Asset::IAsset* target_asset, const Variant& data, const Variant& params) override;
			virtual RV on_load_procedural_data(Asset::IAsset* target_asset, const Variant& params) override;
			virtual void on_unload_data(Asset::IAsset* target_asset) override;
			virtual R<Variant> on_save_data(Asset::IAsset* target_asset, const Variant& params) override;
			virtual void on_dependency_data_load(Asset::IAsset* current_asset, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(Asset::IAsset* current_asset, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(Asset::IAsset* current_asset, const Guid& before, const Guid& after) override;
		};

		extern Unconstructed<MaterialType> g_material_type;
	}
}