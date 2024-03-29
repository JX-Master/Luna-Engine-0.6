// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file CommonInclude.hpp
* @author JXMaster
* @date 2019/7/17
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include <wrl/client.h>
#include <dxgi.h>
using Microsoft::WRL::ComPtr;

namespace Luna
{
	namespace Gfx
	{
		inline DXGI_FORMAT encode_resource_format(EResourceFormat f)
		{
			switch (f)
			{
			case EResourceFormat::unknown:
				return DXGI_FORMAT_UNKNOWN;
			case EResourceFormat::r8_unorm:
				return DXGI_FORMAT_R8_UNORM;
			case EResourceFormat::r8_snorm:
				return DXGI_FORMAT_R8_SNORM;
			case EResourceFormat::r16_unorm:
				return DXGI_FORMAT_R16_UNORM;
			case EResourceFormat::r16_snorm:
				return DXGI_FORMAT_R16_SNORM;
			case EResourceFormat::rg8_unorm:
				return DXGI_FORMAT_R8G8_UNORM;
			case EResourceFormat::rg8_snorm:
				return DXGI_FORMAT_R8G8_SNORM;
			case EResourceFormat::rg16_unorm:
				return DXGI_FORMAT_R16G16_UNORM;
			case EResourceFormat::rg16_snorm:
				return DXGI_FORMAT_R16G16_SNORM;
			case EResourceFormat::r24_unorm_x8:
				return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			case EResourceFormat::rgb5a1_unorm:
				return DXGI_FORMAT_B5G5R5A1_UNORM;
			case EResourceFormat::rgba8_unorm:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			case EResourceFormat::rgba8_snorm:
				return DXGI_FORMAT_R8G8B8A8_SNORM;
			case EResourceFormat::rgb10a2_unorm:
				return DXGI_FORMAT_R10G10B10A2_UNORM;
			case EResourceFormat::rgb10a2_uint:
				return DXGI_FORMAT_R10G10B10A2_UINT;
			case EResourceFormat::rgba16_unorm:
				return DXGI_FORMAT_R16G16B16A16_UNORM;
			case EResourceFormat::rgba8_unorm_srgb:
				return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case EResourceFormat::r16_float:
				return DXGI_FORMAT_R16_FLOAT;
			case EResourceFormat::rg16_float:
				return DXGI_FORMAT_R16G16_FLOAT;
			case EResourceFormat::rgba16_float:
				return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case EResourceFormat::r32_float:
				return DXGI_FORMAT_R32_FLOAT;
			case EResourceFormat::r32_float_x32:
				return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
			case EResourceFormat::rg32_float:
				return DXGI_FORMAT_R32G32_FLOAT;
			case EResourceFormat::rgb32_float:
				return DXGI_FORMAT_R32G32B32_FLOAT;
			case EResourceFormat::rgba32_float:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case EResourceFormat::r11g11b10_float:
				return DXGI_FORMAT_R11G11B10_FLOAT;
			case EResourceFormat::rgb9e5_float:
				return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
			case EResourceFormat::r8_int:
				return DXGI_FORMAT_R8_SINT;
			case EResourceFormat::r8_uint:
				return DXGI_FORMAT_R8_UINT;
			case EResourceFormat::r16_int:
				return DXGI_FORMAT_R16_SINT;
			case EResourceFormat::r16_uint:
				return DXGI_FORMAT_R16_UINT;
			case EResourceFormat::r32_int:
				return DXGI_FORMAT_R32_SINT;
			case EResourceFormat::r32_uint:
				return DXGI_FORMAT_R32_UINT;
			case EResourceFormat::rg8_int:
				return DXGI_FORMAT_R8G8_SINT;
			case EResourceFormat::rg8_uint:
				return DXGI_FORMAT_R8G8_UINT;
			case EResourceFormat::rg16_int:
				return DXGI_FORMAT_R16G16_SINT;
			case EResourceFormat::rg16_uint:
				return DXGI_FORMAT_R16G16_UINT;
			case EResourceFormat::rg32_int:
				return DXGI_FORMAT_R32G32_SINT;
			case EResourceFormat::rg32_uint:
				return DXGI_FORMAT_R32G32_UINT;
			case EResourceFormat::rgb32_int:
				return DXGI_FORMAT_R32G32B32_SINT;
			case EResourceFormat::rgb32_uint:
				return DXGI_FORMAT_R32G32B32_UINT;
			case EResourceFormat::rgba8_int:
				return DXGI_FORMAT_R8G8B8A8_SINT;
			case EResourceFormat::rgba8_uint:
				return DXGI_FORMAT_R8G8B8A8_UINT;
			case EResourceFormat::rgba16_int:
				return DXGI_FORMAT_R16G16B16A16_SINT;
			case EResourceFormat::rgba16_uint:
				return DXGI_FORMAT_R16G16B16A16_UINT;
			case EResourceFormat::rgba32_int:
				return DXGI_FORMAT_R32G32B32A32_SINT;
			case EResourceFormat::rgba32_uint:
				return DXGI_FORMAT_R32G32B32A32_UINT;
			case EResourceFormat::bgra8_unorm:
				return DXGI_FORMAT_B8G8R8A8_UNORM;
			case EResourceFormat::bgra8_unorm_srgb:
				return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			case EResourceFormat::bgrx8_unorm:
				return DXGI_FORMAT_B8G8R8X8_UNORM;
			case EResourceFormat::bgrx8_unorm_srgb:
				return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
			case EResourceFormat::alpha8_unorm:
				return DXGI_FORMAT_A8_UNORM;
			case EResourceFormat::r5g6b5_unorm:
				return DXGI_FORMAT_B5G6R5_UNORM;
			case EResourceFormat::d32_float:
				return DXGI_FORMAT_D32_FLOAT;
			case EResourceFormat::d16_unorm:
				return DXGI_FORMAT_D16_UNORM;
			case EResourceFormat::d32_float_s8x24:
				return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			case EResourceFormat::d24_unorm_s8:
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case EResourceFormat::bc1_unorm:
				return DXGI_FORMAT_BC1_UNORM;
			case EResourceFormat::bc1_unorm_srgb:
				return DXGI_FORMAT_BC1_UNORM_SRGB;
			case EResourceFormat::bc2_unorm:
				return DXGI_FORMAT_BC2_UNORM;
			case EResourceFormat::bc2_unorm_srgb:
				return DXGI_FORMAT_BC2_UNORM_SRGB;
			case EResourceFormat::bc3_unorm:
				return DXGI_FORMAT_BC3_UNORM;
			case EResourceFormat::bc3_unorm_srgb:
				return DXGI_FORMAT_BC3_UNORM_SRGB;
			case EResourceFormat::bc4_unorm:
				return DXGI_FORMAT_BC4_UNORM;
			case EResourceFormat::bc4_snorm:
				return DXGI_FORMAT_BC4_SNORM;
			case EResourceFormat::bc5_unorm:
				return DXGI_FORMAT_BC5_UNORM;
			case EResourceFormat::bc5_snorm:
				return DXGI_FORMAT_BC5_SNORM;
			case EResourceFormat::bc6_hs16:
				return DXGI_FORMAT_BC6H_SF16;
			case EResourceFormat::bc6_hu16:
				return DXGI_FORMAT_BC6H_UF16;
			case EResourceFormat::bc7_unorm:
				return DXGI_FORMAT_BC6H_UF16;
			case EResourceFormat::bc7_unorm_srgb:
				return DXGI_FORMAT_BC7_UNORM_SRGB;
			default:
				lupanic();
				return DXGI_FORMAT_UNKNOWN;
			}
		}

