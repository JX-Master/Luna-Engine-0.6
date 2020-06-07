// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestTriangle.hpp
* @author JXMaster
* @date 2020/3/14
*/
#include "../TestHeader.hpp"

namespace luna
{
	//! This test case that draws a triangle on the screen.
	class TestTriangle : public ITestCase
	{
	public:
		lucid("{4527652f-8467-4ba2-aed8-5aeb7a4514f1}");
		luiimpl(TestTriangle, ITestCase, IObject);

		TestTriangle(IAllocator* alloc) :
			luibind(alloc) {}

		P<gfx::IRenderPass> m_render_pass;
		P<gfx::IFrameBuffer> m_fbo;
		P<gfx::IShaderInputLayout> m_shader_input_layout;
		P<gfx::IPipelineState> m_pso;

		P<gfx::IResource> m_vb;

		virtual RV init() override;
		virtual void update() override;
		virtual void resize() override;
	};
}