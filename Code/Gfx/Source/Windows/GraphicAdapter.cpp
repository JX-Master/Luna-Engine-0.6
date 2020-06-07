// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file GraphicAdapter.cpp
* @author JXMaster
* @date 2019/7/17
*/
#include "GraphicAdapter.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include "GraphicSystemWindows.hpp"

namespace luna
{
	namespace gfx
	{
		namespace win
		{
			void GraphicAdapter::init(IDXGIAdapter1* adapter)
			{
				m_adapter = adapter;
				DXGI_ADAPTER_DESC1 desc;
				m_adapter->GetDesc1(&desc);
				utf16_to_utf8(m_desc.name, 256, (char16_t*)desc.Description);
				m_desc.local_memory = desc.DedicatedSystemMemory + desc.DedicatedVideoMemory;
				m_desc.shared_memory = desc.SharedSystemMemory;
				m_desc.flags = EGraphicAdapterFlag::none;
				if (desc.DedicatedVideoMemory)
				{
					m_desc.flags |= EGraphicAdapterFlag::dedicated;
				}
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					m_desc.flags |= EGraphicAdapterFlag::software;
				}
			}
		}
	}
}

#endif