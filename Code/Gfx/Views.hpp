// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Views.hpp
* @author JXMaster
* @date 2019/1/29
* @brief Definition of all resource views/descriptors.
*/
#pragma once
#include "GraphicDefines.hpp"

namespace luna
{
	namespace gfx
	{
		namespace rtv
		{
			struct Buffer
			{
				//! Number of bytes between the beginning of the buffer and the first element to access.
				uint64 offset;
				//! The total number of elements in the view.
				uint32 count;
			};
			struct Tex1D
			{
				uint32 mip_slice;	
			};
			struct Tex1DArray
			{
				uint32 mip_slice;
				uint32 first_array_slice;
				uint32 array_size;
			};
			struct Tex2D
			{
				uint32 mip_slice;
			};
			struct Tex2DArray
			{
				uint32 mip_slice;
				uint32 first_array_slice;
				uint32 array_size;
			};
			struct Tex2DMS {};
			struct Tex2DMSArray
			{
				uint32 first_array_slice;
				uint32 array_size;
			};
			struct Tex3D
			{
				uint32 mip_slice;
				uint32 first_layer_slice;
				uint32 layer_size;
			};
		}

		enum class ERenderTargetViewType : uint32
		{
			unknown = 0,
			buffer,
			tex1d,
			tex1darray,
			tex2d,
			tex2darray,
			tex2dms,
			tex2dmsarray,
			tex3d
		};

		struct RenderTargetViewDesc
		{
			EResourceFormat format;
			ERenderTargetViewType type;
			union 
			{
				rtv::Buffer buffer;
				rtv::Tex1D tex1d;
				rtv::Tex1DArray tex1darray;
				rtv::Tex2D tex2d;
				rtv::Tex2DArray tex2darray;
				rtv::Tex2DMS tex2dms;
				rtv::Tex2DMSArray tex2dmsarray;
				rtv::Tex3D tex3d;
			};

			static RenderTargetViewDesc as_buffer(EResourceFormat _format, uint64 _offset, uint32 _count)
			{
				RenderTargetViewDesc desc;
				desc.format = _format;
				desc.type = ERenderTargetViewType::buffer;
				desc.buffer.offset = _offset;
				desc.buffer.count = _count;
				return desc;
			}

			static RenderTargetViewDesc as_tex1d(EResourceFormat _format, uint32 _mip_slice)
			{
				RenderTargetViewDesc desc;
				desc.format = _format;
				desc.type = ERenderTargetViewType::tex1d;
				desc.tex1d.mip_slice = _mip_slice;
				return desc;
			}

			static RenderTargetViewDesc as_tex1darray(EResourceFormat _format, uint32 _mip_slice, uint32 _first_array_slice, uint32 _array_size)
			{
				RenderTargetViewDesc desc;
				desc.format = _format;
				desc.type = ERenderTargetViewType::tex1darray;
				desc.tex1darray.array_size = _array_size;
				desc.tex1darray.first_array_slice = _first_array_slice;
				desc.tex1darray.mip_slice = _mip_slice;
				return desc;
			}

			static RenderTargetViewDesc as_tex2d(EResourceFormat _format, uint32 _mip_slice)
			{
				RenderTargetViewDesc desc;
				desc.format = _format;
				desc.type = ERenderTargetViewType::tex2d;
				desc.tex2d.mip_slice = _mip_slice;
				return desc;
			}

			static RenderTargetViewDesc as_tex2darray(EResourceFormat _format, uint32 _mip_slice, uint32 _first_array_slice, uint32 _array_size)
			{
				RenderTargetViewDesc desc;
				desc.format = _format;
				desc.type = ERenderTargetViewType::tex2darray;
				desc.tex2darray.array_size = _array_size;
				desc.tex2darray.first_array_slice = _first_array_slice;
				desc.tex2darray.mip_slice = _mip_slice;
				return desc;
			}

