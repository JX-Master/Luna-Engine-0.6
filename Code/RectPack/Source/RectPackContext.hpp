// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RectPackContext.hpp
* @author JXMaster
* @date 2019/10/9
*/
#pragma once
#include "RectPackHeader.hpp"
#include <Runtime/Vector.hpp>
#include <Stb/stb_rect_pack.h>
#include <Core/Interface.hpp>

namespace Luna
{
	namespace RectPack
	{
		class RectPackContext : public IRectPackContext
		{
		public:
			lucid("{279df815-dfe5-402f-b7c2-c02b11670768}");
			luiimpl(RectPackContext, IRectPackContext, IObject);

			Vector<stbrp_node> m_nodes;
			stbrp_context m_ctx;

			RectPackContext() {}

			virtual void reset(u32 width, u32 height) override;
			virtual RV pack_rects(PackRect* rects, u32 num_rects) override;
		};
	}
}