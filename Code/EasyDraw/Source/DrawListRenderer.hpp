// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file DrawListRenderer.hpp
* @author JXMaster
* @date 2019/10/6
*/
#pragma once
#include "EasyDrawHeader.hpp"
#include "EasyDraw.hpp"
#include <Base/TSAssert.hpp>

namespace luna
{
	namespace edraw
	{
		class DrawListRenderer : public IDrawListRenderer
		{
		public:
			lucid("{7275decc-b597-4976-83d3-5b540b583a5a}");
			luiimpl(DrawListRenderer, IDrawListRenderer, IObject);
			lutsassert_lock();

			//! One-time resources.
			P<gfx::IRenderPass> m_rp;
			P<gfx::IShaderInputLayout> m_slayout;
			P<gfx::IShaderInputLayout> m_slayout_no_tex;
			P<gfx::IPipelineState> m_pso;
			P<gfx::IPipelineState> m_pso_no_tex;

			size_t m_vb_size;
			size_t m_ib_size;
			size_t m_cb_size;

			//! All 3 buffers are created in upload heap.
			P<gfx::IResource> m_vb_res;
			P<gfx::IResource> m_ib_res;
			P<gfx::IResource> m_cb_res;

			size_t m_vb_count;
			size_t m_ib_count;
			size_t m_cb_count;

			struct DrawCallRange
			{
				P<IDrawList> m_list;
				uint32 m_first;
				uint32 m_count;
			};

			Vector<DrawCallRange> m_dcs;

			DrawListRenderer(IAllocator* alloc) :
				luibind(alloc),
				m_dcs(alloc),
				m_vb_size(0),
				m_ib_size(0),
				m_cb_size(0),
				m_vb_count(0),
				m_ib_count(0),
				m_cb_count(0) {}

			RV init();

			virtual gfx::IGraphicDevice* get_device() override
			{
				return m_rp->get_device();
			}
			virtual gfx::IRenderPass* get_render_pass() override
			{
				return m_rp;
			}
			virtual void add_draw_list(IDrawList* draw_list) override
			{
				lutsassert();
				luassert_usr(draw_list);
				DrawCallRange r;
				r.m_list = draw_list;
				r.m_first = 0;
				r.m_count = draw_list->count_draw_calls();
				m_dcs.push_back(move(r));
			}
			virtual void add_draw_calls(IDrawList* draw_list, uint32 first, uint32 num_draw_calls) override
			{
				lutsassert();
				luassert_usr(draw_list);
				DrawCallRange r;
				r.m_list = draw_list;
				r.m_first = first;
				r.m_count = num_draw_calls;
				m_dcs.push_back(move(r));
			}
			virtual RV render(gfx::ICommandBuffer* target_cmd_buffer, gfx::IResource* rt, uint32 width, uint32 height) override;
		};
	}
}