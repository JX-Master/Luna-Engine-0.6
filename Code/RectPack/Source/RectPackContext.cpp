// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RectPackContext.cpp
* @author JXMaster
* @date 2019/10/9
*/
#define STB_RECT_PACK_IMPLEMENTATION
#include "RectPackContext.hpp"

namespace luna
{
	namespace rpack
	{
		void RectPackContext::reset(uint32 width, uint32 height)
		{
			m_nodes.resize(width);
			stbrp_init_target(&m_ctx, width, height, m_nodes.data(), width);
			stbrp_setup_allow_out_of_mem(&m_ctx, 0);
		}
		RV RectPackContext::pack_rects(PackRect* rects, uint32 num_rects)
		{
			stbrp_rect* rs = (stbrp_rect*)m_rc.m_alloc->allocate(sizeof(stbrp_rect) * num_rects);
			if (!rs)
			{
				return e_bad_memory_alloc;
			}
			for (uint32 i = 0; i < num_rects; ++i)
			{
				rs[i].w = rects[i].width;
				rs[i].h = rects[i].height;
			}
			int s = stbrp_pack_rects(&m_ctx, rs, num_rects);
			for (uint32 i = 0; i < num_rects; ++i)
			{
				rects[i].x = rs[i].x;
				rects[i].y = rs[i].y;
				rects[i].packed = rs[i].was_packed ? true : false;
			}
			m_rc.m_alloc->free(rs);
			return s ? s_ok : e_failure;
		}
	}
}