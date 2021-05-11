// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RectPack.cpp
* @author JXMaster
* @date 2019/10/9
*/
#include "RectPackHeader.hpp"
#include "RectPackContext.hpp"
#include <Runtime/Module.hpp>
namespace Luna
{
	namespace RectPack
	{
		StaticRegisterModule m(u8"RectPack", "Core", nullptr, nullptr);

		LUNA_RECT_PACK_API P<IRectPackContext> new_context(u32 width, u32 height)
		{
			P<RectPackContext> ctx = newobj<RectPackContext>();
			ctx->reset(width, height);
			return ctx;
		}
	}
}