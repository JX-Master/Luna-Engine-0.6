// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file main.hpp
* @author JXMaster
* @date 2020/3/9
*/
#include "TestHeader.hpp"
#include "Tests/TestNone.hpp"
#include "Tests/TestTriangle.hpp"
#include "Tests/TestImage.hpp"

namespace Luna
{
	P<Gfx::ICommandBuffer> g_cb;

	P<Gfx::IResource> g_rt;

	P<Gfx::IWindow> g_window;
	P<Gfx::ISwapChain> g_swap_chain;

	ERendererTestCase g_current_test = ERendererTestCase::none;
	P<ITestCase> g_test_none;
	P<ITestCase> g_test_triangle;
	P<ITestCase> g_test_image;

	class RenderTestInputCallback : public Gfx::IWindowEventListener
	{
	public:
		lucid("{ea2e4f0c-7b9e-4450-9302-e9d4ca94b50f}");
		luiimpl(RenderTestInputCallback, Gfx::IWindowEventListener, IObject);

		RenderTestInputCallback() {}

		virtual bool on_window_event(Gfx::IWindow* window, Gfx::EWindowEvent e, void* data) override
		{
			if (e == Gfx::EWindowEvent::key_down)
			{
				Input::EKeyCode k = *((const Input::EKeyCode*)data);
				if (k == Input::EKeyCode::right)
				{
					g_current_test = (ERendererTestCase)(((u32)g_current_test + 1) % ((u32)ERendererTestCase::count));
					return true;
				}
				if (k == Input::EKeyCode::left)
				{
					g_current_test = (ERendererTestCase)(((u32)g_current_test + ((u32)ERendererTestCase::count) - 1) % ((u32)ERendererTestCase::count));
					return true;
				}
			}
			if (e == Gfx::EWindowEvent::resize)
			{
				Gfx::WindowResize resize = *(const Gfx::WindowResize*)(data);
				// resize back buffer.
				g_rt = Renderer::device()->new_resource(Gfx::ResourceDesc::tex2d(Gfx::EResourceFormat::rgba8_unorm,
					Gfx::EAccessType::gpu_local, Gfx::EResourceUsageFlag::render_target | Gfx::EResourceUsageFlag::shader_resource, resize.size.x, resize.size.y, 1, 1), nullptr).get();
				g_test_none->resize();
				g_test_triangle->resize();
				g_test_image->resize();
				return true;
			}
			return false;
		}
	};

	P<RenderTestInputCallback> g_input_callback;
}

using namespace Luna;
using namespace Luna::Gfx;

void init()
{
	g_cb = Renderer::main_graphic_queue()->new_command_buffer().get();

	g_window = Gfx::new_window("Luna Renderer Test").get();

	g_swap_chain = Gfx::new_swap_chain(Renderer::main_graphic_queue(), g_window, SwapChainDesc(0, 0, EResourceFormat::rgba8_unorm, 2, true)).get();

	auto sz = g_window->size();
	g_rt = Renderer::device()->new_resource(Gfx::ResourceDesc::tex2d(EResourceFormat::rgba8_unorm, 
		EAccessType::gpu_local, EResourceUsageFlag::render_target | EResourceUsageFlag::shader_resource, sz.x, sz.y, 1, 1), nullptr).get();

	g_test_none = newobj<TestNone>();
	g_test_triangle = newobj<TestTriangle>();
	g_test_image = newobj<TestImage>();
	luassert_always(g_test_none->init().valid());
	luassert_always(g_test_triangle->init().valid());
	luassert_always(g_test_image->init().valid());

	// register event.
	g_input_callback = newobj<RenderTestInputCallback>();
	Input::get_device(Name("window")).get().as<IWindowInputDevice>()->add_listener(g_input_callback);
}

void run()
{
	while (!g_window->closed())
	{
		new_frame();
		Input::update();

		g_cb->reset();

		switch (g_current_test)
		{
		case ERendererTestCase::none:
			g_test_none->update();
			break;
		case ERendererTestCase::triangle:
			g_test_triangle->update();
			break;
		case ERendererTestCase::image:
			g_test_image->update();
		default:
			break;
		}

		luassert_always(g_swap_chain->present(g_rt.get(), 0, 1).valid());
		g_swap_chain->wait();
	}
}

void shutdown()
{
	g_input_callback = nullptr;

	g_test_image = nullptr;
	g_test_triangle = nullptr;
	g_test_none = nullptr;

	g_swap_chain = nullptr;
	g_window = nullptr;

	g_rt = nullptr;
	g_cb = nullptr;
}

int main()
{
	Luna::init();
	::init();
	run();
	::shutdown();
	Luna::close();
}