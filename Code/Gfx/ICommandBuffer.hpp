// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ICommandBuffer.hpp
* @author JXMaster
* @date 2020/3/10
*/
#pragma once
#include <Base/IObject.hpp>
#include "IPipelineState.hpp"
#include "IFrameBuffer.hpp"
#include <Base/Math.hpp>
#include "IViewSet.hpp"
#include "IRenderPass.hpp"
#include "IShaderInputLayout.hpp"

namespace luna
{
	namespace gfx
	{
		enum class ECommandQueueType : uint32
		{
			graphic = 1,
			compute = 2,
			copy = 3,
		};

		enum class EClearFlag : uint32
		{
			none = 0x00,
			depth = 0x01,
			stencil = 0x02
		};

		enum class ETextureCopyType : uint32
		{
			subresource_index,	// When the referencing resource is a texture.
			placed_footprint		// When the referencing resource is a buffer.
		};

		struct SubresourceFootprint
		{
			EResourceFormat format;
			uint32 width;
			uint32 height;
			uint32 depth;
			uint32 row_pitch;
		};

		struct PlacedResourceFootprint
		{
			uint64 offset;
			SubresourceFootprint footprint;
		};

		struct TextureCopyLocation
		{
			IResource* resource;
			ETextureCopyType type;
			union
			{
				PlacedResourceFootprint placed_footprint;
				uint32 subresource_index;
			};

			static TextureCopyLocation as_placed_foorprint(IResource* _resource, uint64 _offset, EResourceFormat _format, uint32 _width, uint32 _height, uint32 _depth, uint32 _row_pitch)
			{
				TextureCopyLocation r;
				r.type = ETextureCopyType::placed_footprint;
				r.resource = _resource;
				r.placed_footprint.offset = _offset;
				r.placed_footprint.footprint.format = _format;
				r.placed_footprint.footprint.width = _width;
				r.placed_footprint.footprint.height = _height;
				r.placed_footprint.footprint.depth = _depth;
				r.placed_footprint.footprint.row_pitch = _row_pitch;
				return r;
			}

			static TextureCopyLocation as_subresource_index(IResource* _resource, uint32 _subresource_index)
			{
				TextureCopyLocation r;
				r.type = ETextureCopyType::subresource_index;
				r.resource = _resource;
				r.subresource_index = _subresource_index;
				return r;
			}
		};

		enum class EResourceBarrierType : uint32
		{
			//! Translates the state of the resource to a new state. In most of the time you don't need to call this 
			//! manually, since the resource state tracking system built in Gfx tracks the resource state and inserts
			//! proper barriers for you, however you can explicitly specify this if you need the resource to be in a 
			//! specific state that cannot be converted automatically by system, or you can explicitly specify this
			//! to do some optimizations.
			transition = 1,
			//! Issues an aliasing barrier when a new resource whose memory overlaps with the old resource is about
			//! to be access by the graphic pipeline. This call inserts a memory barrier so that the access to the new
			//! resource is deferred until all access to the old resource finished to prevent race condition.
			aliasing = 2,
			//! Issues an uav barrier that makes the previous dispatch call to access the specified resource finishes 
			//! before the next dispatch call that access the resource can start. This this to prevent race condition
			//! between multiple dispatch calls.
			uav = 3
		};

		enum class EResourceBarrierFlag : uint32
		{
			none = 0x00,
			begin_only = 0x01,
			end_only = 0x02,
		};

		constexpr uint32 resource_barrier_all_subresources_v = uint32_max_v;
		struct IResource;

		struct ResourceTransitionBarrierDesc
		{
			IResource* resource;
			uint32 subresource;
			EResourceState after;
		};

		struct ResourceUAVBarrierDesc
		{
			IResource* resource;
		};

		struct ResourceAliasingBarrierDesc
		{
			IResource* before;
			IResource* after;
		};

		struct ResourceBarrierDesc
		{
			EResourceBarrierType type;
			EResourceBarrierFlag flags;
			union
			{
				ResourceTransitionBarrierDesc transition;
				ResourceAliasingBarrierDesc aliasing;
				ResourceUAVBarrierDesc uav;
			};

			static ResourceBarrierDesc as_transition(IResource* resource, EResourceState after, uint32 subresource = resource_barrier_all_subresources_v, EResourceBarrierFlag flags = EResourceBarrierFlag::none)
			{
				ResourceBarrierDesc r;
				r.type = EResourceBarrierType::transition;
				r.transition.resource = resource;
				r.transition.subresource = subresource;
				r.transition.after = after;
				r.flags = flags;
				return r;
			}