			static RenderTargetViewDesc as_tex2dms(EResourceFormat _format)
			{
				RenderTargetViewDesc desc;
				desc.format = _format;
				desc.type = ERenderTargetViewType::tex2dms;
				return desc;
			}

			static RenderTargetViewDesc as_tex2dmsarray(EResourceFormat _format, uint32 _first_array_slice, uint32 _array_size)
			{
				RenderTargetViewDesc desc;
				desc.format = _format;
				desc.type = ERenderTargetViewType::tex2dmsarray;
				desc.tex2dmsarray.first_array_slice = _first_array_slice;
				desc.tex2dmsarray.array_size = _array_size;
				return desc;
			}

			static RenderTargetViewDesc as_tex3d(EResourceFormat _format, uint32 _mip_slice, uint32 _first_layer_slice, uint32 _layer_size)
			{
				RenderTargetViewDesc desc;
				desc.format = _format;
				desc.type = ERenderTargetViewType::tex3d;
				desc.tex3d.first_layer_slice = _first_layer_slice;
				desc.tex3d.layer_size = _layer_size;
				desc.tex3d.mip_slice = _mip_slice;
				return desc;
			}
		};

		namespace srv
		{
			struct Buffer
			{
				//! The index of the first element to be accessed by the view.
				uint64 offset;
				//! The number of elements in the resource.
				uint32 count;
				//! The size of each element in the buffer structure (in bytes) when the buffer represents a structured buffer.
				//! This can be ignored for typed 
				uint32 element_size;
				//! Indicates the buffer is a byte address buffer ( raw view buffer ).
				bool raw_view;
			};
			struct Tex1D
			{
				//! The most detailed mip level to use.
				uint32  most_detailed_mip;		
				//! The mip levels to use. Set to -1 to indicate all the mipmap levels from `most_detailed_mip` on down to least detailed. 
				//! Available mip levels are [most_detailed, most_detailed + mip_levels - 1].
				uint32  mip_levels;
				//! A value to clamp sample LOD values to. 
				//! For example, if you specify 2.0f for the clamp value, you ensure that no individual sample accesses a mip level 
				//! less than 2.0f.
				float32 resource_min_lod_clamp;	
			};
			struct Tex1DArray
			{
				uint32  most_detailed_mip;
				uint32  mip_levels;
				//! The index of the first texture to use in an array of textures.
				uint32  first_array_slice;
				//! Number of textures in the array.
				uint32  array_size;
				float32 resource_min_lod_clamp;
			};
			struct Tex2D
			{
				uint32  most_detailed_mip;
				uint32  mip_levels;
				float32 resource_min_lod_clamp;
			};
			struct Tex2DArray
			{
				uint32  most_detailed_mip;
				uint32  mip_levels;
				uint32  first_array_slice;
				uint32  array_size;
				float32 resource_min_lod_clamp;
			};
			struct Tex2DMS {};
			struct Tex2DMSArray
			{
				uint32  first_array_slice;
				uint32  array_size;
			};
			struct Tex3D
			{
				uint32  most_detailed_mip;
				uint32  mip_levels;
				float32 resource_min_lod_clamp;
			};
			struct TexCube
			{
				uint32  most_detailed_mip;
				uint32  mip_levels;
				float32 resource_min_lod_clamp;
			};
			struct TexCubeArray
			{
				uint32  most_detailed_mip;
				uint32  mip_levels;
				uint32  first_2darray_face;
				uint32  num_cubes;
				float32 resource_min_lod_clamp;
			};
		}

		enum class EShaderResourceViewType
		{
			unknown = 0,
			buffer,
			tex1d,
			tex1darray,
			tex2d,
			tex2darray,
			tex2dms,
			tex2dmsarray,
			tex3d,
			texcube,
			texcubearray
		};

