// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IPipelineState.hpp
* @author JXMaster
* @date 2019/7/20
*/
#pragma once
#include "IGraphicDeviceChild.hpp"
#include "GraphicDefines.hpp"

namespace luna
{
	namespace gfx
	{
		struct ShaderBytecode
		{
			const void* code;
			size_t length;

			ShaderBytecode() = default;
			ShaderBytecode(const void* _code, size_t _length) :
				code(_code), length(_length) {}
			ShaderBytecode(Default) :
				ShaderBytecode(nullptr, 0) {}
		};

		struct StreamOutputDeclarationEntry
		{
			uint32 stream;
			const char* semantic_name;
			uint32 semantic_index;
			byte_t start_component;
			byte_t component_count;
			byte_t output_slot;

			StreamOutputDeclarationEntry() = default;
			StreamOutputDeclarationEntry(uint32 _stream,
				const char* _semantic_name,
				uint32 _semantic_index,
				byte_t _start_component,
				byte_t _component_count,
				byte_t _output_slot) :
				stream(_stream),
				semantic_name(_semantic_name),
				semantic_index(_semantic_index),
				start_component(_start_component),
				component_count(_component_count),
				output_slot(_output_slot) {}
		};

		struct StreamOutputDesc
		{
			const StreamOutputDeclarationEntry* entries;
			uint32 num_entries;
			const uint32* buffer_strides;
			uint32 num_strides;
			uint32 rasterized_stream;

			StreamOutputDesc() = default;
			StreamOutputDesc(const StreamOutputDeclarationEntry* _entries,
				uint32 _num_entries,
				const uint32* _buffer_strides,
				uint32 _num_strides,
				uint32 _rasterized_stream) :
				entries(_entries),
				num_entries(_num_entries),
				buffer_strides(_buffer_strides),
				num_strides(_num_strides),
				rasterized_stream(_rasterized_stream) {}
			StreamOutputDesc(Default) :
				StreamOutputDesc(nullptr, 0, nullptr, 0, 0) {}
		};

		enum class EBlendFactor : uint32
		{
			zero,
			one,
			src_color,
			inv_src_color,
			src_alpha,
			inv_src_alpha,
			dest_alpha,
			inv_dest_alpha,
			dest_color,
			inv_dest_color,
			src_alpha_sat,
			blend_factor,
			inv_blend_factor,
			src1_color,
			inv_src1_color,
			src1_alpha,
			inv_src1_alpha
		};
		enum class EBlendOp : uint32
		{
			add,
			subtract,
			rev_subtract,
			min,
			max
		};
		enum class ELogicOp : uint32
		{
			clear,
			set,
			copy,
			copy_inverted,
			noop,
			invert,
			and,
			nand,
			or,
			nor,
			xor,
			equiv,
			and_reverse,
			and_inverted,
			or_reverse,
			or_inverted
		};
		enum class EColorWriteMask : uint8
		{
			none = 0x00,
			red = 1,
			green = 2,
			blue = 4,
			alpha = 8,
			all = red | green | blue | alpha
		};

		struct RenderTargetBlendDesc
		{
			bool blend_enable;
			bool logic_op_enable;
			EBlendFactor src_blend;
			EBlendFactor dest_blend;
			EBlendOp blend_op;
			EBlendFactor src_blend_alpha;
			EBlendFactor dest_blend_alpha;
			EBlendOp blend_op_alpha;
			ELogicOp logic_op;
			EColorWriteMask render_target_write_mask;

			RenderTargetBlendDesc() = default;
			RenderTargetBlendDesc(
				bool _blend_enable,
				bool _logic_op_enable,
				EBlendFactor _src_blend,
				EBlendFactor _dest_blend,
				EBlendOp _blend_op,
				EBlendFactor _src_blend_alpha,
				EBlendFactor _dest_blend_alpha,
				EBlendOp _blend_op_alpha,
				ELogicOp _logic_op,
				EColorWriteMask _render_target_write_mask
			) :
				blend_enable(_blend_enable),
				logic_op_enable(_logic_op_enable),
				src_blend(_src_blend),
				dest_blend(_dest_blend),
				blend_op(_blend_op),
				src_blend_alpha(_src_blend_alpha),
				dest_blend_alpha(_dest_blend_alpha),
				blend_op_alpha(_blend_op_alpha),
				logic_op(_logic_op),
				render_target_write_mask(_render_target_write_mask) {}
			RenderTargetBlendDesc(Default) :
				RenderTargetBlendDesc(false, false,
					EBlendFactor::one, EBlendFactor::zero, EBlendOp::add,
					EBlendFactor::one, EBlendFactor::zero, EBlendOp::add,
					ELogicOp::noop, EColorWriteMask::all) {}
		};

