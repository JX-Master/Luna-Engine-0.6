// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IMaterial.hpp
* @author JXMaster
* @date 2020/5/25
*/
#pragma once
#include <Asset/Asset.hpp>
#include <Texture/ITexture.hpp>

namespace Luna
{
	namespace E3D
	{
		enum class EMeterialType : u32
		{
			// Opaque Standard PBR: Base Color/Roughness/Normal/Metallic/Emissive
			lit = 0,
			// Opaque Unlit(blackbody) model, emissive only. Add to the final scene buffer directly.
			unlit = 1,
		};

		//! @interface IMaterial
		struct IMaterial : public Asset::IAsset
		{
			luiid("{d2ae697d-b3ca-4f7d-82d1-ccfc047be8ce}");

			virtual EMeterialType material_type() = 0;

			virtual void set_material_type(EMeterialType type) = 0;

			virtual Asset::PAsset<Texture::ITexture> base_color() = 0;

			virtual Asset::PAsset<Texture::ITexture> roughness() = 0;

			virtual Asset::PAsset<Texture::ITexture> normal() = 0;

			virtual Asset::PAsset<Texture::ITexture> metallic() = 0;

			virtual Asset::PAsset<Texture::ITexture> emissive() = 0;

			virtual void set_base_color(Asset::PAsset<Texture::ITexture> tex) = 0;

			virtual void set_roughness(Asset::PAsset<Texture::ITexture> tex) = 0;

			virtual void set_normal(Asset::PAsset<Texture::ITexture> tex) = 0;

			virtual void set_metallic(Asset::PAsset<Texture::ITexture> tex) = 0;

			virtual void set_emissive(Asset::PAsset<Texture::ITexture> tex) = 0;
		};
	}
}