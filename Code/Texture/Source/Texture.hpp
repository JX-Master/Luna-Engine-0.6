// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Texture.hpp
* @author JXMaster
* @date 2020/5/8
*/
#include "TextureHeader.hpp"

namespace luna
{
	namespace texture
	{
		class Texture : public ITexture
		{
		public:
			lucid("{e2e624a4-19e1-442f-990a-13496c045669}");
			luiimpl(Texture, ITexture, asset::IAsset, IObject);

			P<asset::IAssetMeta> m_meta;
			P<gfx::IResource> m_res;

			Texture(IAllocator* alloc) :
				luibind(alloc) {}

			virtual asset::IAssetMeta* meta() override
			{
				return m_meta;
			}

			virtual RP<gfx::IResource> texture() override
			{
				MutexGuard g(m_meta->mutex());
				if (m_res)
				{
					return m_res;
				}
				return e_bad_calling_time;
			}

			virtual void reset(const gfx::ResourceDesc& desc, SubresourceData* initial_data = nullptr, uint32 num_initial_data = 0) override;
		};

		class TextureAssetType : public asset::IAssetType
		{
		public:
			lucid("{602d8c55-27bd-4190-a399-0e0300adf88d}");
			luiimpl_static(TextureAssetType, asset::IAssetType, IObject);

			P<IName> m_texture_asset_name;

			TextureAssetType()
			{
				m_texture_asset_name = intern_name("Texture");
			}

			virtual IName* type_name() override
			{
				return m_texture_asset_name;
			}
			virtual P<asset::IAsset> on_new_asset(asset::IAssetMeta* meta) override;
			virtual RV on_load_data(asset::IAsset* target_asset, IVariant* data, IVariant* params) override;
			virtual RV on_load_procedural_data(asset::IAsset* target_asset, IVariant* params) override;
			virtual void on_unload_data(asset::IAsset* target_asset) override;
			virtual RP<IVariant> on_save_data(asset::IAsset* target_asset, IVariant* params) override;
			virtual void on_dependency_data_load(asset::IAsset* current_asset, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(asset::IAsset* current_asset, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(asset::IAsset* current_asset, const Guid& before, const Guid& after) override {}
		};

		extern Unconstructed<TextureAssetType> g_texture_asset_type;
	}
}