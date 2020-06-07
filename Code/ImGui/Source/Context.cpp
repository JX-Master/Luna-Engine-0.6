// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Context.cpp
* @author JXMaster
* @date 2020/4/5
*/
#include "Context.hpp"
#include <Input/IKeyboard.hpp>
#include <Input/IMouse.hpp>
#include <Font/Font.hpp>

namespace luna
{
	namespace imgui
	{
		bool ImGuiWindowEventListener::on_window_event(gfx::IWindow* window, gfx::EWindowEvent e, void* data)
		{
			using namespace gfx;
			if (!m_ctx)
			{
				return false;
			}
			MutexGuard guard(m_mtx);
			ImGui::SetCurrentContext(m_ctx->m_ctx);
			ImGuiIO& io = ImGui::GetIO();
			switch (e)
			{
			case EWindowEvent::mouse_down:
			{
				const WindowMouseDown* param = (const WindowMouseDown*)data;
				int button = 0;
				if (param->key == input::EMouseKey::lb)
				{
					button = 0;
				}
				if (param->key == input::EMouseKey::rb)
				{
					button = 1;
				}
				if (param->key == input::EMouseKey::mb)
				{
					button = 2;
				}
				if (param->key == input::EMouseKey::x1b)
				{
					button = 3;
				}
				if (param->key == input::EMouseKey::x2b)
				{
					button = 4;
				}
				io.MouseDown[button] = true;
				return true;
			}
			case EWindowEvent::mouse_up:
			{
				const WindowMouseUp* param = (const WindowMouseUp*)data;
				int button = 0;
				if (param->key == input::EMouseKey::lb)
				{
					button = 0;
				}
				if (param->key == input::EMouseKey::rb)
				{
					button = 1;
				}
				if (param->key == input::EMouseKey::mb)
				{
					button = 2;
				}
				if (param->key == input::EMouseKey::x1b)
				{
					button = 3;
				}
				if (param->key == input::EMouseKey::x2b)
				{
					button = 4;
				}
				io.MouseDown[button] = false;
				return true;
			}
			case EWindowEvent::mouse_move:
			{
				const WindowMouseMove* param = (const WindowMouseMove*)data;
				io.MousePos = ImVec2((float32)param->pos.x, (float32)param->pos.y);
				return true;
			}
			case EWindowEvent::mouse_wheel:
			{
				const WindowMouseWheel* param = (const WindowMouseWheel*)data;
				io.MouseWheel += param->delta;
				return true;
			}
			case EWindowEvent::mouse_hwheel:
			{
				const WindowMouseWheel* param = (const WindowMouseWheel*)data;
				io.MouseWheelH += param->delta;
				return true;
			}
			case EWindowEvent::key_down:
			{
				const input::EKeyCode* param = (const input::EKeyCode*)data;
				io.KeysDown[(uint32)*param] = 1;
				return true;
			}
			case EWindowEvent::key_up:
			{
				const input::EKeyCode* param = (const input::EKeyCode*)data;
				io.KeysDown[(uint32)*param] = 0;
				return true;
			}
			case EWindowEvent::input_char:
			{
				const char32_t* ch = (const char32_t*)data;
				io.AddInputCharacter((unsigned int)*ch);
				return true;
			}
			}
			return false;
		}

