// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ImGuiDemo.cpp
* @author JXMaster
* @date 2020/4/12
*/
#include <Renderer/Renderer.hpp>
#include <Gfx/Gfx.hpp>
#include <Input/Input.hpp>
#include <ImGui/ImGui.hpp>
#include <RectPack/RectPack.hpp>
#include <Image/Image.hpp>
#include <Font/Font.hpp>
#include <Runtime/Runtime.hpp>
#include <Runtime/Platform.hpp>

using namespace Luna;

int main()
{
	using namespace Gfx;
	// Start modules.
	Luna::init();
	P<IGraphicDevice> dev = Renderer::device();

	P<Gfx::IWindow> window = Gfx::new_window("ImGui Demo").get();
	P<Gfx::ISwapChain> swap_chain = Gfx::new_swap_chain(Renderer::main_graphic_queue(), window, SwapChainDesc(0, 0, EResourceFormat::rgba8_unorm, 2, true)).get();

	P<ICommandBuffer> cmdbuf = Renderer::main_graphic_queue()->new_command_buffer().get();

	// Create back buffer.
	P<Gfx::IResource> back_buffer;
	u32 w = 0, h = 0;

	// Create ImGui context.
	P<ImGui::IContext> ctx = ImGui::new_context(Renderer::device(), cmdbuf).get();
	ctx->attach_system_window(window);

	P<Gfx::IRenderPass> rp = Renderer::device()->new_render_pass(Gfx::RenderPassDesc({ Gfx::AttachmentDesc(EResourceFormat::rgba8_unorm, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::store) },
		EResourceFormat::unknown, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, 1, false)).get();
	P<Gfx::IFrameBuffer> back_buffer_fbo;

	while (true)
	{
		new_frame();
		Input::update();

		if (window->closed())
		{
			break;
		}

		// Recreate the back buffer if needed.
		auto sz = window->size();
		auto ww = sz.x;
		auto wh = sz.y;
		if (!back_buffer || ww != w || wh != h)
		{
			swap_chain->resize_buffers(2, ww, wh, EResourceFormat::unknown);
			f32 clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			back_buffer = Renderer::device()->new_resource(ResourceDesc::tex2d(EResourceFormat::rgba8_unorm, EAccessType::gpu_local, EResourceUsageFlag::render_target, ww, wh, 1, 1), 
				&ClearValue::as_color(EResourceFormat::rgba8_unorm, clear_color)).get();
			w = ww;
			h = wh;
			back_buffer_fbo = Renderer::device()->new_frame_buffer(rp, 1, back_buffer.get_address_of(), nullptr, nullptr, nullptr).get();
		}

		ctx->new_frame();

		ctx->show_demo_window();

		ctx->end_frame();
		Float4U clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		cmdbuf->begin_render_pass(rp, back_buffer_fbo, 1, &clear_color, 0.0f, 0);
		cmdbuf->clear_render_target_view(0, clear_color.m, 0, nullptr);
		cmdbuf->end_render_pass();
		ctx->render(cmdbuf, back_buffer);
		cmdbuf->submit();
		cmdbuf->wait();
		cmdbuf->reset();
		swap_chain->present(back_buffer, 0, 1);
		swap_chain->wait();
	}
	rp = nullptr;
	back_buffer_fbo = nullptr;

	ctx = nullptr;
	back_buffer = nullptr;
	swap_chain = nullptr;
	window = nullptr;
	cmdbuf = nullptr;
	dev = nullptr;
	Luna::close();
	return 0;
}