		struct BlendDesc
		{
			bool alpha_to_coverage_enable;
			bool independent_blend_enable;
			RenderTargetBlendDesc rt[8];

			BlendDesc() = default;
			BlendDesc(
				bool _alpha_to_coverage_enable,
				bool _independent_blend_enable,
				std::initializer_list<RenderTargetBlendDesc> _rt) :
				alpha_to_coverage_enable(_alpha_to_coverage_enable),
				independent_blend_enable(_independent_blend_enable)
			{
				uint32 i = 0;
				for (auto& it : _rt)
				{
					rt[i] = it;
					++i;
				}
				while (i < 8)
				{
					rt[i] = RenderTargetBlendDesc(Default());
					++i;
				}
			}
			BlendDesc(Default) :
				BlendDesc(false, false, {}) {}
		};

		enum class EFillMode : uint32
		{
			wireframe,
			solid
		};

		enum class ECullMode : uint32
		{
			none,
			front,
			back
		};

		struct RasterizerDesc
		{
			EFillMode fill_mode;
			ECullMode cull_mode;
			int32 depth_bias;
			float32 depth_bias_clamp;
			float32 slope_scaled_depth_bias;
			uint32 forced_sample_count;
			bool front_counter_clockwise;
			bool depth_clip_enable;
			bool multisample_enable;
			bool antialiased_line_enable;
			bool conservative_raster_enabled;

			RasterizerDesc() = default;
			RasterizerDesc(
				EFillMode _fill_mode,
				ECullMode _cull_mode,
				int32 _depth_bias,
				float32 _depth_bias_clamp,
				float32 _slope_scaled_depth_bias,
				uint32 _forced_sample_count,
				bool _front_counter_clockwise,
				bool _depth_clip_enable,
				bool _multisample_enable,
				bool _antialiased_line_enable,
				bool _conservative_raster_enabled
			) :
				fill_mode(_fill_mode),
				cull_mode(_cull_mode),
				depth_bias(_depth_bias),
				depth_bias_clamp(_depth_bias_clamp),
				slope_scaled_depth_bias(_slope_scaled_depth_bias),
				forced_sample_count(_forced_sample_count),
				front_counter_clockwise(_front_counter_clockwise),
				depth_clip_enable(_depth_clip_enable),
				multisample_enable(_multisample_enable),
				antialiased_line_enable(_antialiased_line_enable),
				conservative_raster_enabled(_conservative_raster_enabled) {}
			RasterizerDesc(Default) :
				RasterizerDesc(
					EFillMode::solid, 
					ECullMode::back, 
					0, 0.0f, 0.0f, 0, 
					false, true, false, false, false) {}
		};

		enum class EStencilOp : uint32
		{
			keep,
			zero,
			replace,
			incr_sat,
			decr_sat,
			invert,
			incr,
			decr
		};

		struct DepthStencilOpDesc
		{
			EStencilOp stencil_fail_op;
			EStencilOp stencil_depth_fail_op;
			EStencilOp stencil_pass_op;
			EComparisonFunc stencil_func;

			DepthStencilOpDesc() = default;
			DepthStencilOpDesc(
				EStencilOp _stencil_fail_op,
				EStencilOp _stencil_depth_fail_op,
				EStencilOp _stencil_pass_op,
				EComparisonFunc _stencil_func
			) :
				stencil_fail_op(_stencil_fail_op),
				stencil_depth_fail_op(_stencil_depth_fail_op),
				stencil_pass_op(_stencil_pass_op),
				stencil_func(_stencil_func) {}
			DepthStencilOpDesc(Default) :
				DepthStencilOpDesc(
					EStencilOp::keep, 
					EStencilOp::keep, 
					EStencilOp::keep, 
					EComparisonFunc::always) {}
		};

		constexpr uint8 l_default_stencil_read_mask = 0xff;
		constexpr uint8 l_default_stencil_write_mask = 0xff;

		struct DepthStencilDesc
		{
			bool depth_test_enable;
			bool depth_write_enable;
			EComparisonFunc depth_func;
			bool stencil_enable;
			uint8 stencil_read_mask;
			uint8 stencil_write_mask;
			DepthStencilOpDesc front_face;
			DepthStencilOpDesc back_face;

