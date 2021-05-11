// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ResourceFormat.hpp
* @author JXMaster
* @date 2019/1/29
* @brief Resource format enum
*/
#pragma once
#include <Core/Core.hpp>
namespace Luna
{
	namespace Gfx
	{
		enum class EResourceFormat : u32
		{
			unknown = 0,
			r8_unorm,
			r8_snorm,
			r16_unorm,
			r16_snorm,
			rg8_unorm,
			rg8_snorm,
			rg16_unorm,
			rg16_snorm,
			r24_unorm_x8,
			rgb5a1_unorm,
			rgba8_unorm,
			rgba8_snorm,
			rgb10a2_unorm,
			rgb10a2_uint,
			rgba16_unorm,
			rgba8_unorm_srgb,

			r16_float,
			rg16_float,
			rgba16_float,
			r32_float,
			r32_float_x32,
			rg32_float,
			rgb32_float,
			rgba32_float,
			r11g11b10_float,
			rgb9e5_float,

			r8_int,
			r8_uint,
			r16_int,
			r16_uint,
			r32_int,
			r32_uint,
			rg8_int,
			rg8_uint,
			rg16_int,
			rg16_uint,
			rg32_int,
			rg32_uint,
			rgb32_int,
			rgb32_uint,
			rgba8_int,
			rgba8_uint,
			rgba16_int,
			rgba16_uint,
			rgba32_int,
			rgba32_uint,

			bgra8_unorm,
			bgra8_unorm_srgb,

			bgrx8_unorm,
			bgrx8_unorm_srgb,
			alpha8_unorm,
			r5g6b5_unorm,

			// Depth-stencil
			d32_float,
			d16_unorm,
			d32_float_s8x24,
			d24_unorm_s8,

			// Compressed formats
			bc1_unorm,   // DXT1
			bc1_unorm_srgb,
			bc2_unorm,   // DXT3
			bc2_unorm_srgb,
			bc3_unorm,   // DXT5
			bc3_unorm_srgb,
			bc4_unorm,   // RGTC Unsigned Red
			bc4_snorm,   // RGTC Signed Red
			bc5_unorm,   // RGTC Unsigned RG
			bc5_snorm,   // RGTC Signed RG
			bc6_hs16,
			bc6_hu16,
			bc7_unorm,
			bc7_unorm_srgb,

			count
		};

		//! Returns the size of one pixel in the specified format, in bits.
		inline u16 bits_per_pixel(EResourceFormat format)
		{
			switch (format)
			{
			case EResourceFormat::rgba32_float:
			case EResourceFormat::rgba32_int:
			case EResourceFormat::rgba32_uint:
				return 128;
			case EResourceFormat::rgb32_float:
			case EResourceFormat::rgb32_int:
			case EResourceFormat::rgb32_uint:
				return 96;
			case EResourceFormat::rgba16_float:
			case EResourceFormat::rgba16_int:
			case EResourceFormat::rgba16_uint:
			case EResourceFormat::rgba16_unorm:
			case EResourceFormat::rg32_float:
			case EResourceFormat::rg32_int:
			case EResourceFormat::rg32_uint:
			case EResourceFormat::r32_float_x32:
			case EResourceFormat::d32_float_s8x24:
				return 64;
			case EResourceFormat::rgb10a2_uint:
			case EResourceFormat::rgb10a2_unorm:
			case EResourceFormat::r11g11b10_float:
			case EResourceFormat::rgba8_int:
			case EResourceFormat::rgba8_snorm:
			case EResourceFormat::rgba8_uint:
			case EResourceFormat::rgba8_unorm:
			case EResourceFormat::rgba8_unorm_srgb:
			case EResourceFormat::rg16_float:
			case EResourceFormat::rg16_int:
			case EResourceFormat::rg16_snorm:
			case EResourceFormat::rg16_uint:
			case EResourceFormat::rg16_unorm:
			case EResourceFormat::r32_float:
			case EResourceFormat::r32_int:
			case EResourceFormat::r32_uint:
			case EResourceFormat::d32_float:
			case EResourceFormat::r24_unorm_x8:
			case EResourceFormat::rgb9e5_float:
			case EResourceFormat::bgra8_unorm:
			case EResourceFormat::bgra8_unorm_srgb:
			case EResourceFormat::bgrx8_unorm:
			case EResourceFormat::bgrx8_unorm_srgb:
				return 32;
			case EResourceFormat::rg8_int:
			case EResourceFormat::rg8_snorm:
			case EResourceFormat::rg8_uint:
			case EResourceFormat::rg8_unorm:
			case EResourceFormat::r16_float:
			case EResourceFormat::r16_int:
			case EResourceFormat::r16_snorm:
			case EResourceFormat::r16_uint:
			case EResourceFormat::r16_unorm:
			case EResourceFormat::d16_unorm:
			case EResourceFormat::r5g6b5_unorm:
			case EResourceFormat::rgb5a1_unorm:
				return 16;
			case EResourceFormat::r8_int:
			case EResourceFormat::r8_snorm:
			case EResourceFormat::r8_uint:
			case EResourceFormat::r8_unorm:
			case EResourceFormat::alpha8_unorm:
				return 8;
			case EResourceFormat::bc1_unorm:
			case EResourceFormat::bc1_unorm_srgb:
			case EResourceFormat::bc4_snorm:
			case EResourceFormat::bc4_unorm:
				return 4;
			case EResourceFormat::bc2_unorm:
			case EResourceFormat::bc2_unorm_srgb:
			case EResourceFormat::bc3_unorm:
			case EResourceFormat::bc3_unorm_srgb:
			case EResourceFormat::bc5_snorm:
			case EResourceFormat::bc5_unorm:
			case EResourceFormat::bc6_hs16:
			case EResourceFormat::bc6_hu16:
			case EResourceFormat::bc7_unorm:
			case EResourceFormat::bc7_unorm_srgb:
				return 8;
			default:
				lupanic();
				return 0;
			}
		}

