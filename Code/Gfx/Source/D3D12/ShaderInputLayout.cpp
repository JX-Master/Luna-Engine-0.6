// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ShaderInputLayout.cpp
* @author JXMaster
* @date 2020/3/14
*/
#include "ShaderInputLayout.hpp"

#ifdef LUNA_GFX_D3D12

namespace Luna
{
	namespace Gfx
	{
		namespace D3D12
		{
			RV ShaderInputLayout::init(const ShaderInputLayoutDesc& desc)
			{
				m_groups.resize(desc.num_groups);

				for (u32 i = 0; i < desc.num_groups; ++i)
				{
					m_groups[i] = desc.groups[i];
				}

				D3D12_ROOT_SIGNATURE_DESC d;
				d.NumStaticSamplers = 0;
				d.pStaticSamplers = nullptr;
				d.NumParameters = desc.num_groups;
				D3D12_ROOT_PARAMETER* params = (D3D12_ROOT_PARAMETER*)alloca(sizeof(D3D12_ROOT_PARAMETER) * desc.num_groups);
				d.pParameters = params;
				u32 ranges_count = desc.num_groups;
				D3D12_DESCRIPTOR_RANGE* ranges = (D3D12_DESCRIPTOR_RANGE*)alloca(sizeof(D3D12_DESCRIPTOR_RANGE) * ranges_count);
				// for every group (view table).
				for (u32 i = 0; i < desc.num_groups; ++i)
				{
					// One range per group.
					const ShaderInputGroupDesc& group = desc.groups[i];
					D3D12_ROOT_PARAMETER& param = params[i];
					param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
					param.DescriptorTable.NumDescriptorRanges = 1;
					param.DescriptorTable.pDescriptorRanges = &(ranges[i]);
					// for every range.
					D3D12_DESCRIPTOR_RANGE& dest_range = ranges[i];
					dest_range.BaseShaderRegister = group.base_shader_register;
					dest_range.NumDescriptors = group.num_views;
					dest_range.OffsetInDescriptorsFromTableStart = 0;
					switch (group.type)
					{
					case EShaderInputGroupType::cbv:
						dest_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
						break;
					case EShaderInputGroupType::sampler:
						dest_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
						break;
					case EShaderInputGroupType::srv:
						dest_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
						break;
					case EShaderInputGroupType::uav:
						dest_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
						break;
					default:
						lupanic();
						break;
					}
					dest_range.RegisterSpace = 0;
					switch (group.shader_visibility)
					{
					case EShaderVisibility::all:
						param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
						break;
					case EShaderVisibility::pixel:
						param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
						break;
					case EShaderVisibility::vertex:
						param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
						break;
					case EShaderVisibility::domain:
						param.ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
						break;
					case EShaderVisibility::geometry:
						param.ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
						break;
					case EShaderVisibility::hull:
						param.ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
						break;
					default:
						lupanic();
						break;
					}
				}
				d.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
				if ((desc.flags & EShaderInputLayoutFlag::allow_input_assembler_input_layout) != EShaderInputLayoutFlag::none)
				{
					d.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
				}
				if ((desc.flags & EShaderInputLayoutFlag::allow_stream_output) != EShaderInputLayoutFlag::none)
				{
					d.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
				}
				if ((desc.flags & EShaderInputLayoutFlag::deny_domain_shader_access) != EShaderInputLayoutFlag::none)
				{
					d.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
				}
				if ((desc.flags & EShaderInputLayoutFlag::deny_geometry_shader_access) != EShaderInputLayoutFlag::none)
				{
					d.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
				}
				if ((desc.flags & EShaderInputLayoutFlag::deny_hull_shader_access) != EShaderInputLayoutFlag::none)
				{
					d.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
				}
				if ((desc.flags & EShaderInputLayoutFlag::deny_pixel_shader_access) != EShaderInputLayoutFlag::none)
				{
					d.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
				}
				if ((desc.flags & EShaderInputLayoutFlag::deny_vertex_shader_access) != EShaderInputLayoutFlag::none)
				{
					d.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
				}

				ComPtr<ID3DBlob> b;
				ComPtr<ID3DBlob> err;
				if (FAILED(D3D12SerializeRootSignature(&d, D3D_ROOT_SIGNATURE_VERSION_1_0, b.GetAddressOf(), err.GetAddressOf())))
				{
					get_error_object() = Error(BasicError::bad_system_call(), "Failed to create D3D12 root signature: %s", err->GetBufferPointer());
					return BasicError::error_object();
				}
				if (FAILED(m_device->m_device->CreateRootSignature(0, b->GetBufferPointer(), b->GetBufferSize(), IID_PPV_ARGS(&m_rs))))
				{
					get_error_object() = Error(BasicError::bad_system_call(), "Failed to create D3D12 root signature.");
					return BasicError::error_object();
				}
				return RV();
			}
		}
	}
}

#endif