		struct ShaderResourceViewDesc
		{
			EResourceFormat format;
			EShaderResourceViewType type;
			union
			{
				srv::Buffer buffer;
				srv::Tex1D tex1d;
				srv::Tex1DArray tex1darray;
				srv::Tex2D tex2d;
				srv::Tex2DArray tex2darray;
				srv::Tex2DMS tex2dms;
				srv::Tex2DMSArray tex2dmsarray;
				srv::Tex3D tex3d;
				srv::TexCube texcube;
				srv::TexCubeArray texcubearray;
			};

			static ShaderResourceViewDesc as_buffer(EResourceFormat _format, uint64 _offset, uint32 _count, uint32 _element_size, bool _raw_view = false)
			{
				ShaderResourceViewDesc desc;
				desc.format = _format;
				desc.type = EShaderResourceViewType::buffer;
				desc.buffer.offset = _offset;
				desc.buffer.count = _count;
				desc.buffer.raw_view = _raw_view;
				desc.buffer.element_size = _element_size;
				return desc;
			}

			static ShaderResourceViewDesc as_tex1d(EResourceFormat _format, uint32 _most_detailed_mip, uint32 _mip_levels, float32 _resource_min_lod_clamp)
			{
				ShaderResourceViewDesc desc;
				desc.format = _format;
				desc.type = EShaderResourceViewType::tex1d;
				desc.tex1d.mip_levels = _mip_levels;
				desc.tex1d.most_detailed_mip = _most_detailed_mip;
				desc.tex1d.resource_min_lod_clamp = _resource_min_lod_clamp;
				return desc;
			}

			static ShaderResourceViewDesc as_tex1darray(EResourceFormat _format, uint32 _most_detailed_mip, uint32 _mip_levels, uint32 _first_array_slice, uint32 _array_size, float32 _resource_min_lod_clamp)
			{
				ShaderResourceViewDesc desc;
				desc.format = _format;
				desc.type = EShaderResourceViewType::tex1darray;
				desc.tex1darray.most_detailed_mip = _most_detailed_mip;
				desc.tex1darray.mip_levels = _mip_levels;
				desc.tex1darray.array_size = _array_size;
				desc.tex1darray.first_array_slice = _first_array_slice;
				desc.tex1darray.resource_min_lod_clamp = _resource_min_lod_clamp;
				return desc;
			}

			static ShaderResourceViewDesc as_tex2d(EResourceFormat _format, uint32  _most_detailed_mip, uint32  _mip_levels, float32 _resource_min_lod_clamp)
			{
				ShaderResourceViewDesc desc;
				desc.format = _format;
				desc.type = EShaderResourceViewType::tex2d;
				desc.tex2d.most_detailed_mip = _most_detailed_mip;
				desc.tex2d.mip_levels = _mip_levels;
				desc.tex2d.resource_min_lod_clamp = _resource_min_lod_clamp;
				return desc;
			}

			static ShaderResourceViewDesc as_tex2darray(EResourceFormat _format, uint32 _most_detailed_mip, uint32 _mip_levels, uint32 _first_array_slice, uint32 _array_size, float32 _resource_min_lod_clamp)
			{
				ShaderResourceViewDesc desc;
				desc.format = _format;
				desc.type = EShaderResourceViewType::tex2darray;
				desc.tex2darray.most_detailed_mip = _most_detailed_mip;
				desc.tex2darray.mip_levels = _mip_levels;
				desc.tex2darray.first_array_slice = _first_array_slice;
				desc.tex2darray.array_size = _array_size;
				desc.tex2darray.resource_min_lod_clamp = _resource_min_lod_clamp;
				return desc;
			}

			static ShaderResourceViewDesc as_tex2dms(EResourceFormat _format)
			{
				ShaderResourceViewDesc desc;
				desc.format = _format;
				desc.type = EShaderResourceViewType::tex2dms;
				return desc;
			}

