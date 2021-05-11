// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file D3D12Common.hpp
* @author JXMaster
* @date 2019/8/10
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12

#include <d3d12.h>
#include "../Windows/CommonInclude.hpp"

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			inline D3D12_RESOURCE_STATES encode_resource_state(EResourceState s)
			{
				switch (s)
				{
				case EResourceState::common:
					return D3D12_RESOURCE_STATE_COMMON;
				case EResourceState::vertex_and_constant_buffer:
					return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
				case EResourceState::copy_dest:
					return D3D12_RESOURCE_STATE_COPY_DEST;
				case EResourceState::copy_source:
					return D3D12_RESOURCE_STATE_COPY_SOURCE;
				case EResourceState::depth_stencil_write:
					return D3D12_RESOURCE_STATE_DEPTH_WRITE; // If depth-writes are disabled, return D3D12_RESOURCE_STATE_DEPTH_WRITE
				case EResourceState::depth_stencil_read:
					return D3D12_RESOURCE_STATE_DEPTH_READ;
				case EResourceState::index_buffer:
					return D3D12_RESOURCE_STATE_INDEX_BUFFER;
				case EResourceState::indirect_argument:
					return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
				//case EResourceState::present:
					//return D3D12_RESOURCE_STATE_PRESENT;
				case EResourceState::render_target:
					return D3D12_RESOURCE_STATE_RENDER_TARGET;
				case EResourceState::resolve_dest:
					return D3D12_RESOURCE_STATE_RESOLVE_DEST;
				case EResourceState::resolve_src:
					return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
				case EResourceState::shader_resource_pixel:
					return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // Need the shader flags mask in case the SRV is used by non-PS
				case EResourceState::shader_resource_non_pixel:
					return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
				case EResourceState::stream_out:
					return D3D12_RESOURCE_STATE_STREAM_OUT;
				case EResourceState::unordered_access:
					return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
				case EResourceState::generic_read:
					return D3D12_RESOURCE_STATE_GENERIC_READ;
				default:
					lupanic();
					return D3D12_RESOURCE_STATE_COMMON;
				}
			}

			inline D3D12_FILTER encode_filter(EFilter f)
			{
				switch (f)
				{
				case EFilter::min_mag_mip_point:
					return D3D12_FILTER_MIN_MAG_MIP_POINT;
				case EFilter::min_mag_point_mip_linear:
					return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
				case EFilter::min_point_mag_linear_mip_point:
					return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
				case EFilter::min_point_mag_mip_linear:
					return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
				case EFilter::min_linear_mag_mip_point:
					return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
				case EFilter::min_linear_mag_point_mip_linear:
					return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				case EFilter::min_mag_linear_mip_point:
					return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				case EFilter::min_mag_mip_linear:
					return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
				case EFilter::anisotropic:
					return D3D12_FILTER_ANISOTROPIC;
				case EFilter::comparison_min_mag_mip_point:
					return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
				case EFilter::comparison_min_mag_point_mip_linear:
					return D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
				case EFilter::comparison_min_point_mag_linear_mip_point:
					return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
				case EFilter::comparison_min_point_mag_mip_linear:
					return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
				case EFilter::comparison_min_linear_mag_mip_point:
					return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
				case EFilter::comparison_min_linear_mag_point_mip_linear:
					return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				case EFilter::comparison_min_mag_linear_mip_point:
					return D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
				case EFilter::comparison_min_mag_mip_linear:
					return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
				case EFilter::comparison_anisotropic:
					return D3D12_FILTER_COMPARISON_ANISOTROPIC;
				case EFilter::minimum_min_mag_mip_point:
					return D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
				case EFilter::minimum_min_mag_point_mip_linear:
					return D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
				case EFilter::minimum_min_point_mag_linear_mip_point:
					return D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
				case EFilter::minimum_min_point_mag_mip_linear:
					return D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
				case EFilter::minimum_min_linear_mag_mip_point:
					return D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
				case EFilter::minimum_min_linear_mag_point_mip_linear:
					return D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				case EFilter::minimum_min_mag_linear_mip_point:
					return D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
				case EFilter::minimum_min_mag_mip_linear:
					return D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
				case EFilter::minimum_anisotropic:
					return D3D12_FILTER_MINIMUM_ANISOTROPIC;
				case EFilter::maximum_min_mag_mip_point:
					return D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
				case EFilter::maximum_min_mag_point_mip_linear:
					return D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
				case EFilter::maximum_min_point_mag_linear_mip_point:
					return D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
				case EFilter::maximum_min_point_mag_mip_linear:
					return D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
				case EFilter::maximum_min_linear_mag_mip_point:
					return D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
				case EFilter::maximum_min_linear_mag_point_mip_linear:
					return D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				case EFilter::maximum_min_mag_linear_mip_point:
					return D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
				case EFilter::maximum_min_mag_mip_linear:
					return D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
				case EFilter::maximum_anisotropic:
				default:
					lupanic();
					return D3D12_FILTER_MIN_MAG_MIP_POINT;
				}
			}

			inline D3D12_TEXTURE_ADDRESS_MODE encode_address_mode(ETextureAddressMode mode)
			{
				switch (mode)
				{
				case ETextureAddressMode::warp:
					return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				case ETextureAddressMode::mirror:
					return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
				case ETextureAddressMode::clamp:
					return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
				case ETextureAddressMode::border:
					return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
				case ETextureAddressMode::mirror_once:
				default:
					return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
				}
			}

			inline D3D12_COMPARISON_FUNC encode_comparison_func(EComparisonFunc c)
			{
				switch (c)
				{
				case EComparisonFunc::never:
					return D3D12_COMPARISON_FUNC_NEVER;
				case EComparisonFunc::less:
					return D3D12_COMPARISON_FUNC_LESS;
				case EComparisonFunc::equal:
					return D3D12_COMPARISON_FUNC_EQUAL;
				case EComparisonFunc::less_equal:
					return D3D12_COMPARISON_FUNC_LESS_EQUAL;
				case EComparisonFunc::greater:
					return D3D12_COMPARISON_FUNC_GREATER;
				case EComparisonFunc::not_equal:
					return D3D12_COMPARISON_FUNC_NOT_EQUAL;
				case EComparisonFunc::greater_equal:
					return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
				case EComparisonFunc::always:
				default:
					return D3D12_COMPARISON_FUNC_ALWAYS;
				}
			}

			//inline D3D12_DESCRIPTOR_HEAP_TYPE encode_view_heap_type(EViewTableType t)
			//{
			//	switch (t)
			//	{
			//	case EViewTableType::cbv_srv_uav:
			//		return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			//	case EViewTableType::dsv:
			//		return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			//	case EViewTableType::rtv:
			//		return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			//	case EViewTableType::sampler:
			//		return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			//	default:
			//		lupanic();
			//		return D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
			//	}
			//}

			//inline D3D12_COMMAND_LIST_TYPE encode_command_list_type(ECommandListType t)
			//{
			//	switch (t)
			//	{
			//	case Luna::gfx::ECommandListType::graphic:
			//		return D3D12_COMMAND_LIST_TYPE_DIRECT;
			//	case Luna::gfx::ECommandListType::compute:
			//		return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			//	case Luna::gfx::ECommandListType::copy:
			//		return D3D12_COMMAND_LIST_TYPE_COPY;
			//	default:
			//		lupanic();
			//		return D3D12_COMMAND_LIST_TYPE_DIRECT;
			//	}
			//}

			inline D3D12_RESOURCE_DESC encode_resource_desc(const ResourceDesc& desc)
			{
				D3D12_RESOURCE_DESC rd;
				switch (desc.type)
				{
				case EResourceType::buffer:
					rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
					break;
				case EResourceType::texture_1d:
					rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
					break;
				case EResourceType::texture_2d:
					rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
					break;
				case EResourceType::texture_3d:
					rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
					break;
				default:
					lupanic();
					rd.Dimension = D3D12_RESOURCE_DIMENSION_UNKNOWN;
					break;
				}
				rd.Alignment = 0;
				rd.Width = desc.width;
				rd.Height = desc.height;
				rd.DepthOrArraySize = desc.depth_or_array_size;
				rd.MipLevels = desc.mip_levels;
				rd.Format = encode_resource_format(desc.format);
				rd.SampleDesc.Count = desc.sample_count;
				rd.SampleDesc.Quality = desc.sample_quality;
				if (desc.type == EResourceType::buffer)
				{
					rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
				}
				else
				{
					rd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				}
				rd.Flags = D3D12_RESOURCE_FLAG_NONE;
				if ((desc.usages & EResourceUsageFlag::render_target) != EResourceUsageFlag::none)
				{
					rd.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
				}
				if ((desc.usages & EResourceUsageFlag::depth_stencil) != EResourceUsageFlag::none)
				{
					rd.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
				}
				if ((desc.usages & EResourceUsageFlag::unordered_access) != EResourceUsageFlag::none)
				{
					rd.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
				}
				if ((desc.usages & EResourceUsageFlag::simultaneous_access) != EResourceUsageFlag::none)
				{
					rd.Flags |= D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
				}
				// The D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE flag will make Visual Studio graphic debug layer to crash.
#ifndef LUNA_PROFILE
				if (((desc.usages & EResourceUsageFlag::shader_resource) == EResourceUsageFlag::none) && ((desc.usages & EResourceUsageFlag::depth_stencil) != EResourceUsageFlag::none))
				{
					rd.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
				}
#endif
				return rd;
			}

			//! The resource object now caches the resource desc, so this may not needed.
			/*inline ResourceDesc decode_resource_desc(const D3D12_RESOURCE_DESC& desc)
			{
				ResourceDesc r;
				switch (desc.Dimension)
				{
				case D3D12_RESOURCE_DIMENSION_BUFFER:
					r.type = EResourceType::buffer;
					break;
				case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
					r.type = EResourceType::texture_1d;
					break;
				case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
					r.type = EResourceType::texture_2d;
					break;
				case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
					r.type = EResourceType::texture_3d;
				case D3D12_RESOURCE_DIMENSION_UNKNOWN:
					r.type = EResourceType::unknown;
					break;
				default:
					lupanic();
					break;
				}
				r.width = desc.Width;
				r.height = desc.Height;
				r.depth_or_array_size = desc.DepthOrArraySize;
				r.mip_levels = desc.MipLevels;
				r.format = decode_resource_format(desc.Format);
				r.sample_count = desc.SampleDesc.Count;
				r.sample_quality = desc.SampleDesc.Quality;
				r.usages = EResourceUsageFlag::none;
				if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
				{
					r.usages |= EResourceUsageFlag::render_target;
				}
				if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
				{
					r.usages |= EResourceUsageFlag::depth_stencil;
				}
				if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
				{
					r.usages |= EResourceUsageFlag::unordered_access;
				}

				return r;
			}*/
		}
	}
}

#endif