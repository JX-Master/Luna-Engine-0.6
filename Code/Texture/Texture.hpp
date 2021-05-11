// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Texture.hpp
* @author JXMaster
* @date 2020/5/8
*/
#pragma once
#include <Asset/Asset.hpp>
#include "ITexture.hpp"
#include <Image/Image.hpp>

#ifndef LUNA_TEXTURE_API
#define LUNA_TEXTURE_API
#endif

namespace Luna
{
	namespace Texture
	{
		//! Creates a new texture object with no data loaded. This is the same as 
		//! `asset::new_asset` with type "Texture".
		LUNA_TEXTURE_API RP<ITexture> new_texture();
	}
}