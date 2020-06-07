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

namespace luna
{
	P<ILogger> g_logger;
	P<gfx::ICommandBuffer> g_cb;

	P<gfx::IResource> g_rt;

	P<gfx::IWindow> g_window;
	P<gfx::ISwapChain> g_swap_chain;

	ERendererTestCase g_current_test = ERendererTestCase::none;
	P<ITestCase> g_test_none;
	P<ITestCase> g_test_triangle;
	P<ITestCase> g_test_image;

	class RenderTestInputCallback : public gfx::IWindowEventListener
	{
	public:
		lucid("{ea2e4f0c-7b9e-4450-9302-e9d4ca94b50f}");
		luiimpl(RenderTestInputCallback, gfx::IWindowEventListener, IObject);

		RenderTestInputCallback() :
			luibind(get_module_allocator()) {}

		virtual bool on_window_event(gfx::IWindow* window, gfx::EWindowEvent e, void* data) override
		{
			if (e == gfx::EWindowEvent::key_down)
			{
				input::EKeyCode k = *((const input::EKeyCode*)data);
				if (k == input::EKeyCode::right)
				{
					g_current_test = (ERendererTestCase)(((uint32)g_current_test + 1) % ((uint32)ERendererTestCase::count));
					return true;
				}
				if (k == input::EKeyCode::left)
				{
					g_current_test = (ERendererTestCase)(((uint32)g_current_test + ((uint32)ERendererTestCase::count) - 1) % ((uint32)ERendererTestCase::count));
					return true;
				}
			}
			if (e == gfx::EWindowEvent::resize)
			{
				gfx::WindowResize resize = *(const gfx::WindowResize*)(data);
				// resize back buffer.
				g_rt = renderer::device()->new_resource(gfx::ResourceDesc::tex2d(gfx::EResourceFormat::rgba8_unorm,
					gfx::EAccessType::gpu_local, gfx::EResourceUsageFlag::render_target | gfx::EResourceUsageFlag::shader_resource, resize.size.x, resize.size.y, 1, 1), nullptr).get();
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

using namespace luna;
using namespace luna::gfx;

void init()
{
	g_logger = new_logger(intern_name("RenderTest").get()).get();
	input::init();
	gfx::init();
	auto r = renderer::init();
	if (failed(r))
	{
		log_error(explain(r));
		lupanic();
	}
	g_cb = renderer::main_graphic_queue()->new_command_buffer().get();

	g_window = gfx::new_window("Luna Renderer Test").get();

	g_swap_chain = gfx::new_swap_chain(renderer::main_graphic_queue(), g_window, SwapChainDesc(0, 0, EResourceFormat::rgba8_unorm, 2, true)).get();

	auto sz = g_window->size();
	g_rt = renderer::device()->new_resource(gfx::ResourceDesc::tex2d(EResourceFormat::rgba8_unorm, 
		EAccessType::gpu_local, EResourceUsageFlag::render_target | EResourceUsageFlag::shader_resource, sz.x, sz.y, 1, 1), nullptr).get();

	g_test_none = box_ptr<TestNone>(new_obj<TestNone>(get_module_allocator()));
	g_test_triangle = box_ptr<TestTriangle>(new_obj<TestTriangle>(get_module_allocator()));
	g_test_image = box_ptr<TestImage>(new_obj<TestImage>(get_module_allocator()));
	luassert_always(succeeded(g_test_none->init()));
	luassert_always(succeeded(g_test_triangle->init()));
	luassert_always(succeeded(g_test_image->init()));

	// register event.
	g_input_callback = new_obj<RenderTestInputCallback>();
	input::get_device(intern_name("window").get()).get().as<IWindowInputDevice>()->add_listener(g_input_callback);
}

void run()
{
	while (!g_window->closed())
	{
		new_frame();
		input::update();

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

		g_swap_chain->present(g_rt.get(), 0, 1);
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
	g_logger = nullptr;
}

int main()
{
	luna::init();
	::init();
	run();
	::shutdown();
	luna::shutdown();
}