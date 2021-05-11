// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestNone.cpp
* @author JXMaster
* @date 2020/3/9
*/
#include "TestNone.hpp"
#include <Runtime/Math.hpp>

namespace Luna
{
	using namespace Gfx;
	RV TestNone::init()
	{
		lutry
		{
			Gfx::RenderPassDesc rpd;
			rpd.attachments[0].format = EResourceFormat::rgba8_unorm;
			rpd.attachments[0].load_op = EAttachmentLoadOp::dont_care;
			rpd.attachments[0].store_op = EAttachmentStoreOp::store;
			rpd.num_attachments = 1;
			rpd.depth_load_op = EAttachmentLoadOp::dont_care;
			rpd.depth_stencil_format = EResourceFormat::unknown;
			rpd.depth_store_op = EAttachmentStoreOp::dont_care;
			rpd.sample_count = 1;
			rpd.stencil_load_op = EAttachmentLoadOp::dont_care;
			rpd.stencil_store_op = EAttachmentStoreOp::dont_care;
			rpd.use_depth_stencil = false;
			luset(m_render_pass, Renderer::device()->new_render_pass(rpd));
			luset(m_fbo, Renderer::device()->new_frame_buffer(m_render_pass, 1, g_rt.get_address_of(), nullptr, nullptr, nullptr));
		}
		lucatchret;
		return RV();
	}
	void TestNone::update()
	{
		g_cb->begin_render_pass(m_render_pass, m_fbo, 0, nullptr, 0.0f, 0);
		g_cb->resource_barrier(Gfx::ResourceBarrierDesc::as_transition(g_rt, EResourceState::render_target, 0));
		auto clear_color = Color::blue_violet;
		g_cb->clear_render_target_view(0, clear_color.m, 0, nullptr);
		g_cb->end_render_pass();
		g_cb->submit();
		g_cb->wait();
	}
	void TestNone::resize()
	{
		m_fbo = Renderer::device()->new_frame_buffer(m_render_pass, 1, g_rt.get_address_of(), nullptr, nullptr, nullptr).get();
	}
}