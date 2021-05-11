// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestImage.hpp
* @author JXMaster
* @date 2020/3/14
*/
#include "../TestHeader.hpp"

namespace Luna
{
	//! This test case that draws a triangle on the screen.
	class TestImage : public ITestCase
	{
	public:
		lucid("{b2623f01-c907-4d73-983a-9096bd159be6}");
		luiimpl(TestImage, ITestCase, IObject);

		TestImage() {}

		P<Gfx::IRenderPass> m_render_pass;
		P<Gfx::IFrameBuffer> m_fbo;
		P<Gfx::IShaderInputLayout> m_shader_input_layout;
		P<Gfx::IPipelineState> m_pso;
		P<Gfx::IResource> m_tex;
		P<Gfx::IViewSet> m_view_set;

		P<Gfx::IResource> m_vb;
		P<Gfx::IResource> m_ib;

		virtual RV init() override;
		virtual void update() override;
		virtual void resize() override;
	};
}

extern const unsigned char test_image_data_v[65536];