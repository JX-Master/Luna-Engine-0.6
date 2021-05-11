// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file CommandBuffer.cpp
* @author JXMaster
* @date 2020/3/11
*/
#include "CommandBuffer.hpp"

#ifdef LUNA_GFX_D3D12

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			inline u32 calc_subresource_index(u32 mip_slice, u32 array_slice, u32 mip_levels)
			{
				return mip_slice + array_slice * mip_levels;
			}

			/*inline bool is_state_compatible_to_generic_read(EResourceState s)
			{
				switch (s)
				{
				case EResourceState::vertex_and_constant_buffer:
				case EResourceState::index_buffer:
				case EResourceState::shader_resource_non_pixel:
				case EResourceState::shader_resource_pixel:
				case EResourceState::indirect_argument:
				case EResourceState::copy_source:
					return true;
				default:
					break;
				}
				return false;
			}*/

			void ResourceStateTrackingSystem::append_transition(Resource* res, u32 subresource, EResourceState before, EResourceState after, EResourceBarrierFlag flags)
			{
				// Early out for unnecessary calls.
				if (before == after)
				{
					return;
				}
				// Use implicit transition whenever possible.
				// Refs: https://docs.microsoft.com/en-us/windows/win32/direct3d12/using-resource-barriers-to-synchronize-resource-states-in-direct3d-12#implicit-state-transitions
				if (before == EResourceState::common)
				{
					if (res->m_states.empty())
					{
						// Buffers or textures with EResourceUsageFlag::simultaneous_access
						return;
					}
					else if (
						(after == EResourceState::shader_resource_non_pixel) ||
						(after == EResourceState::shader_resource_pixel) ||
						(after == EResourceState::copy_dest) ||
						(after == EResourceState::copy_source))
					{
						return;
					}
				}
				D3D12_RESOURCE_BARRIER t;
				t.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				t.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				if ((flags & EResourceBarrierFlag::begin_only) != EResourceBarrierFlag::none)
				{
					t.Flags |= D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY;
				}
				if ((flags & EResourceBarrierFlag::end_only) != EResourceBarrierFlag::none)
				{
					t.Flags |= D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
				}
				t.Transition.StateBefore = encode_resource_state(before);
				t.Transition.StateAfter = encode_resource_state(after);
				if (subresource == resource_barrier_all_subresources_v)
				{
					t.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				}
				else
				{
					t.Transition.Subresource = subresource;
				}
				t.Transition.pResource = res->m_res.Get();
				m_barriers.push_back(t);
			}

			void ResourceStateTrackingSystem::pack_transition(Resource* res, u32 subresource, EResourceState after, EResourceBarrierFlag flags)
			{
				if (subresource == resource_barrier_all_subresources_v)
				{
					u32 num_subresources = res->count_subresources();
					for (u32 i = 0; i < num_subresources; ++i)
					{
						pack_transition(res, i, after, flags);
					}
				}
				else
				{
					// Checks if the subresource is resolved.
					ResourceKey k;
					k.m_res = res;
					k.m_subres = subresource;
					auto citer = m_current.find(k);
					if (citer == m_current.end())
					{
						// The subresource is not resolved.

						// The `begin_only` call is discarded for unresolved resource.
						if ((flags & EResourceBarrierFlag::begin_only) != EResourceBarrierFlag::none)
						{
							return;
						}

						// The `end_only` flag will be dropped.
						if ((flags & EResourceBarrierFlag::end_only) != EResourceBarrierFlag::none)
						{
							flags &= (~EResourceBarrierFlag::end_only);
						}
							
						if (res->m_states.empty())
						{
							// If this resource does not have a global state, always proceed as common.
							append_transition(res, subresource, EResourceState::common, after, flags);
						}
						else
						{
							// If the resource has global states, the transition is deferred to submit time.
							m_unresloved.insert(make_pair(k, after));
						}
						m_current.insert(make_pair(k, after));
					}
					else
					{
						// The subresource is resolved.
						// Insert a transition always.
						append_transition(res, subresource, citer->second, after, flags);

						// Ignore applying changes to tracking system for `begin_only`.
						if ((flags & EResourceBarrierFlag::begin_only) == EResourceBarrierFlag::none)
						{
							citer->second = after;
						}
					}
				}
			}

			void ResourceStateTrackingSystem::resolve()
			{
				begin_new_transition_batch();
				for (auto& i : m_unresloved)
				{
					Resource* res = i.first.m_res;
					luassert(!res->m_states.empty());
					append_transition(res, i.first.m_subres, res->m_states[i.first.m_subres], i.second, EResourceBarrierFlag::none);
				}
			}

			void ResourceStateTrackingSystem::apply(ECommandQueueType type)
			{
				for (auto& i : m_current)
				{
					if (i.first.m_res->m_states.empty())
					{
						continue;
					}
					// Any resources accessed by Copy queue can be implicitly decayed to common state.
					// Any read state that can be implicitly promoted from common state can be implicitly decayed to common state.
					if (type == ECommandQueueType::copy /* || is_texture_decayable_to_common(i.second) */)
					{
						i.first.m_res->m_states[i.first.m_subres] = EResourceState::common;
					}
					else
					{
						i.first.m_res->m_states[i.first.m_subres] = i.second;
					}
				}
			}

			RV CommandBuffer::init()
			{
				HRESULT hr;
				hr = m_device->m_device->CreateCommandAllocator(encode_command_list_type(m_queue->m_desc.type), IID_PPV_ARGS(&m_ca));
				if (FAILED(hr))
				{
					return BasicError::bad_system_call();
				}
				hr = m_device->m_device->CreateCommandList(0, encode_command_list_type(m_queue->m_desc.type), m_ca.Get(), NULL, IID_PPV_ARGS(&m_li));
				if (FAILED(hr))
				{
					return BasicError::bad_system_call();
				}
				hr = m_device->m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
				if (FAILED(hr))
				{
					return BasicError::bad_system_call();
				}
				m_event = ::CreateEventA(NULL, TRUE, FALSE, NULL);
				if (m_event == NULL)
				{
					return BasicError::bad_system_call();
				}
				m_wait_value = 1;	// The fist wait value.
				return RV();
			}

			RV CommandBuffer::reset()
			{
				BOOL b = ::ResetEvent(m_event);
				if (!b)
				{
					return BasicError::bad_system_call();
				}
				++m_wait_value;
				if (!m_cmdlist_closed)
				{
					HRESULT hr = m_li->Close();
					if (FAILED(hr))
					{
						return BasicError::bad_system_call();
					}
					m_cmdlist_closed = true;
				}
				HRESULT hr = m_ca->Reset();
				if (FAILED(hr))
				{
					return BasicError::bad_system_call();
				}
				hr = m_li->Reset(m_ca.Get(), NULL);
				m_cmdlist_closed = false;
				if (FAILED(hr))
				{
					return BasicError::bad_system_call();
				}
				m_tracking_system.reset();
				m_objs.clear();
				return RV();
			}

			void CommandBuffer::begin_render_pass(IRenderPass* render_pass, IFrameBuffer* fbo, u32 num_rt_clear_values, const Float4U* clear_values,
				f32 depth_clear_value, u8 stencil_clear_value)
			{
				lucheck(render_pass && fbo);
				m_render_pass = render_pass;
				m_frame_buffer = fbo;

				if (!m_frame_buffer->m_rts.empty())
				{
					if (m_frame_buffer->m_ds)
					{
						D3D12_CPU_DESCRIPTOR_HANDLE h = m_frame_buffer->get_rtv_cpu_handle(0);
						D3D12_CPU_DESCRIPTOR_HANDLE dsh = m_frame_buffer->get_dsv_cpu_handle();
						m_li->OMSetRenderTargets(m_frame_buffer->count_rtv(), &h, TRUE, &dsh);
					}
					else
					{
						D3D12_CPU_DESCRIPTOR_HANDLE h = m_frame_buffer->get_rtv_cpu_handle(0);
						m_li->OMSetRenderTargets(m_frame_buffer->count_rtv(), &h, TRUE, NULL);
					}
				}
				else
				{
					if (m_frame_buffer->m_ds)
					{
						D3D12_CPU_DESCRIPTOR_HANDLE dsh = m_frame_buffer->get_dsv_cpu_handle();
						m_li->OMSetRenderTargets(0, NULL, TRUE, &dsh);
					}
					else
					{
						m_li->OMSetRenderTargets(0, NULL, TRUE, NULL);
					}
				}

				// Clear render target and depth stencil if needed.
				for (u32 i = 0; (i < m_render_pass->m_desc.num_attachments) && (i < num_rt_clear_values); ++i)
				{
					if ((m_render_pass->m_desc.attachments[i].load_op == EAttachmentLoadOp::clear) && m_frame_buffer->m_rts[i])
					{
						m_li->ClearRenderTargetView(m_frame_buffer->get_rtv_cpu_handle(i), clear_values[i].m, 0, NULL);
					}
				}
				if ((m_render_pass->m_desc.depth_load_op == EAttachmentLoadOp::clear) || (m_render_pass->m_desc.stencil_load_op == EAttachmentLoadOp::clear))
				{
					if (m_render_pass->m_desc.use_depth_stencil && m_frame_buffer->m_ds)
					{
						D3D12_CLEAR_FLAGS flags = (D3D12_CLEAR_FLAGS)0;
						if (m_render_pass->m_desc.depth_load_op == EAttachmentLoadOp::clear)
						{
							flags |= D3D12_CLEAR_FLAG_DEPTH;
						}
						if (m_render_pass->m_desc.stencil_load_op == EAttachmentLoadOp::clear)
						{
							flags |= D3D12_CLEAR_FLAG_STENCIL;
						}
						m_li->ClearDepthStencilView(m_frame_buffer->get_dsv_cpu_handle(), flags, depth_clear_value, stencil_clear_value, 0, NULL);
					}
				}
			}

			void CommandBuffer::set_pipeline_state(IPipelineState* pso)
			{
				PipelineState* p = static_cast<PipelineState*>(pso);
				m_li->SetPipelineState(p->m_pso.Get());
			}

			void CommandBuffer::set_graphic_shader_input_layout(IShaderInputLayout* shader_input_layout)
			{
				lutsassert();
				lucheck(shader_input_layout);
				ShaderInputLayout* o = static_cast<ShaderInputLayout*>(shader_input_layout);
				m_graphic_shader_input_layout = o;
				m_li->SetGraphicsRootSignature(o->m_rs.Get());
			}

			void CommandBuffer::set_vertex_buffers(u32 start_slot, u32 num_views, const VertexBufferViewDesc* views)
			{
				lutsassert();
				m_vbs.resize(start_slot + num_views);
				for (u32 i = start_slot; i < num_views; ++i)
				{
					m_vbs[i] = views[i - start_slot];
				}

				D3D12_VERTEX_BUFFER_VIEW* vbv = (D3D12_VERTEX_BUFFER_VIEW*)alloca(sizeof(D3D12_VERTEX_BUFFER_VIEW) * num_views);
				for (u32 i = 0; i < num_views; ++i)
				{
					vbv[i].BufferLocation = static_cast<Resource*>(views[i].resource)->m_res->GetGPUVirtualAddress() + views[i].offset_in_bytes;
					vbv[i].SizeInBytes = views[i].size_in_bytes;
					vbv[i].StrideInBytes = views[i].stride_in_bytes;
				}
				m_li->IASetVertexBuffers(start_slot, num_views, vbv);
			}

			void CommandBuffer::set_index_buffer(IResource* buffer, u32 offset_in_bytes, u32 size_in_bytes, EResourceFormat format)
			{
				lutsassert();
				Resource* b = static_cast<Resource*>(buffer);
				m_ib = b;
				D3D12_INDEX_BUFFER_VIEW v;
				v.BufferLocation = b->m_res->GetGPUVirtualAddress() + offset_in_bytes;
				v.Format = encode_resource_format(format);
				v.SizeInBytes = size_in_bytes;
				m_li->IASetIndexBuffer(&v);
			}

			void CommandBuffer::set_graphic_view_set(IViewSet* view_set)
			{
				lutsassert();
				lucheck_msg(m_graphic_shader_input_layout, "Graphic Shader Input Layout must be set before Graphic View Set can be bound!");

				m_view_set = view_set;
				ID3D12DescriptorHeap* heaps[2];
				u32 num_heaps = 0;
				if (m_view_set->m_cbv_srv_uav_heap)
				{
					heaps[num_heaps] = m_view_set->m_cbv_srv_uav_heap.Get();
					++num_heaps;
				}
				if (m_view_set->m_sampler_heap)
				{
					heaps[num_heaps] = m_view_set->m_sampler_heap.Get();
					++num_heaps;
				}
				m_li->SetDescriptorHeaps(num_heaps, heaps);

				// Set parameters.
				// The parameter layout in heap:
				// 
				// Heap Start
				// v
				// |-CBV-|-SRV-----|-UAV--|
				// How heaps are bind to groups are described by Pipeline State.
				auto& groups = m_graphic_shader_input_layout->m_groups;
				u32 num_groups = (u32)groups.size();

				for (u32 i = 0; i < num_groups; ++i)
				{
					D3D12_GPU_DESCRIPTOR_HANDLE h;
					switch (groups[i].type)
					{
					case EShaderInputGroupType::cbv:
						h = m_view_set->m_cbv_srv_uav_heap->GetGPUDescriptorHandleForHeapStart();
						h.ptr += m_view_set->m_cbv_srv_uav_size * groups[i].base_shader_register;
						break;
					case EShaderInputGroupType::srv:
						h = m_view_set->m_cbv_srv_uav_heap->GetGPUDescriptorHandleForHeapStart();
						h.ptr += m_view_set->m_cbv_srv_uav_size * (groups[i].base_shader_register + m_view_set->m_cbs.size());
						break;
					case EShaderInputGroupType::uav:
						h = m_view_set->m_cbv_srv_uav_heap->GetGPUDescriptorHandleForHeapStart();
						h.ptr += m_view_set->m_cbv_srv_uav_size * (groups[i].base_shader_register + m_view_set->m_cbs.size() + m_view_set->m_srvs.size());
						break;
					case EShaderInputGroupType::sampler:
						h = m_view_set->m_sampler_heap->GetGPUDescriptorHandleForHeapStart();
						h.ptr += m_view_set->m_sampler_size * groups[i].base_shader_register;
						break;
					default:
						lupanic();
						break;
					}
					m_li->SetGraphicsRootDescriptorTable(i, h);
				}
			}

			void CommandBuffer::set_primitive_topology(EPrimitiveTopology primitive_topology)
			{
				lutsassert();
				D3D12_PRIMITIVE_TOPOLOGY t;
				switch (primitive_topology)
				{
				case EPrimitiveTopology::undefined:
					t = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
					break;
				case EPrimitiveTopology::point_list:
					t = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
					break;
				case EPrimitiveTopology::line_list:
					t = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
					break;
				case EPrimitiveTopology::line_strip:
					t = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
					break;
				case EPrimitiveTopology::triangle_list:
					t = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					break;
				case EPrimitiveTopology::triangle_strip:
					t = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
					break;
				case EPrimitiveTopology::line_list_adj:
					t = D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
					break;
				case EPrimitiveTopology::line_strip_adj:
					t = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
					break;
				case EPrimitiveTopology::triangle_list_adj:
					t = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
					break;
				case EPrimitiveTopology::triangle_strip_adj:
					t = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
					break;
				case EPrimitiveTopology::patchlist_1_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_2_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_3_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_4_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_5_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_6_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_7_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_8_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_9_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_10_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_11_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_12_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_13_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_14_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_15_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_16_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_17_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_18_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_19_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_20_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_21_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_22_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_23_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_24_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_25_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_26_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_27_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_28_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_29_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_30_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_31_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST;
					break;
				case EPrimitiveTopology::patchlist_32_control_point:
					t = D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;
					break;
				default:
					lupanic();
					break;
				}
				m_li->IASetPrimitiveTopology(t);
			}

			void CommandBuffer::set_stream_output_targets(u32 start_slot, u32 num_views, const StreamOutputBufferView * views)
			{
				lutsassert();
				D3D12_STREAM_OUTPUT_BUFFER_VIEW* vs = (D3D12_STREAM_OUTPUT_BUFFER_VIEW*)alloca(sizeof(D3D12_STREAM_OUTPUT_BUFFER_VIEW) * num_views);
				for (u32 i = 0; i < num_views; ++i)
				{
					vs[i].BufferLocation = static_cast<Resource*>(views[i].soresource)->m_res->GetGPUVirtualAddress() + views[i].offset_in_bytes;
					vs[i].SizeInBytes = views[i].size_in_bytes;
					vs[i].BufferFilledSizeLocation = static_cast<Resource*>(views[i].buffer_filled_size_resource)->m_res->GetGPUVirtualAddress() + views[i].buffer_filled_size_offset;
				}
				m_li->SOSetTargets(start_slot, num_views, vs);
			}

			void CommandBuffer::set_viewports(u32 num_viewports, const Viewport* viewports)
			{
				lutsassert();
				D3D12_VIEWPORT* vs = (D3D12_VIEWPORT*)alloca(sizeof(D3D12_VIEWPORT) * num_viewports);
				for (u32 i = 0; i < num_viewports; ++i)
				{
					vs[i].Height = viewports[i].height;
					vs[i].MaxDepth = viewports[i].max_depth;
					vs[i].MinDepth = viewports[i].min_depth;
					vs[i].TopLeftX = viewports[i].top_left_x;
					vs[i].TopLeftY = viewports[i].top_left_y;
					vs[i].Width = viewports[i].width;
				}
				m_li->RSSetViewports(num_viewports, vs);
			}

			void CommandBuffer::set_scissor_rects(u32 num_rects, const RectI * rects)
			{
				lutsassert();
				D3D12_RECT* rs = (D3D12_RECT*)alloca(sizeof(D3D12_RECT) * num_rects);
				for (u32 i = 0; i < num_rects; ++i)
				{
					rs[i].top = rects[i].top;
					rs[i].bottom = rects[i].bottom;
					rs[i].left = rects[i].left;
					rs[i].right = rects[i].right;
				}
				m_li->RSSetScissorRects(num_rects, rs);
			}

			void CommandBuffer::set_blend_factor(const f32 blend_factor[4])
			{
				lutsassert();
				m_li->OMSetBlendFactor(blend_factor);
			}

			void CommandBuffer::set_stencil_ref(u32 stencil_ref)
			{
				lutsassert();
				m_li->OMSetStencilRef(stencil_ref);
			}

			RV CommandBuffer::submit()
			{
				lutsassert();
				HRESULT hr;
				hr = m_li->Close();
				if (FAILED(hr))
				{
					return BasicError::bad_system_call();
				}
				m_cmdlist_closed = true;

				// Resolve barriers.
				m_tracking_system.resolve();

				// Submit commands.
				if (!m_tracking_system.m_barriers.empty())
				{
					ComPtr<ID3D12GraphicsCommandList> li;
					hr = m_device->m_device->CreateCommandList(0, encode_command_list_type(m_queue->m_desc.type), m_ca.Get(), NULL, IID_PPV_ARGS(&li));
					if (FAILED(hr))
					{
						return BasicError::bad_system_call();
					}
					li->ResourceBarrier((UINT)m_tracking_system.m_barriers.size(), m_tracking_system.m_barriers.data());
					hr = li->Close();
					if (FAILED(hr))
					{
						return BasicError::bad_system_call();
					}
					ID3D12CommandList* lists[2];
					lists[0] = li.Get();
					lists[1] = m_li.Get();
					m_queue->m_queue->ExecuteCommandLists(2, lists);
				}
				else
				{
					m_queue->m_queue->ExecuteCommandLists(1, (ID3D12CommandList**)m_li.GetAddressOf());
				}

				MutexGuard guard(m_queue->m_mtx);

				// Apply barrier changes to the global state.
				m_tracking_system.apply(m_queue->m_desc.type);

				guard.unlock();

				// Set fences.
				hr = m_fence->SetEventOnCompletion(m_wait_value, m_event);
				if (FAILED(hr))
				{
					return BasicError::bad_system_call();
				}
				hr = m_queue->m_queue->Signal(m_fence.Get(), m_wait_value);
				if (FAILED(hr))
				{
					return BasicError::bad_system_call();
				}

				// Release the context.
				m_frame_buffer = nullptr;

				return RV();
			}

			void CommandBuffer::draw_indexed_instanced(u32 index_count_per_instance, u32 instance_count, u32 start_index_location, i32 base_vertex_location, u32 start_instance_location)
			{
				lutsassert();
				m_li->DrawIndexedInstanced(index_count_per_instance, instance_count, start_index_location, base_vertex_location, start_instance_location);
			}

			void CommandBuffer::draw_instanced(u32 vertex_count_per_instance, u32 instance_count, u32 start_vertex_location,
				u32 start_instance_location)
			{
				lutsassert();
				m_li->DrawInstanced(vertex_count_per_instance, instance_count, start_vertex_location, start_instance_location);
			}

			void CommandBuffer::clear_depth_stencil_view(EClearFlag clear_flags, f32 depth, u8 stencil, u32 num_rects, const RectI* rects)
			{
				lutsassert();
				D3D12_CPU_DESCRIPTOR_HANDLE h = m_frame_buffer->get_dsv_cpu_handle();
				D3D12_RECT* d3drects = (D3D12_RECT*)alloca(sizeof(D3D12_RECT) * num_rects);
				for (u32 i = 0; i < num_rects; ++i)
				{
					d3drects[i].bottom = rects[i].bottom;
					d3drects[i].left = rects[i].left;
					d3drects[i].right = rects[i].right;
					d3drects[i].top = rects[i].top;
				}
				D3D12_CLEAR_FLAGS f;
				if ((clear_flags & EClearFlag::depth) != EClearFlag::none)
				{
					if ((clear_flags & EClearFlag::stencil) != EClearFlag::none)
					{
						f = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
					}
					else
					{
						f = D3D12_CLEAR_FLAG_DEPTH;
					}
				}
				else if ((clear_flags & EClearFlag::stencil) != EClearFlag::none)
				{
					f = D3D12_CLEAR_FLAG_STENCIL;
				}
				else
				{
					return;
				}
				m_li->ClearDepthStencilView(h, f, depth, stencil, num_rects, d3drects);
			}

			void CommandBuffer::clear_render_target_view(u32 index, const f32 color_rgba[4], u32 num_rects, const RectI* rects)
			{
				lutsassert();
				D3D12_CPU_DESCRIPTOR_HANDLE h = m_frame_buffer->get_rtv_cpu_handle(index);
				D3D12_RECT* d3drects = (D3D12_RECT*)alloca(sizeof(D3D12_RECT) * num_rects);
				for (u32 i = 0; i < num_rects; ++i)
				{
					d3drects[i].bottom = rects[i].bottom;
					d3drects[i].left = rects[i].left;
					d3drects[i].right = rects[i].right;
					d3drects[i].top = rects[i].top;
				}
				m_li->ClearRenderTargetView(h, color_rgba, num_rects, d3drects);
			}

			void CommandBuffer::end_render_pass()
			{
				lutsassert();
				m_render_pass = nullptr;
				m_frame_buffer = nullptr;
			}

			void CommandBuffer::copy_resource(IResource* dest, IResource* src)
			{
				lutsassert();
				lucheck(dest && src);
				m_li->CopyResource(static_cast<Resource*>(dest)->m_res.Get(), static_cast<Resource*>(src)->m_res.Get());
			}

			void CommandBuffer::copy_buffer_region(IResource* dest, u64 dest_offset, IResource* src, u64 src_offset, u64 num_bytes)
			{
				lutsassert();
				Resource* d = static_cast<Resource*>(dest);
				Resource* s = static_cast<Resource*>(src);
				m_li->CopyBufferRegion(d->m_res.Get(), dest_offset, s->m_res.Get(), src_offset, num_bytes);
			}

			void CommandBuffer::copy_texture_region(const TextureCopyLocation& dst, u32 dst_x, u32 dst_y, u32 dst_z, const TextureCopyLocation& src, const BoxU* src_box)
			{
				lutsassert();
				Resource* d = static_cast<Resource*>(dst.resource);
				Resource* s = static_cast<Resource*>(src.resource);
				D3D12_BOX* pb = nullptr;
				D3D12_BOX b;
				if (src_box)
				{
					b.left = src_box->left;
					b.right = src_box->right;
					b.top = src_box->top;
					b.bottom = src_box->bottom;
					b.front = src_box->front;
					b.back = src_box->back;
					pb = &b;
				}
				D3D12_TEXTURE_COPY_LOCATION desttex;
				D3D12_TEXTURE_COPY_LOCATION srctex;
				desttex.pResource = d->m_res.Get();
				if (dst.type == ETextureCopyType::placed_footprint)
				{
					desttex.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
					desttex.PlacedFootprint.Offset = dst.placed_footprint.offset;
					desttex.PlacedFootprint.Footprint.Format = encode_resource_format(dst.placed_footprint.footprint.format);
					desttex.PlacedFootprint.Footprint.Width = dst.placed_footprint.footprint.width;
					desttex.PlacedFootprint.Footprint.Height = dst.placed_footprint.footprint.height;
					desttex.PlacedFootprint.Footprint.Depth = dst.placed_footprint.footprint.depth;
					desttex.PlacedFootprint.Footprint.RowPitch = dst.placed_footprint.footprint.row_pitch;
				}
				else
				{
					desttex.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
					desttex.SubresourceIndex = dst.subresource_index;
				}
				srctex.pResource = s->m_res.Get();
				if (src.type == ETextureCopyType::placed_footprint)
				{
					srctex.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
					srctex.PlacedFootprint.Offset = src.placed_footprint.offset;
					srctex.PlacedFootprint.Footprint.Format = encode_resource_format(src.placed_footprint.footprint.format);
					srctex.PlacedFootprint.Footprint.Width = src.placed_footprint.footprint.width;
					srctex.PlacedFootprint.Footprint.Height = src.placed_footprint.footprint.height;
					srctex.PlacedFootprint.Footprint.Depth = src.placed_footprint.footprint.depth;
					srctex.PlacedFootprint.Footprint.RowPitch = src.placed_footprint.footprint.row_pitch;
				}
				else
				{
					srctex.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
					srctex.SubresourceIndex = src.subresource_index;
				}
				m_li->CopyTextureRegion(&desttex, dst_x, dst_y, dst_z, &srctex, pb);
			}

			void CommandBuffer::set_compute_shader_input_layout(IShaderInputLayout* shader_input_layout)
			{
				lutsassert();
				lucheck(shader_input_layout);
				ShaderInputLayout* o = static_cast<ShaderInputLayout*>(shader_input_layout);
				m_compute_shader_input_layout = o;
				m_li->SetComputeRootSignature(o->m_rs.Get());
			}

			void CommandBuffer::set_compute_view_set(IViewSet* view_set)
			{
				lutsassert();
				lucheck(view_set);
				lucheck_msg(m_compute_shader_input_layout, "Compute Shader Input Layout must be set before Compute View Set can be attached.");

				m_view_set = view_set;
				ID3D12DescriptorHeap* heaps[2];
				u32 num_heaps = 0;
				if (m_view_set->m_cbv_srv_uav_heap)
				{
					heaps[num_heaps] = m_view_set->m_cbv_srv_uav_heap.Get();
					++num_heaps;
				}
				if (m_view_set->m_sampler_heap)
				{
					heaps[num_heaps] = m_view_set->m_sampler_heap.Get();
					++num_heaps;
				}
				m_li->SetDescriptorHeaps(num_heaps, heaps);

				for (u32 i = 0; i < m_compute_shader_input_layout->m_groups.size(); ++i)
				{
					auto& group = m_compute_shader_input_layout->m_groups[i];
					D3D12_GPU_DESCRIPTOR_HANDLE h;
					switch (group.type)
					{
					case EShaderInputGroupType::uav:
						h = m_view_set->m_cbv_srv_uav_heap->GetGPUDescriptorHandleForHeapStart();
						h.ptr += m_view_set->m_cbv_srv_uav_size * (group.base_shader_register + m_view_set->m_cbs.size() + m_view_set->m_srvs.size());
						break;
					case EShaderInputGroupType::cbv:
						h = m_view_set->m_cbv_srv_uav_heap->GetGPUDescriptorHandleForHeapStart();
						h.ptr += m_view_set->m_cbv_srv_uav_size * group.base_shader_register;
						break;
					case EShaderInputGroupType::srv:
						h = m_view_set->m_cbv_srv_uav_heap->GetGPUDescriptorHandleForHeapStart();
						h.ptr += m_view_set->m_cbv_srv_uav_size * (group.base_shader_register + m_view_set->m_cbs.size());
						break;
					case EShaderInputGroupType::sampler:
						h = m_view_set->m_sampler_heap->GetGPUDescriptorHandleForHeapStart();
						h.ptr += m_view_set->m_sampler_size * group.base_shader_register;
						break;
					default:
						lupanic();
						break;
					}
					m_li->SetComputeRootDescriptorTable(i, h);
				}
			}

			void CommandBuffer::resource_barrier(const ResourceBarrierDesc& barrier)
			{
				lutsassert();
				m_tracking_system.begin_new_transition_batch();
				m_tracking_system.pack_barrier(barrier);
				if (!m_tracking_system.m_barriers.empty())
				{
					m_li->ResourceBarrier((UINT)m_tracking_system.m_barriers.size(), m_tracking_system.m_barriers.data());
				}
			}

			void CommandBuffer::resource_barriers(u32 num_barriers, const ResourceBarrierDesc* barriers)
			{
				lutsassert();
				m_tracking_system.begin_new_transition_batch();
				for (u32 i = 0; i < num_barriers; ++i)
				{
					m_tracking_system.pack_barrier(barriers[i]);
				}
				if (!m_tracking_system.m_barriers.empty())
				{
					m_li->ResourceBarrier((UINT)m_tracking_system.m_barriers.size(), m_tracking_system.m_barriers.data());
				}
			}

			void CommandBuffer::dispatch(u32 thread_group_count_x, u32 thread_group_count_y, u32 thread_group_count_z)
			{
				lutsassert();
				m_li->Dispatch(thread_group_count_x, thread_group_count_y, thread_group_count_z);
			}
		}
	}
}

#endif