			static ShaderResourceViewDesc as_tex2dmsarray(EResourceFormat _format, uint32 _first_array_slice, uint32 _array_size)
			{
				ShaderResourceViewDesc desc;
				desc.format = _format;
				desc.type = EShaderResourceViewType::tex2dmsarray;
				desc.tex2dmsarray.first_array_slice = _first_array_slice;
				desc.tex2dmsarray.array_size = _array_size;
				return desc;
			}

			static ShaderResourceViewDesc as_tex3d(EResourceFormat _format, uint32 _most_detailed_mip, uint32 _mip_levels, float32 _resource_min_lod_clamp)
			{
				ShaderResourceViewDesc desc;
				desc.format = _format;
				desc.type = EShaderResourceViewType::tex3d;
				desc.tex3d.most_detailed_mip = _most_detailed_mip;
				desc.tex3d.mip_levels = _mip_levels;
				desc.tex3d.resource_min_lod_clamp = _resource_min_lod_clamp;
				return desc;
			}

			static ShaderResourceViewDesc as_texcube(EResourceFormat _format, uint32 _most_detailed_mip, uint32 _mip_levels, float32 _resource_min_lod_clamp)
			{
				ShaderResourceViewDesc desc;
				desc.format = _format;
				desc.type = EShaderResourceViewType::texcube;
				desc.texcube.mip_levels = _mip_levels;
				desc.texcube.most_detailed_mip = _most_detailed_mip;
				desc.texcube.resource_min_lod_clamp = _resource_min_lod_clamp;
				return desc;
			}

			static ShaderResourceViewDesc as_texcubearray(EResourceFormat _format, uint32 _most_detailed_mip, uint32 _mip_levels, uint32 _first_2d_array_face, uint32 _num_cubes, float32 _resource_min_lod_clamp)
			{
				ShaderResourceViewDesc desc;
				desc.format = _format;
				desc.type = EShaderResourceViewType::texcubearray;
				desc.texcubearray.first_2darray_face = _first_2d_array_face;
				desc.texcubearray.mip_levels = _mip_levels;
				desc.texcubearray.most_detailed_mip = _most_detailed_mip;
				desc.texcubearray.num_cubes = _num_cubes;
				desc.texcubearray.resource_min_lod_clamp = _resource_min_lod_clamp;
				return desc;
			}
		};

		namespace uav
		{
			struct Buffer
			{
				uint64 offset;
				uint32 count;
				uint32 element_size;
				uint64 counter_offset_in_bytes;
				bool raw_view;
			};
			struct Tex1D
			{
				uint32 mip_slice;
			};
			struct Tex1DArray
			{
				uint32 mip_slice;
				uint32 first_array_slice;
				uint32 array_size;
			};
			struct Tex2D
			{
				uint32 mip_slice;
			};
			struct Tex2DArray
			{
				uint32 mip_slice;
				uint32 first_array_slice;
				uint32 array_size;
			};
			struct Tex3D
			{
				uint32 mip_slice;
				uint32 first_layer_slice;
				uint32 layer_size;
			};
		}

		enum class EUnorderedAccessViewType
		{
			unknown = 0,
			buffer,
			tex1d,
			tex1darray,
			tex2d,
			tex2darray,
			tex3d
		};

		struct UnorderedAccessViewDesc
		{
			EResourceFormat format;
			EUnorderedAccessViewType type;
			union
			{
				uav::Buffer buffer;
				uav::Tex1D tex1d;
				uav::Tex1DArray tex1darray;
				uav::Tex2D tex2d;
				uav::Tex2DArray tex2darray;
				uav::Tex3D tex3d;
			};

			static UnorderedAccessViewDesc as_buffer(EResourceFormat _format, uint64 _offset, uint32 _count, uint32 _element_size, uint64 _counter_offset_in_bytes, bool _raw_view)
			{
				UnorderedAccessViewDesc desc;
				desc.format = _format;
				desc.type = EUnorderedAccessViewType::buffer;
				desc.buffer.offset = _offset;
				desc.buffer.count = _count;
				desc.buffer.element_size = _element_size;
				desc.buffer.counter_offset_in_bytes = _counter_offset_in_bytes;
				desc.buffer.raw_view = _raw_view;
				return desc;
			}

