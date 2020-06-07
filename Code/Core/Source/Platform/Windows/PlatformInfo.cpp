// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformInfo.cpp
* @author JXMaster
* @date 2019/12/26
*/
#include "../../Platform.hpp"
#ifdef LUNA_PLATFORM_WINDOWS
#include <VersionHelpers.h>
namespace luna
{
	namespace platform
	{
		RV get_system_info(ESystemInfo system_info, void* data)
		{
			switch (system_info)
			{
			case ESystemInfo::processors_count:
			{
				SYSTEM_INFO si;
				memzero(&si, sizeof(SYSTEM_INFO));
				::GetSystemInfo(&si);
				*((uint32*)data) = si.dwNumberOfProcessors;
				return s_ok;
			}
			default:
				break;
			}
			return e_not_available;
		}
	}
}

#endif