		enum class EResourceType : u32
		{
			unknown = 0,
			buffer,
			texture_1d,
			texture_2d,
			texture_3d
		};

		enum class EComparisonFunc : u32
		{
			never,	// Never pass comparison
			less,
			equal,
			less_equal,
			greater,
			not_equal,
			greater_equal,
			always	// Always pass comparison
		};

		enum class EResourceState : u32
		{
			//! The state used for passing resources between different graphic engine types (graphic/compute/copy).
			//! This state is also required if you need to read/write data in CPU side.
			common = 0,
			//! Used as vertex and constant buffer in 3D pipeline by vertex buffer view and index buffer view.
			//! Read-only.
			vertex_and_constant_buffer,
			//! Used as index buffer in 3D pipeline.
			//! Read-only.
			index_buffer,
			//! Used for render target in 3D pipeline via RTV or clear render target.
			//! Write-only.
			render_target,
			//! Used for unordered access in pipeline via UAV.
			//! Read/Write.
			unordered_access,
			//! Used for depth write in 3d pipeline via DSV or clear depth stencil.
			//! Write-only for depth.
			depth_stencil_write,
			//! Used for depth write is disabled.
			//! Read-only for depth.
			depth_stencil_read,
			//! Used as shader resource for non-pixel shader via SRV.
			//! Read-only.
			shader_resource_non_pixel,
			//! Used as shader resource for pixel shader via SRV.
			//! Read-only.
			shader_resource_pixel,
			//! Used as stream output.
			//! Write-only.
			stream_out,
			//! Used as indirect argument.
			//! Read-only.
			indirect_argument,
			//! Used as a copy destination.
			//! Write-only.
			copy_dest,
			//! Used as a copy source.
			//! Read-only.
			copy_source,
			//! Used as destination in a resolve operation.
			resolve_dest,
			//! Used as source in a resolve operation.
			resolve_src,
			//! Generic read. A combination of all read-only states, use this only when creating upload heap.
			generic_read,
			//! Used to present to window.
			// present
		};