			static UnorderedAccessViewDesc as_tex1d(EResourceFormat _format, uint32 _mip_slice)
			{
				UnorderedAccessViewDesc desc;
				desc.format = _format;
				desc.type = EUnorderedAccessViewType::tex1d;
				desc.tex1d.mip_slice = _mip_slice;
				return desc;
			}

			static UnorderedAccessViewDesc as_tex1darray(EResourceFormat _format, uint32 _mip_slice, uint32 _first_array_slice, uint32 _array_size)
			{
				UnorderedAccessViewDesc desc;
				desc.format = _format;
				desc.type = EUnorderedAccessViewType::tex1darray;
				desc.tex1darray.array_size = _array_size;
				desc.tex1darray.first_array_slice = _first_array_slice;
				desc.tex1darray.mip_slice = _mip_slice;
				return desc;
			}

			static UnorderedAccessViewDesc as_tex2d(EResourceFormat _format, uint32 _mip_slice)
			{
				UnorderedAccessViewDesc desc;
				desc.format = _format;
				desc.type = EUnorderedAccessViewType::tex2d;
				desc.tex2d.mip_slice = _mip_slice;
				return desc;
			}

			static UnorderedAccessViewDesc as_tex2darray(EResourceFormat _format, uint32 _mip_slice, uint32 _first_array_slice, uint32 _array_size)
			{
				UnorderedAccessViewDesc desc;
				desc.format = _format;
				desc.type = EUnorderedAccessViewType::tex2darray;
				desc.tex2darray.array_size = _array_size;
				desc.tex2darray.first_array_slice = _first_array_slice;
				desc.tex2darray.mip_slice = _mip_slice;
				return desc;
			}

			static UnorderedAccessViewDesc as_tex3d(EResourceFormat _format, uint32 _mip_slice, uint32 _first_layer_slice, uint32 _layer_size)
			{
				UnorderedAccessViewDesc desc;
				desc.format = _format;
				desc.type = EUnorderedAccessViewType::tex3d;
				desc.tex3d.first_layer_slice = _first_layer_slice;
				desc.tex3d.layer_size = _layer_size;
				desc.tex3d.mip_slice = _mip_slice;
				return desc;
			}
		};

		namespace dsv
		{
			struct Tex1D
			{
				uint32 mip_slice;
			};
			struct Tex1DArray
			{
				uint32 mip_slice;
				uint32 first_array_slice;
				uint32 array_size;
			};
			struct Tex2D
			{
				uint32 mip_slice;
			};
			struct Tex2DArray
			{
				uint32 mip_slice;
				uint32 first_array_slice;
				uint32 array_size;
			};
			struct Tex2DMS {};
			struct Tex2DMSArray
			{
				uint32  first_array_slice;
				uint32  array_size;
			};
		}

		enum class EDepthStencilViewType
		{
			unknown = 0,
			tex1d,
			tex1darray,
			tex2d,
			tex2darray,
			tex2dms,
			tex2dmsarray
		};

		struct DepthStencilViewDesc
		{
			EResourceFormat format;
			EDepthStencilViewType type;
			bool depth_read_only;
			bool stencil_read_only;
			union
			{
				dsv::Tex1D tex1d;
				dsv::Tex1DArray tex1darray;
				dsv::Tex2D tex2d;
				dsv::Tex2DArray tex2darray;
				dsv::Tex2DMS tex2dms;
				dsv::Tex2DMSArray tex2dmsarray;
			};

			static DepthStencilViewDesc as_tex1d(EResourceFormat _format, uint32 _mip_slice, bool _depth_read_only = false, bool _stencil_read_only = false)
			{
				DepthStencilViewDesc desc;
				desc.format = _format;
				desc.type = EDepthStencilViewType::tex1d;
				desc.tex1d.mip_slice = _mip_slice;
				desc.depth_read_only = _depth_read_only;
				desc.stencil_read_only = _stencil_read_only;
				return desc;
			}

