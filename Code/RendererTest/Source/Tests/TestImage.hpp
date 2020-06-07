// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestImage.hpp
* @author JXMaster
* @date 2020/3/14
*/
#include "../TestHeader.hpp"

namespace luna
{
	//! This test case that draws a triangle on the screen.
	class TestImage : public ITestCase
	{
	public:
		lucid("{b2623f01-c907-4d73-983a-9096bd159be6}");
		luiimpl(TestImage, ITestCase, IObject);

		TestImage(IAllocator* alloc) :
			luibind(alloc) {}

		P<gfx::IRenderPass> m_render_pass;
		P<gfx::IFrameBuffer> m_fbo;
		P<gfx::IShaderInputLayout> m_shader_input_layout;
		P<gfx::IPipelineState> m_pso;
		P<gfx::IResource> m_tex;
		P<gfx::IViewSet> m_view_set;

		P<gfx::IResource> m_vb;
		P<gfx::IResource> m_ib;

		virtual RV init() override;
		virtual void update() override;
		virtual void resize() override;
	};
}

extern const unsigned char test_image_data_v[65536];