		enum class EPrimitiveTopology : u32
		{
			undefined,
			point_list,
			line_list,
			line_strip,
			triangle_list,
			triangle_strip,
			line_list_adj,
			line_strip_adj,
			triangle_list_adj,
			triangle_strip_adj,
			patchlist_1_control_point,
			patchlist_2_control_point,
			patchlist_3_control_point,
			patchlist_4_control_point,
			patchlist_5_control_point,
			patchlist_6_control_point,
			patchlist_7_control_point,
			patchlist_8_control_point,
			patchlist_9_control_point,
			patchlist_10_control_point,
			patchlist_11_control_point,
			patchlist_12_control_point,
			patchlist_13_control_point,
			patchlist_14_control_point,
			patchlist_15_control_point,
			patchlist_16_control_point,
			patchlist_17_control_point,
			patchlist_18_control_point,
			patchlist_19_control_point,
			patchlist_20_control_point,
			patchlist_21_control_point,
			patchlist_22_control_point,
			patchlist_23_control_point,
			patchlist_24_control_point,
			patchlist_25_control_point,
			patchlist_26_control_point,
			patchlist_27_control_point,
			patchlist_28_control_point,
			patchlist_29_control_point,
			patchlist_30_control_point,
			patchlist_31_control_point,
			patchlist_32_control_point,
		};

		struct Viewport
		{
			f32 top_left_x;
			f32 top_left_y;
			f32 width;
			f32 height;
			f32 min_depth;
			f32 max_depth;

			Viewport() = default;
			Viewport(
				f32 _top_left_x,
				f32 _top_left_y,
				f32 _width,
				f32 _height,
				f32 _min_depth,
				f32 _max_depth) :
				top_left_x(_top_left_x),
				top_left_y(_top_left_y),
				width(_width),
				height(_height),
				min_depth(_min_depth),
				max_depth(_max_depth) {}

		};



		//! Used for initializing descriptors to default.
		struct Default {};

		//! Specify how CPU and GPU should access the resource heap and all of its resources.
		enum class EAccessType
		{
			// Specify this when the resource is placed created in the heap. The system automatically detects 
			// the heap type and makes the resource access type to the heap type.
			unknown = 0,
			// The resource can only be accessed by GPU. CPU cannot access this resource.
			gpu_local = 1,
			// The resource can be written to by CPU and be copied from by GPU.
			upload = 2,
			// The resource can be read from by CPU and be copied to by GPU.
			readback = 3
		};

		//! Specify the possible usage of this resource.
		enum class EResourceUsageFlag : u32
		{
			none = 0x00,
			//! Allows this resource to be bound as shader resource by graphic pipeline.
			shader_resource = 0x01,
			//! Allows this resource to be bound as constant buffer by graphic pipeline.
			constant_buffer = 0x02,
			//! Allows this resource to be bound for unordered access by compute pipeline.
			unordered_access = 0x04,
			//! Allows this resource to be bound as render target by graphic pipeline.
			render_target = 0x08,
			//! Allows this resource to be bound as depth stencil by graphic pipeline.
			depth_stencil = 0x10,
			//! Allows this resource to be bound as vertex buffer by graphic pipeline.
			vertex_buffer = 0x20,
			//! Allows this resource to be bound as index buffer by graphic pipeline.
			index_buffer = 0x40,
			//! Allows this resource to be bound as streaming output buffer by graphic pipeline.
			stream_output = 0x80,
			//! Allow this resource to be used simultaneously from multiple command queues, given that only 
			//! one queue is writing to the resource.
			simultaneous_access = 0x200
		};

		struct DepthStencilValue
		{
			f32 depth;
			u8 stencil;
		};

		enum class EClearValueType : u32
		{
			color = 1,
			depth_stencil = 2
		};

		struct ClearValue
		{
			EResourceFormat format;
			EClearValueType type;
			union
			{
				f32 color[4];
				DepthStencilValue depth_stencil;
			};
			static ClearValue as_color(EResourceFormat _format, f32 _color[4])
			{
				ClearValue r;
				r.format = _format;
				r.type = EClearValueType::color;
				r.color[0] = _color[0];
				r.color[1] = _color[1];
				r.color[2] = _color[2];
				r.color[3] = _color[3];
				return r;
			}
			static ClearValue as_depth_stencil(EResourceFormat _format, f32 _depth, u8 _stencil)
			{
				ClearValue r;
				r.type = EClearValueType::depth_stencil;
				r.format = _format;
				r.depth_stencil.depth = _depth;
				r.depth_stencil.stencil = _stencil;
				return r;
			}
		};
	}
}