			static DepthStencilViewDesc as_tex1darray(EResourceFormat _format, uint32 _mip_slice, uint32 _first_array_slice, uint32 _array_size, bool _depth_read_only = false, bool _stencil_read_only = false)
			{
				DepthStencilViewDesc desc;
				desc.format = _format;
				desc.type = EDepthStencilViewType::tex1darray;
				desc.tex1darray.array_size = _array_size;
				desc.tex1darray.first_array_slice = _first_array_slice;
				desc.tex1darray.mip_slice = _mip_slice;
				desc.depth_read_only = _depth_read_only;
				desc.stencil_read_only = _stencil_read_only;
				return desc;
			}

			static DepthStencilViewDesc as_tex2d(EResourceFormat _format, uint32 _mip_slice, bool _depth_read_only = false, bool _stencil_read_only = false)
			{
				DepthStencilViewDesc desc;
				desc.format = _format;
				desc.type = EDepthStencilViewType::tex2d;
				desc.tex2d.mip_slice = _mip_slice;
				desc.depth_read_only = _depth_read_only;
				desc.stencil_read_only = _stencil_read_only;
				return desc;
			}

			static DepthStencilViewDesc as_tex2darray(EResourceFormat _format, uint32 _mip_slice, uint32 _first_array_slice, uint32 _array_size, bool _depth_read_only = false, bool _stencil_read_only = false)
			{
				DepthStencilViewDesc desc;
				desc.format = _format;
				desc.type = EDepthStencilViewType::tex2darray;
				desc.tex2darray.array_size = _array_size;
				desc.tex2darray.first_array_slice = _first_array_slice;
				desc.tex2darray.mip_slice = _mip_slice;
				desc.depth_read_only = _depth_read_only;
				desc.stencil_read_only = _stencil_read_only;
				return desc;
			}

			static DepthStencilViewDesc as_tex2dms(EResourceFormat _format, bool _depth_read_only = false, bool _stencil_read_only = false)
			{
				DepthStencilViewDesc desc;
				desc.format = _format;
				desc.type = EDepthStencilViewType::tex2dms;
				desc.depth_read_only = _depth_read_only;
				desc.stencil_read_only = _stencil_read_only;
				return desc;
			}

			static DepthStencilViewDesc as_tex2dmsarray(EResourceFormat _format, uint32 _first_array_slice, uint32 _array_size, bool _depth_read_only = false, bool _stencil_read_only = false)
			{
				DepthStencilViewDesc desc;
				desc.format = _format;
				desc.type = EDepthStencilViewType::tex2dmsarray;
				desc.tex2dmsarray.first_array_slice = _first_array_slice;
				desc.tex2dmsarray.array_size = _array_size;
				desc.depth_read_only = _depth_read_only;
				desc.stencil_read_only = _stencil_read_only;
				return desc;
			}
		};

		struct IResource;
		
		struct VertexBufferViewDesc
		{
			IResource* resource;
			//! The offset of the buffer from resource start in bytes.
			uint64 offset_in_bytes;
			//! The size of the resource in bytes.
			uint32 size_in_bytes;
			//! The size of one element in the buffer in bytes.
			uint32 stride_in_bytes;

			VertexBufferViewDesc() = default;
			VertexBufferViewDesc(IResource* _resource,
				uint64 _offset_in_bytes,
				uint32 _size_in_bytes,
				uint32 _stride_in_bytes) :
				resource(_resource),
				offset_in_bytes(_offset_in_bytes),
				size_in_bytes(_size_in_bytes),
				stride_in_bytes(_stride_in_bytes) {}
		};

		struct ConstantBufferViewDesc
		{
			uint64 offset;
			uint32 size;

