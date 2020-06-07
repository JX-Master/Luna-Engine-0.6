// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RectPack.cpp
* @author JXMaster
* @date 2019/10/9
*/
#include "RectPack.hpp"
#include "RectPackContext.hpp"
namespace luna
{
	namespace rpack
	{
		void deinit()
		{

		}

		void init()
		{
			add_module("RectPack", deinit);
		}

		P<IRectPackContext> new_context(uint32 width, uint32 height)
		{
			P<RectPackContext> ctx = box_ptr(new_obj<RectPackContext>(get_module_allocator()));
			ctx->reset(width, height);
			return ctx;
		}
	}
}