		inline EResourceFormat decode_resource_format(DXGI_FORMAT f)
		{
			switch (f)
			{
			case DXGI_FORMAT_UNKNOWN:
				return EResourceFormat::unknown;
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
				return EResourceFormat::rgba32_float;
			case DXGI_FORMAT_R32G32B32A32_UINT:
				return EResourceFormat::rgba32_uint;
			case DXGI_FORMAT_R32G32B32A32_SINT:
				return EResourceFormat::rgba32_int;
			case DXGI_FORMAT_R32G32B32_FLOAT:
				return EResourceFormat::rgb32_float;
			case DXGI_FORMAT_R32G32B32_UINT:
				return EResourceFormat::rgb32_uint;
			case DXGI_FORMAT_R32G32B32_SINT:
				return EResourceFormat::rgb32_int;
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
				return EResourceFormat::rgba16_float;
			case DXGI_FORMAT_R16G16B16A16_UNORM:
				return EResourceFormat::rgba16_unorm;
			case DXGI_FORMAT_R16G16B16A16_UINT:
				return EResourceFormat::rgba16_uint;
			case DXGI_FORMAT_R16G16B16A16_SINT:
				return EResourceFormat::rgba16_int;
			case DXGI_FORMAT_R32G32_FLOAT:
				return EResourceFormat::rg32_float;
			case DXGI_FORMAT_R32G32_UINT:
				return EResourceFormat::rg32_uint;
			case DXGI_FORMAT_R32G32_SINT:
				return EResourceFormat::rg32_int;
			case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
				return EResourceFormat::r32_float_x32;
			case DXGI_FORMAT_R10G10B10A2_UNORM:
				return EResourceFormat::rgb10a2_unorm;
			case DXGI_FORMAT_R10G10B10A2_UINT:
				return EResourceFormat::rgb10a2_uint;
			case DXGI_FORMAT_R11G11B10_FLOAT:
				return EResourceFormat::r11g11b10_float;
			case DXGI_FORMAT_R8G8B8A8_UNORM:
				return EResourceFormat::rgba8_unorm;
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
				return EResourceFormat::rgba8_unorm_srgb;
			case DXGI_FORMAT_R8G8B8A8_UINT:
				return EResourceFormat::rgba8_uint;
			case DXGI_FORMAT_R8G8B8A8_SINT:
				return EResourceFormat::rgba8_int;
			case DXGI_FORMAT_R16G16_FLOAT:
				return EResourceFormat::rg16_float;
			case DXGI_FORMAT_R16G16_UNORM:
				return EResourceFormat::rg16_unorm;
			case DXGI_FORMAT_R16G16_UINT:
				return EResourceFormat::rg16_uint;
			case DXGI_FORMAT_R16G16_SNORM:
				return EResourceFormat::rg16_snorm;
			case DXGI_FORMAT_R16G16_SINT:
				return EResourceFormat::rg16_int;
			case DXGI_FORMAT_D32_FLOAT:
				return EResourceFormat::d32_float;
			case DXGI_FORMAT_R32_FLOAT:
				return EResourceFormat::r32_float;
			case DXGI_FORMAT_R32_UINT:
				return EResourceFormat::r32_uint;
			case DXGI_FORMAT_R32_SINT:
				return EResourceFormat::r32_int;
			case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
				return EResourceFormat::r24_unorm_x8;
			case DXGI_FORMAT_R8G8_UNORM:
				return EResourceFormat::rg8_unorm;
			case DXGI_FORMAT_R8G8_UINT:
				return EResourceFormat::rg8_uint;
			case DXGI_FORMAT_R8G8_SNORM:
				return EResourceFormat::rg8_snorm;
			case DXGI_FORMAT_R8G8_SINT:
				return EResourceFormat::rg8_int;
			case DXGI_FORMAT_R16_FLOAT:
				return EResourceFormat::r16_float;
			case DXGI_FORMAT_D16_UNORM:
				return EResourceFormat::d16_unorm;
			case DXGI_FORMAT_R16_UNORM:
				return EResourceFormat::r16_unorm;
			case DXGI_FORMAT_R16_UINT:
				return EResourceFormat::r16_uint;
			case DXGI_FORMAT_R16_SNORM:
				return EResourceFormat::r16_snorm;
			case DXGI_FORMAT_R16_SINT:
				return EResourceFormat::r16_int;
			case DXGI_FORMAT_R8_UNORM:
				return EResourceFormat::r8_unorm;
			case DXGI_FORMAT_R8_UINT:
				return EResourceFormat::r8_uint;
			case DXGI_FORMAT_R8_SNORM:
				return EResourceFormat::r8_snorm;
			case DXGI_FORMAT_R8_SINT:
				return EResourceFormat::r8_int;
			case DXGI_FORMAT_A8_UNORM:
				return EResourceFormat::alpha8_unorm;
			case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
				return EResourceFormat::rgb9e5_float;
			case DXGI_FORMAT_BC1_UNORM:
				return EResourceFormat::bc1_unorm;
			case DXGI_FORMAT_BC1_UNORM_SRGB:
				return EResourceFormat::bc1_unorm_srgb;
			case DXGI_FORMAT_BC2_UNORM:
				return EResourceFormat::bc2_unorm;
			case DXGI_FORMAT_BC2_UNORM_SRGB:
				return EResourceFormat::bc2_unorm_srgb;
			case DXGI_FORMAT_BC3_UNORM:
				return EResourceFormat::bc3_unorm;
			case DXGI_FORMAT_BC3_UNORM_SRGB:
				return EResourceFormat::bc3_unorm_srgb;
			case DXGI_FORMAT_BC4_UNORM:
				return EResourceFormat::bc4_unorm;
			case DXGI_FORMAT_BC4_SNORM:
				return EResourceFormat::bc4_snorm;
			case DXGI_FORMAT_BC5_UNORM:
				return EResourceFormat::bc5_unorm;
			case DXGI_FORMAT_BC5_SNORM:
				return EResourceFormat::bc5_snorm;
			case DXGI_FORMAT_B8G8R8A8_UNORM:
				return EResourceFormat::bgra8_unorm;
			case DXGI_FORMAT_B8G8R8X8_UNORM:
				return EResourceFormat::bgrx8_unorm;
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
				return EResourceFormat::bgra8_unorm_srgb;
			case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
				return EResourceFormat::bgrx8_unorm_srgb;
			case DXGI_FORMAT_BC6H_UF16:
				return EResourceFormat::bc6_hu16;
			case DXGI_FORMAT_BC6H_SF16:
				return EResourceFormat::bc6_hs16;
			case DXGI_FORMAT_BC7_UNORM:
				return EResourceFormat::bc7_unorm;
			case DXGI_FORMAT_BC7_UNORM_SRGB:
				return EResourceFormat::bc7_unorm_srgb;
			default:
				lupanic();
				return EResourceFormat::unknown;
			}
		}
	}
}

#endif