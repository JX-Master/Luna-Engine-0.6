// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RectPack.hpp
* @author JXMaster
* @date 2019/10/9
*/
#pragma once
#include "IRectPackContext.hpp"

#ifndef LUNA_RECT_PACK_API
#define LUNA_RECT_PACK_API
#endif

namespace luna
{
	namespace rpack
	{
		struct IRectPackContext;

		LUNA_RECT_PACK_API void init();

		//! Creates a new packer context.
		//! @param[in] width The width of the rect.
		//! @param[in] height The height of the rect.
		LUNA_RECT_PACK_API P<IRectPackContext> new_context(uint32 width, uint32 height);
	}
}