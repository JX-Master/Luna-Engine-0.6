// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file CommandBuffer.hpp
* @author JXMaster
* @date 2020/3/11
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12
#include "CommandQueue.hpp"
#include <Runtime/TSAssert.hpp>
#include <Runtime/HashMap.hpp>
#include "Resource.hpp"
#include "RenderPass.hpp"
#include "FrameBuffer.hpp"
#include "ViewSet.hpp"
#include "PipelineState.hpp"
#include "ShaderInputLayout.hpp"

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			struct ResourceKey
			{
				Resource* m_res;
				u32 m_subres;
			};

			inline bool operator==(const ResourceKey& lhs, const ResourceKey& rhs)
			{
				return (lhs.m_res == rhs.m_res) && (lhs.m_subres == rhs.m_subres);
			}

		}
	}

	template<>
	struct hash<Gfx::D3D12::ResourceKey>
	{
		usize operator()(const Gfx::D3D12::ResourceKey& key)
		{
			return memhash<usize>(&key, sizeof(Gfx::D3D12::ResourceKey));
		}
	};

	namespace Gfx
	{
		namespace D3D12
		{
			// Only for Non-Simultaneous-Access Textures.
			inline bool is_texture_decayable_to_common(EResourceState s)
			{
				/*if (s == EResourceState::common ||
					s == EResourceState::shader_resource_non_pixel ||
					s == EResourceState::shader_resource_pixel ||
					s == EResourceState::copy_source)
				{
					return true;
				}*/
				return false;
			}

			class ResourceStateTrackingSystem
			{
			public:
				//! One table for unresolved resources. Unlike most implementations in other library, because
				//! we don't know when the list will be submitted to the queue, we defer the resolving of this 
				//! to the time when the list is actually submitted.
				HashMap<ResourceKey, EResourceState> m_unresloved;
				//! One table for the current state of resources.
				HashMap<ResourceKey, EResourceState> m_current;

				Vector<D3D12_RESOURCE_BARRIER> m_barriers;

				ResourceStateTrackingSystem() {}

			private:
				//! Translates one transition into the D3D12 transition barrier, and ignores the implicit promotion and decay.
				void append_transition(Resource* res, u32 subresource, EResourceState before, EResourceState after, EResourceBarrierFlag flags);

			public:

				void reset()
				{
					m_unresloved.clear();
					m_current.clear();
				}

				void begin_new_transition_batch()
				{
					m_barriers.clear();
				}

				R<EResourceState> get_state(Resource* res, u32 subresource)
				{
					ResourceKey k;
					k.m_res = res;
					k.m_subres = subresource;
					auto iter = m_current.find(k);
					if (iter == m_current.end())
					{
						return BasicError::not_found();
					}
					return iter->second;
				}

				//! Appends one barrier that transits the specified subresources' state to after
				//! state, and records the change into the tracking system.
				//! 
				//! If any of `begin_only` and `end_only` flag is specified: 
				//! 1. If the specified subresource is not resolved, the `begin_only` call is ignored, and the `end_only`
				//!    call will be converted to a full call (flag `end_only` dropped).
				//! 2. If the specified subresource has been resolved by previous states, both `begin_only` and `end_only`
				//!	   calls will be recorded, but the changes will only be applied to the tracking system when `end_only`
				//!	   is called.
				void pack_transition(Resource* res, u32 subresource, EResourceState after, EResourceBarrierFlag flags);

				//! Appends any barrier.
				void pack_barrier(const ResourceBarrierDesc& desc)
				{
					switch (desc.type)
					{
					case EResourceBarrierType::transition:
					{
						Resource* res = const_cast<Resource*>(static_cast<const Resource*>(desc.transition.resource));
						pack_transition(res, desc.transition.subresource, desc.transition.after, desc.flags);
						return;
					}
					case EResourceBarrierType::aliasing:
					{
						if (desc.aliasing.after)
						{
							D3D12_RESOURCE_BARRIER ba;
							ba.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
							if (desc.aliasing.before)
							{
								ba.Aliasing.pResourceBefore = static_cast<const Resource*>(desc.aliasing.before)->m_res.Get();
							}
							else
							{
								ba.Aliasing.pResourceBefore = nullptr;
							}
							ba.Aliasing.pResourceAfter = static_cast<const Resource*>(desc.aliasing.after)->m_res.Get();
							m_barriers.push_back(ba);
						}
					}
					case EResourceBarrierType::uav:
					{
						D3D12_RESOURCE_BARRIER ba;
						ba.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
						ba.UAV.pResource = static_cast<Resource*>(desc.uav.resource)->m_res.Get();
						m_barriers.push_back(ba);
					}
					default:
						lupanic();
						break;
					}
				}

				//! Resolves all unresolved transitions into m_transitions based on their current state.
				void resolve();

				//! Applies all after state back to the resource global state.
				void apply(ECommandQueueType type);
			};

			class CommandBuffer : public ICommandBuffer
			{
			public:
				lucid("{2aa94bb6-f36d-4aa2-826b-3076026c2cec}");
				luqbegin();
				luqitem(this, CommandBuffer, ICommandBuffer, IWaitable);
				luqitem((IGraphicDeviceChild*)this, IGraphicDeviceChild, IObject);
				luqend();
				lurc();
				lutsassert_lock();

				P<GraphicDevice> m_device;
				P<CommandQueue> m_queue;

				// Resource Tracking System.
				ResourceStateTrackingSystem m_tracking_system;
				
				ComPtr<ID3D12CommandAllocator> m_ca;
				ComPtr<ID3D12GraphicsCommandList> m_li;

				//! The fence used for wait/set from GPU 
				ComPtr<ID3D12Fence> m_fence;
				//! The event used for wait from CPU.
				HANDLE m_event;
				//! The next value to wait for by CPU/GPU.
				u64 m_wait_value;

				//! Checks if the command list is closed.
				bool m_cmdlist_closed;

				// Render Contexts.

				//! The current set render pass.
				P<RenderPass> m_render_pass;
				//! The current set frame buffer.
				P<FrameBuffer> m_frame_buffer;
				//! The current set vertex buffer.
				Vector<VertexBufferViewDesc> m_vbs;
				//! The current bound index buffer.
				P<Resource> m_ib;
				//! The current bound view set.
				P<ViewSet> m_view_set;
				//! The current bound pipeline state.
				P<PipelineState> m_pipeline_state;

				//! The current bound graphic shader input layout.
				P<ShaderInputLayout> m_graphic_shader_input_layout;

				//! The current bound compute shader input layout.
				P<ShaderInputLayout> m_compute_shader_input_layout;

				//! The attached graphic objects.
				Vector<P<IGraphicDeviceChild>> m_objs;

				CommandBuffer() :
					m_event(NULL),
					m_cmdlist_closed(false) {}

				~CommandBuffer()
				{
					if (m_event)
					{
						::CloseHandle(m_event);
						m_event = NULL;
					}
				}

				RV init();

				virtual void wait() override
				{
					DWORD res = ::WaitForSingleObject(m_event, INFINITE);
					if (res != WAIT_OBJECT_0)
					{
						lupanic_msg_always("WaitForSingleObject failed.");
					}
				}
				virtual RV try_wait() override
				{
					DWORD res = ::WaitForSingleObject(m_event, 0);
					if (res == WAIT_OBJECT_0)
					{
						return RV();
					}
					if (res == WAIT_TIMEOUT)
					{
						return BasicError::timeout();
					}
					return BasicError::bad_system_call();
				}

				virtual IGraphicDevice* get_device() override
				{
					return m_device;
				}

				virtual ECommandQueueType type() override
				{
					return m_queue->m_desc.type;
				}

				virtual RV reset() override;
				virtual void attach_graphic_object(IGraphicDeviceChild* obj) override
				{
					m_objs.push_back(obj);
				}
				virtual void begin_render_pass(IRenderPass* render_pass, IFrameBuffer* fbo, u32 num_rt_clear_values, const Float4U* clear_values,
					f32 depth_clear_value, u8 stencil_clear_value) override;
				virtual void set_pipeline_state(IPipelineState* pso) override;
				virtual void set_graphic_shader_input_layout(IShaderInputLayout* shader_input_layout) override;
				virtual void set_vertex_buffers(u32 start_slot, u32 num_views, const VertexBufferViewDesc* views) override;
				virtual void set_index_buffer(IResource* buffer, u32 offset_in_bytes, u32 size_in_bytes, EResourceFormat format) override;
				virtual void set_graphic_view_set(IViewSet* view_set) override;
				virtual void set_primitive_topology(EPrimitiveTopology primitive_topology) override;
				virtual void set_stream_output_targets(u32 start_slot, u32 num_views, const StreamOutputBufferView* views) override;
				virtual void set_viewport(const Viewport& viewport) override
				{
					set_viewports(1, &viewport);
				}
				virtual void set_viewports(u32 num_viewports, const Viewport* viewports) override;
				virtual void set_scissor_rect(const RectI& rects) override
				{
					return set_scissor_rects(1, &rects);
				}
				virtual void set_scissor_rects(u32 num_rects, const RectI* rects) override;
				virtual void set_blend_factor(const f32 blend_factor[4]) override;
				virtual void set_stencil_ref(u32 stencil_ref) override;
				virtual void draw(u32 vertex_count, u32 start_vertex_location) override
				{
					draw_instanced(vertex_count, 1, start_vertex_location, 0);
				}
				virtual void draw_indexed(u32 index_count, u32 start_index_location, i32 base_vertex_location) override
				{
					draw_indexed_instanced(index_count, 1, start_index_location, base_vertex_location, 0);
				}
				virtual void draw_indexed_instanced(u32 index_count_per_instance, u32 instance_count, u32 start_index_location,
					i32 base_vertex_location, u32 start_instance_location) override;
				virtual void draw_instanced(u32 vertex_count_per_instance, u32 instance_count, u32 start_vertex_location,
					u32 start_instance_location) override;
				virtual void clear_depth_stencil_view(EClearFlag clear_flags, f32 depth, u8 stencil, u32 num_rects, const RectI* rects) override;
				virtual void clear_render_target_view(u32 index, const f32 color_rgba[4], u32 num_rects, const RectI* rects) override;
				virtual void end_render_pass() override;
				virtual void copy_resource(IResource* dest, IResource* src) override;
				virtual void copy_buffer_region(IResource* dest, u64 dest_offset, IResource* src, u64 src_offset, u64 num_bytes) override;
				virtual void copy_texture_region(const TextureCopyLocation& dst, u32 dst_x, u32 dst_y, u32 dst_z,
					const TextureCopyLocation& src, const BoxU* src_box = nullptr) override;
				virtual void set_compute_shader_input_layout(IShaderInputLayout* shader_input_layout) override;
				virtual void set_compute_view_set(IViewSet* view_set) override;
				virtual void resource_barrier(const ResourceBarrierDesc& barrier) override;
				virtual void resource_barriers(u32 num_barriers, const ResourceBarrierDesc* barriers) override;
				virtual void dispatch(u32 thread_group_count_x, u32 thread_group_count_y, u32 thread_group_count_z) override;
				virtual RV submit() override;
			};
		}
	}
}

#endif