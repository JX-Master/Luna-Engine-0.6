// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RectPackContext.cpp
* @author JXMaster
* @date 2019/10/9
*/
#define STB_RECT_PACK_IMPLEMENTATION
#include "RectPackContext.hpp"

namespace Luna
{
	namespace RectPack
	{
		void RectPackContext::reset(u32 width, u32 height)
		{
			m_nodes.resize(width);
			stbrp_init_target(&m_ctx, width, height, m_nodes.data(), width);
			stbrp_setup_allow_out_of_mem(&m_ctx, 0);
		}
		RV RectPackContext::pack_rects(PackRect* rects, u32 num_rects)
		{
			stbrp_rect* rs = (stbrp_rect*)memalloc(sizeof(stbrp_rect) * num_rects);
			if (!rs)
			{
				return BasicError::bad_memory_alloc();
			}
			for (u32 i = 0; i < num_rects; ++i)
			{
				rs[i].w = rects[i].width;
				rs[i].h = rects[i].height;
			}
			int s = stbrp_pack_rects(&m_ctx, rs, num_rects);
			for (u32 i = 0; i < num_rects; ++i)
			{
				rects[i].x = rs[i].x;
				rects[i].y = rs[i].y;
				rects[i].packed = rs[i].was_packed ? true : false;
			}
			memfree(rs);
			return s ? RV() : BasicError::overflow();
		}
	}
}