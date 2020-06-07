// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestNone.hpp
* @author JXMaster
* @date 2020/3/9
*/
#include "../TestHeader.hpp"

namespace luna
{
	//! This test case just clears the screen to one color.
	class TestNone : public ITestCase
	{
	public:
		lucid("{af50a389-8183-4731-9b12-1bab3c8c2d6b}");
		luiimpl(TestNone, ITestCase, IObject);

		TestNone(IAllocator* alloc) :
			luibind(alloc) {}

		P<gfx::IRenderPass> m_render_pass;
		P<gfx::IFrameBuffer> m_fbo;

		virtual RV init() override;
		virtual void update() override;
		virtual void resize() override;
	};
}