			static ResourceBarrierDesc as_aliasing(IResource* before, IResource* after, EResourceBarrierFlag flags = EResourceBarrierFlag::none)
			{
				ResourceBarrierDesc r;
				r.type = EResourceBarrierType::aliasing;
				r.aliasing.before = before;
				r.aliasing.after = after;
				r.flags = flags;
				return r;
			}

			static ResourceBarrierDesc as_uav(IResource* resource, EResourceBarrierFlag flags = EResourceBarrierFlag::none)
			{
				ResourceBarrierDesc r;
				r.type = EResourceBarrierType::uav;
				r.uav.resource = resource;
				r.flags = flags;
				return r;
			}

		};

		//! @interface ICommandBuffer
		//! The command buffer is used to store and record commands that can be submitted
		//! to the command queue for execution by GPU. The memory for the command buffer is
		//! reused by resetting the command buffer so it is efficient to record commands.
		//! 
		//! One command buffer is created by calling `new_command_buffer` of the `ICommandQueue`
		//! interface, and the command buffer is attached to the command queue and can only be
		//! executed by that queue. Like command queue, there are also three types of command 
		//! buffer: Graphic, Compute and Copy, while Compute and Copy command buffer only supports
		//! a subset of the commands that can be recorded in.
		//! 
		//! Command buffer is not thread safe and should only be assigned to one thread. Create multiple
		//! command buffers if you want to record commands by multiple threads.
		struct ICommandBuffer : public IGraphicDeviceChild, public IWaitable
		{
			luiid("{2970a4c8-d905-4e58-9247-46ba6a33b220}");
			luiprimary(IGraphicDeviceChild);

			virtual ECommandQueueType type() = 0;

			//! Reset the command buffer. This call clears all commands in the command buffer and reopens
			//! the command buffer for recording new commands.
			//! 
			//! You should only call this after the command buffer has finished execution by the command queue,
			//! or the behavior is undefined. In order to make sure all commands are executed by GPU, call
			//! `wait` to block the thread until this buffer gets finished, or you can use `try_wait` to test
			//! if the buffer has finished execution. Calling `wait` and `try_wait` before the buffer is submitted
			//! cause `e_bad_calling_time` failure.
			virtual RV reset() = 0;

			//! Attaches one graphic device object to this command buffer. The command buffer keeps a strong reference 
			//! to the object until the next `reset` is called.
			//! This is mainly used to keep references to the graphic objects used by the current command buffer, so they
			//! will not be released before GPU finishes accessing them.
			virtual void attach_graphic_object(IGraphicDeviceChild* obj) = 0;

			//! Starts a new render pass. The previous render pass should be closed before beginning another one.
			//! @param[in] render_pass The render pass object to apply to the render pass. 
			//! Only one pipeline state should be applied to one render pass, if you need to change pso, you
			//! need to make a new render pass.
			//! @param[in] fbo The frame buffer object bound as the render target of the render pass.
			//! Only one frame buffer object should be applied to one render pass, if you need to change fbo, you
			//! need to make a new render pass.
			//! @param[in] num_rt_clear_values The number of clear values used for render target.
			//! @param[in] clear_values An array of clear values used for the render targets. Only render targets that
			//! specify `EAttachmentLoadOp::clear` will be cleared when the render pass begins.
			//! For example, if we have 4 render target attachments and only attachments at 0, 2 sets `EAttachmentLoadOp::clear`,
			//! then only clear values at index 0 and 2 will be used.
			//! @param[in] depth_clear_value The depth value to use for clear if `depth_load_op` specified in render pass
			//! is `EAttachmentLoadOp::clear`.
			//! @param[in] stencil_clear_value The stencil value to use for clear if `stencil_load_op` specified in render pass
			//! is `EAttachmentLoadOp::clear`.
			//! @remark In order to let EAttachmentLoadOp::clear works, the render textures that need to be cleared must be in
			//! `EResourceState::render_target` state and the depth stencil texture that needs to be cleared must be in 
			//! `EResourceState::depth_write` state.
			//! 
			//! The following functions can only be called in between `begin_render_pass` and `end_render_pass`:
			//! * draw
			//! * draw_indexed
			//! * draw_instanced
			//! * draw_indexed_instanced
			//! * clear_render_target_view
			//! * clear_depth_stencil_view
			//! The following functions can only be called outside of one render pass range:
			//! * submit
			virtual void begin_render_pass(IRenderPass* render_pass, IFrameBuffer* fbo, uint32 num_rt_clear_values, const Float4U* clear_values,
				float32 depth_clear_value, uint8 stencil_clear_value) = 0;

			//! Sets the graphic or compute pipeline state.
			virtual void set_pipeline_state(IPipelineState* pso) = 0;

			//! Sets the graphic shader input layout.
			virtual void set_graphic_shader_input_layout(IShaderInputLayout* shader_input_layout) = 0;

