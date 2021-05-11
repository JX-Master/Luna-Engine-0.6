// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file EasyDrawDemo.cpp
* @author JXMaster
* @date 2019/10/2
*/
#include <Runtime/Runtime.hpp>
#include <Renderer/Renderer.hpp>
#include <Gfx/Gfx.hpp>
#include <EasyDraw/EasyDraw.hpp>
#include <Font/Font.hpp>
#include <RectPack/RectPack.hpp>
#include <Image/Image.hpp>
#include <Input/Input.hpp>

namespace Luna
{
	class RenderTestInputCallback : public Gfx::IWindowEventListener
	{
	public:
		lucid("{ea2e4f0c-7b9e-4450-9302-e9d4ca94b50f}");
		luiimpl(RenderTestInputCallback, Gfx::IWindowEventListener, IObject);

		P<Gfx::IResource> m_back_buffer;
		P<Gfx::ISwapChain> m_swap_chain;

		RenderTestInputCallback() {}

		virtual bool on_window_event(Gfx::IWindow* window, Gfx::EWindowEvent e, void* data) override
		{
			if (e == Gfx::EWindowEvent::resize)
			{
				Gfx::WindowResize resize = *(const Gfx::WindowResize*)(data);
				// resize back buffer.
				Gfx::SwapChainDesc d = m_swap_chain->desc();
				m_swap_chain->resize_buffers(2, resize.size.x, resize.size.y, d.format);
				m_back_buffer = Renderer::device()->new_resource(Gfx::ResourceDesc::tex2d(Gfx::EResourceFormat::rgba8_unorm,
					Gfx::EAccessType::gpu_local, Gfx::EResourceUsageFlag::render_target | Gfx::EResourceUsageFlag::shader_resource, resize.size.x, resize.size.y, 1, 1), nullptr).get();
				return true;
			}
			return false;
		}
	};
}

using namespace Luna;