			ConstantBufferViewDesc() = default;
			ConstantBufferViewDesc(uint64 _offset, uint32 _size) :
				offset(_offset), size(_size) {}
		};

		enum class EFilter : uint32
		{
			min_mag_mip_point,
			min_mag_point_mip_linear,
			min_point_mag_linear_mip_point,
			min_point_mag_mip_linear,
			min_linear_mag_mip_point,
			min_linear_mag_point_mip_linear,
			min_mag_linear_mip_point,
			min_mag_mip_linear,
			anisotropic,
			comparison_min_mag_mip_point,
			comparison_min_mag_point_mip_linear,
			comparison_min_point_mag_linear_mip_point,
			comparison_min_point_mag_mip_linear,
			comparison_min_linear_mag_mip_point,
			comparison_min_linear_mag_point_mip_linear,
			comparison_min_mag_linear_mip_point,
			comparison_min_mag_mip_linear,
			comparison_anisotropic,
			minimum_min_mag_mip_point,
			minimum_min_mag_point_mip_linear,
			minimum_min_point_mag_linear_mip_point,
			minimum_min_point_mag_mip_linear,
			minimum_min_linear_mag_mip_point,
			minimum_min_linear_mag_point_mip_linear,
			minimum_min_mag_linear_mip_point,
			minimum_min_mag_mip_linear,
			minimum_anisotropic,
			maximum_min_mag_mip_point,
			maximum_min_mag_point_mip_linear,
			maximum_min_point_mag_linear_mip_point,
			maximum_min_point_mag_mip_linear,
			maximum_min_linear_mag_mip_point,
			maximum_min_linear_mag_point_mip_linear,
			maximum_min_mag_linear_mip_point,
			maximum_min_mag_mip_linear,
			maximum_anisotropic
		};

		enum class ETextureAddressMode : uint32
		{
			warp,
			mirror,
			clamp,
			border,
			mirror_once
		};

		struct SamplerDesc
		{
			EFilter filter;
			ETextureAddressMode address_u;
			ETextureAddressMode address_v;
			ETextureAddressMode address_w;
			float32 mip_lod_bias;
			uint32 max_anisotropy;
			EComparisonFunc comparison_func;
			float32 border_color[4];
			float32 min_lod;
			float32 max_lod;

			SamplerDesc() = default;
			SamplerDesc(EFilter _filter,
				ETextureAddressMode _address_u,
				ETextureAddressMode _address_v,
				ETextureAddressMode _address_w,
				float32 _mip_lod_bias = 0.0f,
				uint32 _max_anisotropy = 1,
				EComparisonFunc _comparison_func = EComparisonFunc::always,
				float32 _border_color[4] = nullptr,
				float32 _min_lod = 0.0f,
				float32 _max_lod = FLT_MAX) :
				filter(_filter),
				address_u(_address_u),
				address_v(_address_v),
				address_w(_address_w),
				mip_lod_bias(_mip_lod_bias),
				max_anisotropy(_max_anisotropy),
				comparison_func(_comparison_func),
				min_lod(_min_lod),
				max_lod(_max_lod)
			{
				if (_border_color)
				{
					border_color[0] = _border_color[0];
					border_color[1] = _border_color[1];
					border_color[2] = _border_color[2];
					border_color[3] = _border_color[3];
				}
				else
				{
					border_color[0] = 0.0f;
					border_color[1] = 0.0f;
					border_color[2] = 0.0f;
					border_color[3] = 0.0f;
				}
			}
			bool operator==(const SamplerDesc& rhs)
			{
				return !memcmp(this, &rhs, sizeof(SamplerDesc));
			}
			bool operator!= (const SamplerDesc& rhs)
			{
				return !(*this == rhs);
			}
		};

		struct StreamOutputBufferView
		{
			IResource* soresource;
			uint64 offset_in_bytes;
			uint64 size_in_bytes;
			IResource* buffer_filled_size_resource;
			uint64 buffer_filled_size_offset;
		};
	}
}