		RV Context::init(gfx::IGraphicDevice* render_device, gfx::ICommandBuffer* init_cmds, gfx::EResourceFormat rt_format, float32 dpi_scale)
		{
			using namespace gfx;

			m_ctx = ImGui::CreateContext();
			MutexGuard guard(m_mtx);
			ImGui::SetCurrentContext(m_ctx);
			ImGui::StyleColorsDark();
			ImGuiIO& io = ImGui::GetIO();
			io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
			io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
			io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
			io.BackendPlatformName = "imgui_impl_luna";
			io.BackendRendererName = "imgui_impl_luna_gfx";
			io.KeyMap[ImGuiKey_Tab] = (int)input::EKeyCode::tab;
			io.KeyMap[ImGuiKey_LeftArrow] = (int)input::EKeyCode::left;
			io.KeyMap[ImGuiKey_RightArrow] = (int)input::EKeyCode::right;
			io.KeyMap[ImGuiKey_UpArrow] = (int)input::EKeyCode::up;
			io.KeyMap[ImGuiKey_DownArrow] = (int)input::EKeyCode::down;
			io.KeyMap[ImGuiKey_PageUp] = (int)input::EKeyCode::page_up;
			io.KeyMap[ImGuiKey_PageDown] = (int)input::EKeyCode::page_down;
			io.KeyMap[ImGuiKey_Home] = (int)input::EKeyCode::home;
			io.KeyMap[ImGuiKey_End] = (int)input::EKeyCode::end;
			io.KeyMap[ImGuiKey_Insert] = (int)input::EKeyCode::insert;
			io.KeyMap[ImGuiKey_Delete] = (int)input::EKeyCode::del;
			io.KeyMap[ImGuiKey_Backspace] = (int)input::EKeyCode::backspace;
			io.KeyMap[ImGuiKey_Space] = (int)input::EKeyCode::space_bar;
			io.KeyMap[ImGuiKey_Enter] = (int)input::EKeyCode::enter;
			io.KeyMap[ImGuiKey_Escape] = (int)input::EKeyCode::esc;
			io.KeyMap[ImGuiKey_KeyPadEnter] = (int)input::EKeyCode::enter;
			io.KeyMap[ImGuiKey_A] = (int)input::EKeyCode::a;
			io.KeyMap[ImGuiKey_C] = (int)input::EKeyCode::c;
			io.KeyMap[ImGuiKey_V] = (int)input::EKeyCode::v;
			io.KeyMap[ImGuiKey_X] = (int)input::EKeyCode::x;
			io.KeyMap[ImGuiKey_Y] = (int)input::EKeyCode::y;
			io.KeyMap[ImGuiKey_Z] = (int)input::EKeyCode::z;

			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

			m_dev = render_device;
			lutry
			{
				// Build texture atlas
				ImGuiIO& io = ImGui::GetIO();
				unsigned char* pixels;
				int width, height;

				auto def_font = font::get_default_font();

				io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(def_font->data()->data()), def_font->data()->size(), 18.0f * dpi_scale);

				io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
				luset(m_font, m_dev->new_resource(ResourceDesc::tex2d(EResourceFormat::rgba8_unorm, EAccessType::gpu_local, EResourceUsageFlag::shader_resource, width, height, 1, 1)));
				uint32 pitch_alignment;
				m_dev->check_feature_support(EDeviceFeature::texture_data_pitch_alignment, &pitch_alignment);
				uint32 upload_pitch = (width * 4 + pitch_alignment - 1u) & ~(pitch_alignment - 1u);
				uint32 upload_size = height * upload_pitch;
				lulet(up_buf, m_dev->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::none, upload_size)));
				void* mapped;
				luset(mapped, up_buf->map_subresource(0, false, 1, 0));
				for (int32 y = 0; y < height; ++y)
				{
					memcpy((void*)((uintptr_t)mapped + y * upload_pitch), pixels + y * width * 4, width * 4);
				}
				up_buf->unmap_subresource(0, true);
				init_cmds->attach_graphic_object(up_buf);
				TextureCopyLocation src, dest;
				src.type = ETextureCopyType::placed_footprint;
				src.placed_footprint.offset = 0;
				src.placed_footprint.footprint.width = width;
				src.placed_footprint.footprint.height = height;
				src.placed_footprint.footprint.depth = 1;
				src.placed_footprint.footprint.format = EResourceFormat::rgba8_unorm;
				src.placed_footprint.footprint.row_pitch = upload_pitch;
				src.resource = up_buf;
				dest.type = ETextureCopyType::subresource_index;
				dest.subresource_index = 0;
				dest.resource = m_font;
				init_cmds->resource_barrier(ResourceBarrierDesc::as_transition(m_font, EResourceState::copy_dest));
				init_cmds->copy_texture_region(dest, 0, 0, 0, src, nullptr);
				init_cmds->resource_barrier(ResourceBarrierDesc::as_transition(m_font, EResourceState::shader_resource_pixel));
				io.Fonts->TexID = m_font.get();

				// Create pso/rp.
				luset(m_rp, m_dev->new_render_pass(RenderPassDesc({ AttachmentDesc(EResourceFormat::rgba8_unorm, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::store) },
					EResourceFormat::unknown, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, EAttachmentLoadOp::dont_care, EAttachmentStoreOp::dont_care, 1, false)));
				ShaderInputGroupDesc groups[3] = {
					ShaderInputGroupDesc(EShaderInputGroupType::cbv, 0, 1, EShaderVisibility::vertex),
					ShaderInputGroupDesc(EShaderInputGroupType::srv, 0, 1, EShaderVisibility::pixel),
					ShaderInputGroupDesc(EShaderInputGroupType::sampler, 0, 1, EShaderVisibility::pixel)
				};
				luset(m_layout, m_dev->new_shader_input_layout(ShaderInputLayoutDesc(groups, 3, 
					EShaderInputLayoutFlag::allow_input_assembler_input_layout | 
					EShaderInputLayoutFlag::deny_domain_shader_access | 
					EShaderInputLayoutFlag::deny_geometry_shader_access | 
					EShaderInputLayoutFlag::deny_hull_shader_access)));