			//! Sets vertex buffers.
			virtual void set_vertex_buffers(uint32 start_slot, uint32 num_views, const VertexBufferViewDesc* views) = 0;

			//! Sets index buffer.
			virtual void set_index_buffer(IResource* buffer, uint32 offset_in_bytes, uint32 size_in_bytes, EResourceFormat format) = 0;

			//! Sets the view set to be used by the graphic pipeline.
			//! This must be called after `set_pipeline_state`.
			virtual void set_graphic_view_set(IViewSet* view_set) = 0;

			//! Bind information about the primitive type, and data order that describes input data for the input assembler stage.
			virtual void set_primitive_topology(EPrimitiveTopology primitive_topology) = 0;

			//! Sets the stream output buffer views. 
			virtual void set_stream_output_targets(uint32 start_slot, uint32 num_views, const StreamOutputBufferView* views) = 0;

			//! Bind one viewport to the rasterizer stage of the pipeline.
			virtual void set_viewport(const Viewport& viewport) = 0;

			//! Bind an array of viewports to the rasterizer stage of the pipeline.
			virtual void set_viewports(uint32 num_viewports, const Viewport* viewports) = 0;

			//! Binds one scissor rectangle to the rasterizer stage.
			virtual void set_scissor_rect(const RectI& rects) = 0;

			//! Binds an array of scissor rectangles to the rasterizer stage.
			virtual void set_scissor_rects(uint32 num_rects, const RectI* rects) = 0;

			//! Sets the blend factor that modulate values for a pixel shader, render target, or both.
			virtual void set_blend_factor(const float32 blend_factor[4]) = 0;

			//! Sets the reference value for depth stencil tests.
			virtual void set_stencil_ref(uint32 stencil_ref) = 0;

			//! Draw primitives.
			virtual void draw(uint32 vertex_count, uint32 start_vertex_location) = 0;

			//! Draw indexed primitives.
			virtual void draw_indexed(uint32 index_count, uint32 start_index_location, int32 base_vertex_location) = 0;

			//! Draws indexed, instanced primitives.
			virtual void draw_indexed_instanced(uint32 index_count_per_instance, uint32 instance_count, uint32 start_index_location,
				int32 base_vertex_location, uint32 start_instance_location) = 0;

			//! Draws non-indexed, instanced primitives.
			virtual void draw_instanced(uint32 vertex_count_per_instance, uint32 instance_count, uint32 start_vertex_location,
				uint32 start_instance_location) = 0;

			//! Clears the depth stencil view bound to the current render pass.
			virtual void clear_depth_stencil_view(EClearFlag clear_flags, float32 depth, uint8 stencil, uint32 num_rects, const RectI* rects) = 0;

			//! Clears the render target view bound to the current render pass.
			//! @param[in] index The index of the render target view to clear in the frame buffers.
			virtual void clear_render_target_view(uint32 index, const float32 color_rgba[4], uint32 num_rects, const RectI* rects) = 0;

			//! Finishes the current render pass.
			virtual void end_render_pass() = 0;

			//! Copies the entire contents of the source resource to the destination resource.
			virtual void copy_resource(IResource* dest, IResource* src) = 0;

			//! Copies a region of a buffer from one resource to another.
			virtual void copy_buffer_region(IResource* dest, uint64 dest_offset, IResource* src, uint64 src_offset, uint64 num_bytes) = 0;

			//! This method uses the GPU to copy texture data between two locations. 
			virtual void copy_texture_region(const TextureCopyLocation& dst, uint32 dst_x, uint32 dst_y, uint32 dst_z,
				const TextureCopyLocation& src, const BoxU* src_box = nullptr) = 0;

			//! Sets the compute shader input layout.
			virtual void set_compute_shader_input_layout(IShaderInputLayout* shader_input_layout) = 0;

			//! Sets the view set to be used by the compute pipeline.
			//! This must be called after `set_pipeline_state`.
			virtual void set_compute_view_set(IViewSet* view_set) = 0;

			//! Issues one resource barrier.
			virtual void resource_barrier(const ResourceBarrierDesc& barriers) = 0;

			//! Issues resource barriers.
			virtual void resource_barriers(uint32 num_barriers, const ResourceBarrierDesc* barriers) = 0;

			//! Executes a command list from a thread group.
			virtual void dispatch(uint32 thread_group_count_x, uint32 thread_group_count_y, uint32 thread_group_count_z) = 0;

			//! Submits the recorded content in this command buffer to the target command queue.
			//! The command buffer can only be submitted once, and the only operation after the submit is to 
			//! reset the command buffer after it is executed by command queue.
			virtual RV submit() = 0;
		};
	}
}