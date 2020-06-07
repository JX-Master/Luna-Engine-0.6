// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RectPackContext.hpp
* @author JXMaster
* @date 2019/10/9
*/
#pragma once
#include "RectPackHeader.hpp"
#include <Base/Vector.hpp>
#include <Stb/stb_rect_pack.h>
#include <Base/Interface.hpp>

namespace luna
{
	namespace rpack
	{
		class RectPackContext : public IRectPackContext
		{
		public:
			lucid("{279df815-dfe5-402f-b7c2-c02b11670768}");
			luiimpl(RectPackContext, IRectPackContext, IObject);

			Vector<stbrp_node> m_nodes;
			stbrp_context m_ctx;

			RectPackContext(IAllocator* alloc) :
				luibind(alloc),
				m_nodes(alloc) {}

			virtual void reset(uint32 width, uint32 height) override;
			virtual RV pack_rects(PackRect* rects, uint32 num_rects) override;
		};
	}
}