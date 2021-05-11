// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file DrawListRenderer.hpp
* @author JXMaster
* @date 2019/10/6
*/
#pragma once
#include "EasyDrawHeader.hpp"
#include "EasyDraw.hpp"
#include <Runtime/TSAssert.hpp>

namespace Luna
{
	namespace EasyDraw
	{
		class DrawListRenderer : public IDrawListRenderer
		{
		public:
			lucid("{7275decc-b597-4976-83d3-5b540b583a5a}");
			luiimpl(DrawListRenderer, IDrawListRenderer, IObject);
			lutsassert_lock();

			//! One-time resources.
			P<Gfx::IRenderPass> m_rp;
			P<Gfx::IShaderInputLayout> m_slayout;
			P<Gfx::IShaderInputLayout> m_slayout_no_tex;
			P<Gfx::IPipelineState> m_pso;
			P<Gfx::IPipelineState> m_pso_no_tex;

			usize m_vb_size;
			usize m_ib_size;
			usize m_cb_size;

			//! All 3 buffers are created in upload heap.
			P<Gfx::IResource> m_vb_res;
			P<Gfx::IResource> m_ib_res;
			P<Gfx::IResource> m_cb_res;

			usize m_vb_count;
			usize m_ib_count;
			usize m_cb_count;

			struct DrawCallRange
			{
				P<IDrawList> m_list;
				u32 m_first;
				u32 m_count;
			};

			Vector<DrawCallRange> m_dcs;

			DrawListRenderer() :
				m_vb_size(0),
				m_ib_size(0),
				m_cb_size(0),
				m_vb_count(0),
				m_ib_count(0),
				m_cb_count(0) {}

			RV init();

			virtual Gfx::IGraphicDevice* get_device() override
			{
				return m_rp->get_device();
			}
			virtual Gfx::IRenderPass* get_render_pass() override
			{
				return m_rp;
			}
			virtual void add_draw_list(IDrawList* draw_list) override
			{
				lutsassert();
				lucheck(draw_list);
				DrawCallRange r;
				r.m_list = draw_list;
				r.m_first = 0;
				r.m_count = draw_list->count_draw_calls();
				m_dcs.push_back(move(r));
			}
			virtual void add_draw_calls(IDrawList* draw_list, u32 first, u32 num_draw_calls) override
			{
				lutsassert();
				lucheck(draw_list);
				DrawCallRange r;
				r.m_list = draw_list;
				r.m_first = first;
				r.m_count = num_draw_calls;
				m_dcs.push_back(move(r));
			}
			virtual RV render(Gfx::ICommandBuffer* target_cmd_buffer, Gfx::IResource* rt, u32 width, u32 height) override;
		};
	}
}