			DepthStencilDesc() = default;
			DepthStencilDesc(
				bool _depth_test_enable,
				bool _depth_write_enable,
				EComparisonFunc _depth_func,
				bool _stencil_enable,
				uint8 _stencil_read_mask,
				uint8 _stencil_write_mask,
				const DepthStencilOpDesc& _front_face,
				const DepthStencilOpDesc& _back_face
			) :
				depth_test_enable(_depth_test_enable),
				depth_write_enable(_depth_write_enable),
				depth_func(_depth_func),
				stencil_enable(_stencil_enable),
				stencil_read_mask(_stencil_read_mask),
				stencil_write_mask(_stencil_write_mask),
				front_face(_front_face),
				back_face(_back_face) {}
			DepthStencilDesc(Default) :
				DepthStencilDesc(
					true,
					true, 
					EComparisonFunc::less, 
					false, 
					l_default_stencil_read_mask, 
					l_default_stencil_write_mask, 
					DepthStencilOpDesc(Default()),
					DepthStencilOpDesc(Default())) {}
		};

		enum class EInputClassification : uint32
		{
			per_vertex = 1,
			per_instance = 2
		};

		constexpr uint32 l_append_aligned_element = 0xffffffff;

		struct InputElementDesc
		{
			const char* semantic_name;
			uint32 semantic_index;
			EResourceFormat format;
			uint32 input_slot;
			uint32 aligned_byte_offset;
			EInputClassification input_slot_class;
			uint32 instance_data_step_rate;

			InputElementDesc() = default;
			InputElementDesc(
				const char* _semantic_name,
				uint32 _semantic_index,
				EResourceFormat _format,
				uint32 _input_slot = 0,
				uint32 _aligned_byte_offset = l_append_aligned_element,
				EInputClassification _input_slot_class = EInputClassification::per_vertex,
				uint32 _instance_data_step_rate = 0
			) :
				semantic_name(_semantic_name),
				semantic_index(_semantic_index),
				format(_format),
				input_slot(_input_slot),
				aligned_byte_offset(_aligned_byte_offset),
				input_slot_class(_input_slot_class),
				instance_data_step_rate(_instance_data_step_rate) {}
		};

		struct InputLayoutDesc
		{
			const InputElementDesc* input_elements;
			uint32 num_elements;

			InputLayoutDesc() = default;
			InputLayoutDesc(
				uint32 _num_elements,
				const InputElementDesc* _input_elements
			) :
				num_elements(_num_elements),
				input_elements(_input_elements) {}
		};

		enum class EIndexBufferStripCutValue : uint32
		{
			disabled,
			value_0xffff,
			value_0xffffffff
		};

		enum class EPrimitiveTopologyType : uint32
		{
			undefined,
			point,
			line,
			triangle,
			patch
		};

		struct GraphicsPipelineStateDesc
		{
			InputLayoutDesc input_layout;
			ShaderBytecode vs;
			ShaderBytecode ps;
			ShaderBytecode ds;
			ShaderBytecode hs;
			ShaderBytecode gs;
			StreamOutputDesc stream_output;
			BlendDesc blend_state;
			RasterizerDesc rasterizer_state;
			DepthStencilDesc depth_stencil_state;
			EIndexBufferStripCutValue ib_strip_cut_value;
			EPrimitiveTopologyType primitive_topology_type;
			uint32 sample_mask;
			uint32 sample_quality;

			GraphicsPipelineStateDesc() = default;
			GraphicsPipelineStateDesc(
				const InputLayoutDesc& _input_layout,
				const ShaderBytecode& _vs,
				const ShaderBytecode& _ps,
				const ShaderBytecode& _ds,
				const ShaderBytecode& _hs,
				const ShaderBytecode& _gs,
				const StreamOutputDesc& _stream_output,
				const BlendDesc& _blend_state,
				const RasterizerDesc& _rasterizer_state,
				const DepthStencilDesc& _depth_stencil_state,
				EIndexBufferStripCutValue _ib_strip_cut_value,
				EPrimitiveTopologyType _primitive_topology_type,
				uint32 _sample_mask,
				uint32 _sample_quality
			) :
				input_layout(_input_layout),
				vs(_vs),
				ps(_ps),
				ds(_ds),
				hs(_hs),
				gs(_gs),
				stream_output(_stream_output),
				blend_state(_blend_state),
				rasterizer_state(_rasterizer_state),
				depth_stencil_state(_depth_stencil_state),
				ib_strip_cut_value(_ib_strip_cut_value),
				primitive_topology_type(_primitive_topology_type),
				sample_mask(_sample_mask),
				sample_quality(_sample_quality) {}
		};

		struct ComputePipelineStateDesc
		{
			ShaderBytecode cs;

			ComputePipelineStateDesc() = default;
			ComputePipelineStateDesc(
				const ShaderBytecode& _cs) :
				cs(_cs) {}
		};

		//! @interface IPipelineState
		//! @threadsafe
		struct IPipelineState : public IGraphicDeviceChild
		{
			luiid("{84e9d670-4181-4f5d-bfa5-0fc94b75cc19}");

			// No additional interfaces.
		};
	}
}