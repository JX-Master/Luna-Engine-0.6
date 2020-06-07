// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file GraphicAdapter.hpp
* @author JXMaster
* @date 2019/7/17
*/
#pragma once
#include "CommonInclude.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include "GraphicSystemWindows.hpp"
namespace luna
{
	namespace gfx
	{
		namespace win
		{
			class GraphicAdapter : public IGraphicAdapter
			{
			public:
				lucid("{6730655a-5857-4635-8062-987e52708952}");
				luiimpl(GraphicAdapter, IGraphicAdapter, IObject);
				

				GraphicAdapterDesc m_desc;
				ComPtr<IDXGIAdapter1> m_adapter;

				GraphicAdapter() :
					luibind(get_module_allocator()) {}

				void init(IDXGIAdapter1* adapter);
				virtual GraphicAdapterDesc desc() override
				{
					return m_desc;
				}
			};
		}
	}
}

#endif