				GraphicsPipelineStateDesc ps_desc;
				memzero(&ps_desc, sizeof(GraphicsPipelineStateDesc));
				ps_desc.primitive_topology_type = EPrimitiveTopologyType::triangle;
				ps_desc.sample_mask = uint32_max_v;
				ps_desc.sample_quality = 0;
				ps_desc.blend_state = BlendDesc(false, false, { RenderTargetBlendDesc(true, false, EBlendFactor::src_alpha,
					EBlendFactor::inv_src_alpha, EBlendOp::add, EBlendFactor::inv_src_alpha, EBlendFactor::zero, EBlendOp::add, ELogicOp::noop, EColorWriteMask::all) });
				ps_desc.rasterizer_state = RasterizerDesc(EFillMode::solid, ECullMode::none, 0, 0.0f, 0.0f, 1, false, true, false, false, false);
				ps_desc.depth_stencil_state = DepthStencilDesc(false, false, EComparisonFunc::always, false, 0x00, 0x00, DepthStencilOpDesc(Default()), DepthStencilOpDesc(Default()));
				ps_desc.ib_strip_cut_value = EIndexBufferStripCutValue::disabled;
				InputElementDesc input_elements[3] = {
					InputElementDesc("POSITION", 0, EResourceFormat::rg32_float),
					InputElementDesc("TEXCOORD", 0, EResourceFormat::rg32_float),
					InputElementDesc("COLOR", 0, EResourceFormat::rgba8_unorm)
				};
				ps_desc.input_layout.input_elements = input_elements;
				ps_desc.input_layout.num_elements = 3;
				ps_desc.vs.code = m_vs_blob->data();
				ps_desc.vs.length = m_vs_blob->size();
				ps_desc.ps.code = m_ps_blob->data();
				ps_desc.ps.length = m_ps_blob->size();
				luset(m_pso, m_dev->new_graphics_pipeline_state(m_layout, m_rp, ps_desc));

