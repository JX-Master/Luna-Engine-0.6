// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestTriangle.hpp
* @author JXMaster
* @date 2020/3/14
*/
#include "../TestHeader.hpp"

namespace Luna
{
	//! This test case that draws a triangle on the screen.
	class TestTriangle : public ITestCase
	{
	public:
		lucid("{4527652f-8467-4ba2-aed8-5aeb7a4514f1}");
		luiimpl(TestTriangle, ITestCase, IObject);

		TestTriangle() {}

		P<Gfx::IRenderPass> m_render_pass;
		P<Gfx::IFrameBuffer> m_fbo;
		P<Gfx::IShaderInputLayout> m_shader_input_layout;
		P<Gfx::IPipelineState> m_pso;

		P<Gfx::IResource> m_vb;

		virtual RV init() override;
		virtual void update() override;
		virtual void resize() override;
	};
}