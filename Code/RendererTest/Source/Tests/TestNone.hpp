// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestNone.hpp
* @author JXMaster
* @date 2020/3/9
*/
#include "../TestHeader.hpp"

namespace Luna
{
	//! This test case just clears the screen to one color.
	class TestNone : public ITestCase
	{
	public:
		lucid("{af50a389-8183-4731-9b12-1bab3c8c2d6b}");
		luiimpl(TestNone, ITestCase, IObject);

		TestNone() {}

		P<Gfx::IRenderPass> m_render_pass;
		P<Gfx::IFrameBuffer> m_fbo;

		virtual RV init() override;
		virtual void update() override;
		virtual void resize() override;
	};
}