				// Create constant buffer.
				uint32 buffer_size_align;
				m_dev->check_feature_support(EDeviceFeature::buffer_data_alignment, &buffer_size_align);
				luset(m_cb, m_dev->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::constant_buffer, align_upper(sizeof(Float4x4), buffer_size_align))));
			
				luexp(init_cmds->submit());
				init_cmds->wait();
				luexp(init_cmds->reset());
			}

			lucatchret;
			return s_ok;
		}

		RV Context::attach_system_window(gfx::IWindow* window)
		{
			P<ImGuiWindowEventListener> listener = box_ptr(new_obj<ImGuiWindowEventListener>(get_module_allocator()));
			listener->m_ctx = this;
			listener->m_window = window;
			auto window_name = intern_name("window");
			auto window_input_device = input::get_device(window_name);
			if (failed(window_input_device))
			{
				set_err(e_item_not_exist, "Window Input Device not found.");
				return e_user_failure;
			}
			window_input_device.get().as<gfx::IWindowInputDevice>()->add_listener(listener);
			m_listener = listener;
			return s_ok;
		}

		void Context::new_frame()
		{
			MutexGuard _lock(m_mtx);
			ImGui::SetCurrentContext(m_ctx);
			ImGuiIO& io = ImGui::GetIO();

			P<input::IMouse> mouse;

			// Read keyboard modifiers inputs
			io.KeyCtrl = false;
			io.KeyShift = false;
			io.KeyAlt = false;
			io.KeySuper = false;

			P<input::IKeyboard> keyboard;

			auto _keyboard = input::get_device(intern_name("keyboard"));
			if (succeeded(_keyboard))
			{
				keyboard = _keyboard.get();
				if (keyboard)
				{
					io.KeyCtrl = keyboard->keydown(input::EKeyCode::ctrl);
					io.KeyShift = keyboard->keydown(input::EKeyCode::shift);
					io.KeyAlt = keyboard->keydown(input::EKeyCode::alt);
				}
			}
			auto _mouse = input::get_device(intern_name("mouse"));
			if (succeeded(_mouse))
			{
				mouse = _mouse.get();
			}

			if (m_listener)
			{
				// Setup display size (every frame to accommodate for window resizing)
				auto window = m_listener->m_window.lock();
				if (!window)
				{
					m_listener = nullptr;
				}
				else
				{
					auto sz = window->size();
					io.DisplaySize = ImVec2(sz);

					// Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
					if (io.WantSetMousePos)
					{
						auto pos = window->position();
						if (mouse)
						{
							auto _ = mouse->set_pos((int32)io.MousePos.x + pos.x, (int32)io.MousePos.y + pos.y);
						}
					}

					// Update OS mouse cursor with the cursor requested by imgui
					ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
					if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
					{
						gfx::EMouseCursorShape cursor;
						switch (mouse_cursor)
						{
						case ImGuiMouseCursor_None:			cursor = gfx::EMouseCursorShape::none; break;
						case ImGuiMouseCursor_Arrow:        cursor = gfx::EMouseCursorShape::arrow; break;
						case ImGuiMouseCursor_TextInput:    cursor = gfx::EMouseCursorShape::text_input; break;
						case ImGuiMouseCursor_ResizeAll:    cursor = gfx::EMouseCursorShape::resize_all; break;
						case ImGuiMouseCursor_ResizeEW:     cursor = gfx::EMouseCursorShape::resize_we; break;
						case ImGuiMouseCursor_ResizeNS:     cursor = gfx::EMouseCursorShape::resize_ns; break;
						case ImGuiMouseCursor_ResizeNESW:   cursor = gfx::EMouseCursorShape::resize_nesw; break;
						case ImGuiMouseCursor_ResizeNWSE:   cursor = gfx::EMouseCursorShape::resize_nwse; break;
						case ImGuiMouseCursor_Hand:         cursor = gfx::EMouseCursorShape::hand; break;
						case ImGuiMouseCursor_NotAllowed:   cursor = gfx::EMouseCursorShape::not_allowed; break;
						}
						if (mouse)
						{
							auto _ = window->set_mouse_cursor_shape(cursor);	// Don't care if this fails.
						}
					}
				}
			}

			// Setup time step.
			io.DeltaTime = (float32)get_last_frame_ticks() / (float32)get_ticks_per_second();

			ImGui::NewFrame();
			ImGuizmo::BeginFrame();
		}

		RV Context::render(gfx::ICommandBuffer* cmd_buffer, gfx::IResource* render_target)
		{
			using namespace gfx;
			lutry
			{
				MutexGuard _lock(m_mtx);
				ImGui::SetCurrentContext(m_ctx);
				ImDrawData* draw_data = ImGui::GetDrawData();
				// Avoid rendering when minimized
				if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
					return s_ok;
				// Create and grow vertex/index buffers if needed
				auto dev = cmd_buffer->get_device();
				if (!m_vb || m_vb_size < (uint32)draw_data->TotalVtxCount)
				{
					m_vb_size = draw_data->TotalVtxCount + 5000;
					luset(m_vb, dev->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::vertex_buffer, m_vb_size * sizeof(ImDrawVert))));
				}
				if (!m_ib || m_ib_size < (uint32)draw_data->TotalIdxCount)
				{
					m_ib_size = draw_data->TotalIdxCount + 10000;
					luset(m_ib, dev->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::index_buffer, m_ib_size * sizeof(ImDrawIdx))));
				}

				// Upload vertex/index data into a single contiguous GPU buffer
				void* vtx_resource, *idx_resource;
				luset(vtx_resource, m_vb->map_subresource(0, false, 1, 0));
				luset(idx_resource, m_ib->map_subresource(0, false, 1, 0));
				ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource;
				ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource;
				for (int32 n = 0; n < draw_data->CmdListsCount; ++n)
				{
					const ImDrawList* cmd_list = draw_data->CmdLists[n];
					memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
					memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
					vtx_dst += cmd_list->VtxBuffer.Size;
					idx_dst += cmd_list->IdxBuffer.Size;
				}
				m_vb->unmap_subresource(0, true);
				m_ib->unmap_subresource(0, true);

				// Create frame buffer.
				IResource* rtvs = render_target;
				lulet(fbo, dev->new_frame_buffer(m_rp, 1, &rtvs, nullptr, nullptr, nullptr));
				cmd_buffer->attach_graphic_object(fbo);

				// Setup orthographic projection matrix into our constant buffer
				// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
				Float4x4 mvp;
				{
					float L = draw_data->DisplayPos.x;
					float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
					float T = draw_data->DisplayPos.y;
					float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
					mvp =
					{
						{ 2.0f / (R - L),		0.0f,				0.0f,       0.0f },
						{ 0.0f,					2.0f / (T - B),     0.0f,       0.0f },
						{ 0.0f,					0.0f,				0.5f,       0.0f },
						{ (R + L) / (L - R),	(T + B) / (B - T),  0.5f,       1.0f },
					};
					void* cb_resource;
					luset(cb_resource, m_cb->map_subresource(0, false, 1, 0));
					memcpy(cb_resource, mvp.m, sizeof(Float4x4));
					m_cb->unmap_subresource(0, false, 0, sizeof(Float4x4));
				}

				cmd_buffer->resource_barrier(ResourceBarrierDesc::as_transition(render_target, EResourceState::render_target, 0));

				cmd_buffer->begin_render_pass(m_rp, fbo, 0, nullptr, 0.0f, 0);

				cmd_buffer->set_viewport(Viewport(0.0f, 0.0f, draw_data->DisplaySize.x, draw_data->DisplaySize.y, 0.0f, 1.0f));

				cmd_buffer->set_vertex_buffers(0, 1, &VertexBufferViewDesc(m_vb, 0, (uint32)(m_vb_size * sizeof(ImDrawVert)), (uint32)sizeof(ImDrawVert)));
				cmd_buffer->set_index_buffer(m_ib, 0, (uint32)(m_ib_size * sizeof(ImDrawIdx)), sizeof(ImDrawIdx) == 2 ? EResourceFormat::r16_uint : EResourceFormat::r32_uint);
				cmd_buffer->set_primitive_topology(EPrimitiveTopology::triangle_list);
				cmd_buffer->set_pipeline_state(m_pso);
				cmd_buffer->set_graphic_shader_input_layout(m_layout);
				const float32 blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
				cmd_buffer->set_blend_factor(blend_factor);

				// Render command lists.
				int32 vtx_offset = 0;
				int32 idx_offset = 0;
				Float2 clip_off = { draw_data->DisplayPos.x, draw_data->DisplayPos.y };
				for (int32 n = 0; n < draw_data->CmdListsCount; ++n)
				{
					const ImDrawList* cmd_list = draw_data->CmdLists[n];
					for (int32 cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; ++cmd_i)
					{
						const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
						// Apply Scissor, Bind texture, Draw
						const RectI r = { 
							(int32)(pcmd->ClipRect.x - clip_off.x), 
							(int32)(pcmd->ClipRect.y - clip_off.y), 
							(int32)(pcmd->ClipRect.z - clip_off.x), 
							(int32)(pcmd->ClipRect.w - clip_off.y) };
						ViewSetDesc desc;
						desc.num_cbvs = 1;
						desc.num_srvs = 1;
						desc.num_uavs = 0;
						desc.num_samplers = 1;
						lulet(vs, dev->new_view_set(m_layout, desc));
						uint32 cb_align;
						m_dev->check_feature_support(EDeviceFeature::buffer_data_alignment, &cb_align);
						vs->set_cbv(0, m_cb, ConstantBufferViewDesc(0, (uint32)align_upper(sizeof(Float4x4), cb_align)));
						vs->set_srv(0, (IResource*)pcmd->TextureId);
						vs->set_sampler(0, SamplerDesc(EFilter::min_mag_mip_linear, ETextureAddressMode::warp, ETextureAddressMode::warp, ETextureAddressMode::warp));
						cmd_buffer->attach_graphic_object(vs);
						cmd_buffer->resource_barrier(ResourceBarrierDesc::as_transition((IResource*)pcmd->TextureId, EResourceState::shader_resource_pixel));
						cmd_buffer->set_graphic_view_set(vs);
						cmd_buffer->set_scissor_rect(r);
						cmd_buffer->draw_indexed(pcmd->ElemCount, pcmd->IdxOffset + idx_offset, pcmd->VtxOffset + vtx_offset);
					}
					idx_offset += cmd_list->IdxBuffer.Size;
					vtx_offset += cmd_list->VtxBuffer.Size;
				}

				cmd_buffer->end_render_pass();
			}
			lucatchret;
			return s_ok;
		}

		struct InputTextCallback_UserData
		{
			IStringBuffer* Str;
			input_text_callback_t*  ChainCallback;
			void* ChainCallbackUserData;
		};

		static int InputTextCallback(ImGuiInputTextCallbackData* data)
		{
			InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
			if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
			{
				// Resize string callback
				// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
				IStringBuffer* str = user_data->Str;
				IM_ASSERT(data->Buf == str->c_str());
				str->resize(data->BufTextLen);
				data->Buf = (char*)str->c_str();
			}
			else if (user_data->ChainCallback)
			{
				// Forward to user callback, if any
				data->UserData = user_data->ChainCallbackUserData;
				return user_data->ChainCallback((InputTextCallbackData*)data);
			}
			return 0;
		}

		bool Context::input_text(const char* label, IStringBuffer* buf, EInputTextFlag flags, input_text_callback_t* callback, void* user_data)
		{
			MutexGuard _lock(m_mtx);
			ImGui::SetCurrentContext(m_ctx);
			IM_ASSERT((flags & EInputTextFlag::callback_resize) == EInputTextFlag::none);
			flags |= EInputTextFlag::callback_resize;

			InputTextCallback_UserData cb_user_data;
			cb_user_data.Str = buf;
			cb_user_data.ChainCallback = callback;
			cb_user_data.ChainCallbackUserData = user_data;
			return ImGui::InputText(label, (char*)buf->c_str(), buf->capacity() + 1, (uint32)flags, InputTextCallback, &cb_user_data);
		}
		bool Context::input_text_multiline(const char* label, IStringBuffer* buf, const Float2& size, EInputTextFlag flags, input_text_callback_t* callback, void* user_data)
		{
			MutexGuard _lock(m_mtx);
			ImGui::SetCurrentContext(m_ctx);
			IM_ASSERT((flags & EInputTextFlag::callback_resize) == EInputTextFlag::none);
			flags |= EInputTextFlag::callback_resize;

			InputTextCallback_UserData cb_user_data;
			cb_user_data.Str = buf;
			cb_user_data.ChainCallback = callback;
			cb_user_data.ChainCallbackUserData = user_data;
			return ImGui::InputTextMultiline(label, (char*)buf->c_str(), buf->capacity() + 1, size, (uint32)flags, InputTextCallback, &cb_user_data);
		}
		bool Context::input_text_with_hint(const char* label, const char* hint, IStringBuffer* buf, EInputTextFlag flags, input_text_callback_t* callback, void* user_data)
		{
			MutexGuard _lock(m_mtx);
			ImGui::SetCurrentContext(m_ctx);
			IM_ASSERT((flags & EInputTextFlag::callback_resize) == EInputTextFlag::none);
			flags |= EInputTextFlag::callback_resize;

			InputTextCallback_UserData cb_user_data;
			cb_user_data.Str = buf;
			cb_user_data.ChainCallback = callback;
			cb_user_data.ChainCallbackUserData = user_data;
			return ImGui::InputTextWithHint(label, hint, (char*)buf->c_str(), buf->capacity() + 1, (uint32)flags, InputTextCallback, &cb_user_data);
		}
		void Context::gizmo(
			Float4x4& world_matrix,
			const Float4x4& view,
			const Float4x4& projection,
			const RectF& viewport_rect,
			EGizmoOperation operation,
			EGizmoMode mode,
			float32 snap,
			bool enabled,
			bool orthographic,
			Float4x4* delta_matrix,
			bool* is_mouse_hover,
			bool* is_mouse_moving
		)
		{
			MutexGuard _lock(m_mtx);
			MutexGuard _glock(m_gizmo_mtx);
			ImGui::SetCurrentContext(m_ctx);
			
			// Set States.
			ImGuizmo::SetDrawlist();
			ImGuizmo::Enable(enabled);
			ImGuizmo::SetRect(viewport_rect.left, viewport_rect.top, viewport_rect.right, viewport_rect.bottom);
			ImGuizmo::SetOrthographic(orthographic);

			float32* fdelta_matrix = nullptr;
			float32* fsnap = nullptr;
			if (delta_matrix)
			{
				fdelta_matrix = &(delta_matrix->_11);
			}
			if (snap)
			{
				fsnap = &snap;
			}

			ImGuizmo::Manipulate(&view._11, &projection._11, (ImGuizmo::OPERATION)operation, (ImGuizmo::MODE)mode, &world_matrix._11, fdelta_matrix, fsnap);

			if (is_mouse_hover)
			{
				*is_mouse_hover = ImGuizmo::IsOver();
			}
			if (is_mouse_moving)
			{
				*is_mouse_moving = ImGuizmo::IsUsing();
			}
		}
	}
}