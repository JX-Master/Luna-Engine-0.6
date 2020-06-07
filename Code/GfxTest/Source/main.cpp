// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file min.hpp
* @author JXMaster
* @date 2019/6/17
*/
#include <Core/Core.hpp>
#include <Gfx/Gfx.hpp>
#include <Input/Input.hpp>

namespace luna
{
	P<ILogger> g_logger;

	inline ILogger* get_logger()
	{
		return g_logger.get();
	}
}

using namespace luna;
using namespace luna::gfx;

void run()
{
	g_logger = new_logger(intern_name("GfxText").get()).get();
	P<IGraphicDevice> dev = new_device().get();
	uint32 i = 0;
	auto adapter = enum_adapter(i);
	while (succeeded(adapter))
	{
		GraphicAdapterDesc desc = adapter.get()->desc();
		log_message("Video Adapter - %s", desc.name);
		log_message("Dedicated Memory: %u", desc.local_memory);
		log_message("Shared Memory: %u", desc.shared_memory);
		if ((desc.flags & EGraphicAdapterFlag::dedicated) != EGraphicAdapterFlag::none)
		{
			log_message("Dedicated Video Card");
		}
		else
		{
			log_message("Embedded Video Card.");
		}
		if ((desc.flags & EGraphicAdapterFlag::software) != EGraphicAdapterFlag::none)
		{
			log_message("Software Simulated Video Card");
		}
		++i;
		adapter = enum_adapter(i);
	}

	P<IWindow> window = new_window("Test Window").get();

	while (!window->closed())
	{
		new_frame();
		input::update();
	}
}

int main()
{
	luna::init();
	luna::input::init();
	luna::gfx::init();

	run();

	g_logger = nullptr;
	luna::shutdown();
}