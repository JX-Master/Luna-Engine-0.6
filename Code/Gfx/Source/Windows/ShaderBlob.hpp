// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ShaderBlob.hpp
* @author JXMaster
* @date 2019/9/28
*/
#pragma once

#include "../GfxHeader.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include "GraphicSystemWindows.hpp"
#include <d3dcompiler.h>

namespace luna
{
	namespace gfx
	{
		namespace win
		{
			class ShaderBlob : public IBlob
			{
			public:
				lucid("{6b83b3da-e851-41ed-b5f1-165e510fc616}");
				luiimpl(ShaderBlob, IBlob, IObject);
				

				ComPtr<ID3DBlob> m_shader;

				ShaderBlob() :
					luibind(get_module_allocator()) {}

				virtual const void* data() override
				{
					return m_shader->GetBufferPointer();
				}
				virtual size_t size() override
				{
					return m_shader->GetBufferSize();
				}
			};
		}
	}
}

#endif