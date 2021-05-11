// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Texture.hpp
* @author JXMaster
* @date 2020/5/8
*/
#include "TextureHeader.hpp"

namespace Luna
{
	namespace Texture
	{
		class Texture : public ITexture
		{
		public:
			lucid("{e2e624a4-19e1-442f-990a-13496c045669}");
			luiimpl(Texture, ITexture, Asset::IAsset, IObject);

			P<Asset::IAssetMeta> m_meta;
			P<Gfx::IResource> m_res;

			Texture() {}

			virtual Asset::IAssetMeta* meta() override
			{
				return m_meta;
			}

			virtual RP<Gfx::IResource> texture() override
			{
				MutexGuard g(m_meta->mutex());
				if (m_res)
				{
					return m_res;
				}
				return BasicError::bad_calling_time();
			}

			virtual void reset(const Gfx::ResourceDesc& desc, SubresourceData* initial_data = nullptr, u32 num_initial_data = 0) override;
		};

		class TextureAssetType : public Asset::IAssetType
		{
		public:
			lucid("{602d8c55-27bd-4190-a399-0e0300adf88d}");
			luiimpl_static(TextureAssetType, Asset::IAssetType, IObject);

			Name m_texture_asset_name;

			TextureAssetType()
			{
				m_texture_asset_name = Name("Texture");
			}

			virtual Name type_name() override
			{
				return m_texture_asset_name;
			}
			virtual P<Asset::IAsset> on_new_asset(Asset::IAssetMeta* meta) override;
			virtual RV on_load_data(Asset::IAsset* target_asset, const Variant& data, const Variant& params) override;
			virtual RV on_load_procedural_data(Asset::IAsset* target_asset, const Variant& params) override;
			virtual void on_unload_data(Asset::IAsset* target_asset) override;
			virtual R<Variant> on_save_data(Asset::IAsset* target_asset, const Variant& params) override;
			virtual void on_dependency_data_load(Asset::IAsset* current_asset, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(Asset::IAsset* current_asset, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(Asset::IAsset* current_asset, const Guid& before, const Guid& after) override {}
		};

		extern Unconstructed<TextureAssetType> g_texture_asset_type;
	}
}