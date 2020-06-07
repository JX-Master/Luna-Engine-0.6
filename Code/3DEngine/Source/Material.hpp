// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Material.hpp
* @author JXMaster
* @date 2020/5/25
*/
#pragma once
#include "3DEngineHeader.hpp"
namespace luna
{
	namespace e3d
	{
		class Material : public IMaterial
		{
		public:
			lucid("{a3554be6-8866-4c7e-8139-9a28708df995}");
			luiimpl(Material, IMaterial, asset::IAsset, IObject);

			asset::PAsset<texture::ITexture> m_base_color;
			asset::PAsset<texture::ITexture> m_roughness;
			asset::PAsset<texture::ITexture> m_normal;
			asset::PAsset<texture::ITexture> m_metallic;
			asset::PAsset<texture::ITexture> m_emissive;

			P<asset::IAssetMeta> m_meta;

			EMeterialType m_material_type = EMeterialType::lit;

			Material(IAllocator* alloc) :
				luibind(alloc) {}

			virtual asset::IAssetMeta* meta() override
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

			virtual asset::PAsset<texture::ITexture> base_color() override
			{
				return m_base_color;
			}

			virtual asset::PAsset<texture::ITexture> roughness() override
			{
				return m_roughness;
			}

			virtual asset::PAsset<texture::ITexture> normal() override
			{
				return m_normal;
			}

			virtual asset::PAsset<texture::ITexture> metallic() override
			{
				return m_metallic;
			}

			virtual asset::PAsset<texture::ITexture> emissive() override
			{
				return m_emissive;
			}

			virtual void set_base_color(asset::PAsset<texture::ITexture> tex) override;

			virtual void set_roughness(asset::PAsset<texture::ITexture> tex) override;

			virtual void set_normal(asset::PAsset<texture::ITexture> tex) override;

			virtual void set_metallic(asset::PAsset<texture::ITexture> tex) override;

			virtual void set_emissive(asset::PAsset<texture::ITexture> tex) override;
		};

		class MaterialType : public asset::IAssetType
		{
		public:
			lucid("{2658d625-d151-4c43-8a5f-5b168244031c}");
			luiimpl_static(MaterialType, asset::IAssetType, IObject);

			P<IName> m_type_name;

			MaterialType() :
				m_type_name(intern_name("Material")) {}

			virtual IName* type_name() override
			{
				return m_type_name;
			}
			virtual P<asset::IAsset> on_new_asset(asset::IAssetMeta* meta) override
			{
				P<Material> r = box_ptr(new_obj<Material>(get_module_allocator()));
				r->m_meta = meta;
				return r;
			}

			virtual RV on_load_data(asset::IAsset* target_asset, IVariant* data, IVariant* params) override;
			virtual RV on_load_procedural_data(asset::IAsset* target_asset, IVariant* params) override;
			virtual void on_unload_data(asset::IAsset* target_asset) override;
			virtual RP<IVariant> on_save_data(asset::IAsset* target_asset, IVariant* params) override;
			virtual void on_dependency_data_load(asset::IAsset* current_asset, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(asset::IAsset* current_asset, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(asset::IAsset* current_asset, const Guid& before, const Guid& after) override;
		};

		extern Unconstructed<MaterialType> g_material_type;
	}
}