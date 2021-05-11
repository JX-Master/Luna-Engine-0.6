// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file min.hpp
* @author JXMaster
* @date 2019/6/17
*/
#include <Runtime/Runtime.hpp>
#include <Core/Core.hpp>
#include <Gfx/Gfx.hpp>
#include <Input/Input.hpp>

using namespace Luna;
using namespace Luna::Gfx;

void run()
{
	P<IGraphicDevice> dev = new_device().get();
	u32 i = 0;
	auto adapter = enum_adapter(i);
	while (adapter.valid())
	{
		GraphicAdapterDesc desc = adapter.get()->desc();
		printf("Video Adapter - %s\n", desc.name);
		printf("Dedicated Memory: %u\n", desc.local_memory);
		printf("Shared Memory: %u\n", desc.shared_memory);
		if ((desc.flags & EGraphicAdapterFlag::dedicated) != EGraphicAdapterFlag::none)
		{
			printf("Dedicated Video Card\n");
		}
		else
		{
			printf("Embedded Video Card.\n");
		}
		if ((desc.flags & EGraphicAdapterFlag::software) != EGraphicAdapterFlag::none)
		{
			printf("Software Simulated Video Card\n");
		}
		++i;
		adapter = enum_adapter(i);
	}

	P<IWindow> window = new_window("Test Window").get();

	while (!window->closed())
	{
		new_frame();
		Input::update();
	}
}

int main()
{
	luassert_always(succeeded(Luna::init()));
	run();
	Luna::close();
}