void run()
{
	using namespace Gfx;
	P<IGraphicDevice> dev = Renderer::device();
	P<EasyDraw::IDrawList> drawlist = EasyDraw::new_draw_list();
	P<IRenderPass> rp = dev->new_render_pass(Gfx::RenderPassDesc({ Gfx::AttachmentDesc(EResourceFormat::rgba8_unorm, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care) },
		EResourceFormat::unknown, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, 1, false)).get();
	P<EasyDraw::IDrawListRenderer> renderer = EasyDraw::new_draw_list_renderer(rp).get();
	P <Font::IFontAtlas> font_atlas;
	P<EasyDraw::IDrawPath> path = EasyDraw::new_draw_path();

	P<Gfx::IWindow> window = Gfx::new_window("EasyDraw Demo").get();
	P<Gfx::ISwapChain> swap_chain = Gfx::new_swap_chain(Renderer::main_graphic_queue(), window, SwapChainDesc(0, 0, EResourceFormat::rgba8_unorm, 2, true)).get();

	P<RenderTestInputCallback> cb = newobj<RenderTestInputCallback>();
	cb->m_swap_chain = swap_chain;
	auto sz = window->size();
	cb->m_back_buffer = dev->new_resource(Gfx::ResourceDesc::tex2d(Gfx::EResourceFormat::rgba8_unorm,
		Gfx::EAccessType::gpu_local, Gfx::EResourceUsageFlag::render_target | Gfx::EResourceUsageFlag::shader_resource, sz.x, sz.y, 1, 1), nullptr).get();
	P<IWindowInputDevice> winput = Input::get_device(Name("window")).get();
	winput->add_listener(cb);
	winput = nullptr;
	font_atlas = Font::new_font_atlas(Font::get_default_font(), 0, 24.0f).get();
	font_atlas->set_scale_factor(window->dpi_scale_factor());
	Font::FontAtlasRange range;
	range.start_codepoint = 32;
	range.num_chars = 95;
	font_atlas->add_glyph_ranges(1, &range);
	font_atlas->render();

	// Test write.
	auto img = font_atlas->get_rendered_image();
	auto f = platform_open_file("test_write.png", EFileOpenFlag::write | EFileOpenFlag::user_buffering, EFileCreationMode::create_always).get();
	img->save(f, Image::EImageTargetFormat::png);
	f = nullptr;

	P<ICommandBuffer> cmdbuf = Renderer::main_graphic_queue()->new_command_buffer().get();
	P<EasyDraw::IFontTexture> font_tex = EasyDraw::new_font_texture(font_atlas, cmdbuf).get();

	while (!window->closed())
	{
		new_frame();
		Input::update();
		drawlist->reset();

		if (font_atlas->get_scale_factor() != (window->dpi_scale_factor()))
		{
			// Recreate atlas.
			font_atlas->set_scale_factor(window->dpi_scale_factor());
			font_atlas->render();
			font_tex->reset(font_atlas, cmdbuf);
		}

		drawlist->set_font(font_tex);

		// Window.
		drawlist->draw_rectangle_filled(Float2(100, 100), Float2(400, 700), Float4(0.1294f, 0.1294f, 0.1490f, 1.0f));
		drawlist->draw_rectangle_filled(Float2(100, 150), Float2(400, 650), Float4(0.1568f, 0.1568f, 0.1803f, 1.0f));
		drawlist->draw_rectangle_bordered(Float2(100, 100), Float2(400, 700), Float4(0.0f, 0.0f, 0.0f, 1.0f), 2.0f);
		drawlist->draw_line(Float2(100, 150), Float2(500, 150), Float4(0.0f, 0.0f, 0.0f, 1.0f), 2.0f, false);
		drawlist->drawcall_barrier();

		Float2 off(125, 300);

		// Normal Slider.
		drawlist->draw_rounded_rectangle_filled(off + Float2(0, 2), Float2(200, 4), Float4(0.1843f, 0.1843f, 0.2078f, 1.0f), 2, false);
		drawlist->draw_rounded_rectangle_filled(off, Float2(200, 4), Float4(0.0902f, 0.0902f, 0.0902f, 1.0f), 2, false);

		drawlist->draw_circle_filled(off + Float2(50, 2.5), 7, Float4(0.5686f, 0.5686f, 0.5686f, 1.0f));
		drawlist->draw_circle_bordered(off + Float2(50, 2.5), 7, Float4(0.1216f, 0.1216f, 0.1216f, 1.0f), 1.0f);

		off += Float2(0, 30);

		// Float4ed Selected Slider.
		drawlist->draw_rounded_rectangle_filled(off + Float2(0, 2), Float2(200, 4), Float4(0.1843f, 0.1843f, 0.2078f, 1.0f), 2, false);
		drawlist->draw_rounded_rectangle_filled(off, Float2(200, 4), Float4(0.0902f, 0.0902f, 0.0902f, 1.0f), 2, false);
		drawlist->draw_rounded_rectangle_filled(off, Float2(130, 4), Float4(0.1725f, 0.5804f, 0.0941f, 1.0f), 2, false);

		drawlist->draw_circle_filled(off + Float2(130, 2.5), 7, Float4(0.5686f, 0.5686f, 0.5686f, 1.0f));
		drawlist->draw_circle_bordered(off + Float2(130, 2.5), 7, Float4(0.1216f, 0.1216f, 0.1216f, 1.0f), 1.0f);
		drawlist->draw_circle_filled(off + Float2(130, 2.5), 3, Float4(0.3725f, 0.3725f, 0.3725f, 1.0f));

		off += Float2(0, 30);

		// Rect Slider.
		drawlist->draw_rectangle_filled(off, Float2(200, 3), Float4(0.2627f, 0.2784f, 0.30196f, 1.0f));
		drawlist->draw_rectangle_filled(off + Float2(30, -7), Float2(5, 16), Color::white);
		drawlist->draw_rectangle_bordered(off + Float2(30, -7), Float2(5, 16), Color::black, 1.0f);

		off += Float2(0, 30);

		Float2 button_off = off;
		// Button.
		drawlist->draw_rounded_rectangle_bordered(off, Float2(160, 40), Float4(0.2627f, 0.2784f, 0.30196f, 1.0f), 20, 2);

		off += Float2(175, 0);
		drawlist->draw_rounded_rectangle_bordered(off, Float2(160, 40), Float4(0.9020f, 0.2941f, 0.2627f, 1.0f), 20, 2);

		char buf[64];
		auto scaled_size = window->dpi_scaled_size();
		sprintf_s(buf, "Window : %dx%d", scaled_size.x, scaled_size.y);

		drawlist->draw_text("Sample Window", Float2(110, 110), Float2(200, 25));
		drawlist->draw_text(buf, Float2(125, 160), Float2(200, 25));

		ResourceDesc rd = cb->m_back_buffer->desc();
		sprintf_s(buf, "Backbuffer : %dx%d", (u32)rd.width, rd.height);
		drawlist->draw_text(buf, Float2(125, 185), Float2(200, 25));
		drawlist->draw_text("Button", button_off + Float2(50, 6), Float2(100, 50), Float4(0.67059f, 0.67059f, 0.67059f, 1.0f));
		drawlist->draw_text("Button", button_off + Float2(225, 6), Float2(100, 50), Float4(0.67059f, 0.67059f, 0.67059f, 1.0f));

		sprintf_s(buf, "FPS: %f", get_fps());
		drawlist->draw_text(buf, Float2(125, 210), Float2(200, 25));



		renderer->add_draw_list(drawlist);
		renderer->render(cmdbuf, cb->m_back_buffer, scaled_size.x, scaled_size.y);
		cmdbuf->submit();
		cmdbuf->wait();
		cmdbuf->reset();
		swap_chain->present(cb->m_back_buffer, 0, 1);
		swap_chain->wait();
	}
}

int main()
{
	// Start modules.
	Luna::init();
	run();
	Luna::close();
	return 0;
}