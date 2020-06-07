// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IShaderInputLayout.hpp
* @author JXMaster
* @date 2019/7/20
*/
#pragma once
#include "IGraphicDeviceChild.hpp"

namespace luna
{
	namespace gfx
	{
		enum class EShaderInputGroupType : uint32
		{
			srv,
			uav,
			cbv,
			sampler
		};

		enum class EShaderVisibility : uint32
		{
			all = 0,
			vertex = 1,
			hull = 2,
			domain = 3,
			geometry = 4,
			pixel = 5
		};

		//! One shader input group is mapped to one continuous range in one view table.
		//! 
		struct ShaderInputGroupDesc
		{
			EShaderInputGroupType type;
			uint32 base_shader_register;
			uint32 num_views;
			EShaderVisibility shader_visibility;

			ShaderInputGroupDesc() = default;
			ShaderInputGroupDesc(
				EShaderInputGroupType _type,
				uint32 _base_shader_register,
				uint32 _num_views,
				EShaderVisibility _shader_visibility) :
				type(_type),
				base_shader_register(_base_shader_register),
				num_views(_num_views),
				shader_visibility(_shader_visibility) {}
		};

		enum class EShaderInputLayoutFlag : uint32
		{
			none = 0,
			//! Input assembler and input layout is used.
			allow_input_assembler_input_layout = 0x01,
			//! Vertex shader cannot access the shader input.
			deny_vertex_shader_access = 0x02,
			//! Hull shader cannot access the shader input.
			deny_hull_shader_access = 0x04,
			//! Domain shader cannot access the shader input.
			deny_domain_shader_access = 0x08,
			//! Geometry shader cannot access the shader input.
			deny_geometry_shader_access = 0x10,
			//! Pixel shader cannot access the shader input.
			deny_pixel_shader_access = 0x20,
			//! Stream output is used.
			allow_stream_output = 0x40,
		};

		//! Describes the shader input layout, that is, how the views are mapped to the 
		//! shader input register and how they exposed to the every shader stage.
		//! get bound to the rendering pipeline and exposed to all shaders.
		struct ShaderInputLayoutDesc
		{
			const ShaderInputGroupDesc* groups;
			uint32 num_groups;
			EShaderInputLayoutFlag flags;

			ShaderInputLayoutDesc() = default;
			ShaderInputLayoutDesc(
				const ShaderInputGroupDesc* _groups,
				uint32 _num_groups,
				EShaderInputLayoutFlag _flags) :
				groups(_groups),
				num_groups(_num_groups),
				flags(_flags) {}
		};

		//! @interface IShaderInputLayout
		//! Describes how shader inputs are accessed by every shader
		//! in the pipeline.
		struct IShaderInputLayout : public IGraphicDeviceChild
		{
			luiid("{347097dc-04e2-44e8-a9a0-3f89e77b4425}");

			// No additional interfaces.
		};
	}
}