// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestHeader.hpp
* @author JXMaster
* @date 2020/3/9
*/
#pragma once

#include <Core/Core.hpp>
#include <Gfx/Gfx.hpp>
#include <Input/Input.hpp>
#include <Renderer/Renderer.hpp>

namespace luna
{
	extern P<ILogger> g_logger;
	extern P<gfx::ICommandBuffer> g_cb;

	extern P<gfx::IWindow> g_window;
	extern P<gfx::ISwapChain> g_swap_chain;

	extern P<gfx::IResource> g_rt;

	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}

	inline ILogger* get_logger()
	{
		return g_logger.get();
	}

	enum class ERendererTestCase : uint32
	{
		// Renders a blue screen.
		none = 0,
		// Renders one triangle on the screen.
		triangle = 1,
		// Renders one texture on the screen.
		image = 2,

		count,
	};

	//! Marks the current running test.
	extern ERendererTestCase g_current_test;

	//! @interface ITestCase
	//! Implements this to add test cases to the Renderer Test.
	struct ITestCase : public IObject
	{
		luiid("{1265e34d-cb6f-46f8-b566-5f477867b462}");

		virtual RV init() = 0;
		virtual void update() = 0;
		virtual void resize() = 0;
	};
}