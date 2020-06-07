// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IMaterial.hpp
* @author JXMaster
* @date 2020/5/25
*/
#pragma once
#include <Asset/Asset.hpp>
#include <Texture/ITexture.hpp>

namespace luna
{
	namespace e3d
	{
		enum class EMeterialType : uint32
		{
			// Opaque Standard PBR: Base Color/Roughness/Normal/Metallic/Emissive
			lit = 0,
			// Opaque Unlit(blackbody) model, emissive only. Add to the final scene buffer directly.
			unlit = 1,
		};

		//! @interface IMaterial
		struct IMaterial : public asset::IAsset
		{
			luiid("{d2ae697d-b3ca-4f7d-82d1-ccfc047be8ce}");

			virtual EMeterialType material_type() = 0;

			virtual void set_material_type(EMeterialType type) = 0;

			virtual asset::PAsset<texture::ITexture> base_color() = 0;

			virtual asset::PAsset<texture::ITexture> roughness() = 0;

			virtual asset::PAsset<texture::ITexture> normal() = 0;

			virtual asset::PAsset<texture::ITexture> metallic() = 0;

			virtual asset::PAsset<texture::ITexture> emissive() = 0;

			virtual void set_base_color(asset::PAsset<texture::ITexture> tex) = 0;

			virtual void set_roughness(asset::PAsset<texture::ITexture> tex) = 0;

			virtual void set_normal(asset::PAsset<texture::ITexture> tex) = 0;

			virtual void set_metallic(asset::PAsset<texture::ITexture> tex) = 0;

			virtual void set_emissive(asset::PAsset<texture::ITexture> tex) = 0;
		};
	}
}