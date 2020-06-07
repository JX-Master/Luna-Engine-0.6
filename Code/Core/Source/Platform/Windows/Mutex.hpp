// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Mutex.hpp
* @author JXMaster
* @date 2018/12/22
* @brief Windows implementation of Sync System.
*/
#pragma once
#include "../../CoreHeader.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
namespace luna
{
	namespace win
	{
		class Mutex final : public IMutex
		{
		public:
			lucid("{0df3d468-0d98-4aee-b11d-905ad291def2}");
			luiimpl(Mutex, IMutex, IWaitable, IObject);
			
			CRITICAL_SECTION m_cs;

			Mutex(IAllocator* alloc) :
				luibind(alloc)
			{
				::InitializeCriticalSection(&m_cs);
			}
			~Mutex()
			{
				::DeleteCriticalSection(&m_cs);
			}

			virtual void wait() override
			{
				::EnterCriticalSection(&m_cs);
			}
			virtual RV try_wait() override
			{
				if (::TryEnterCriticalSection(&m_cs))
				{
					return s_ok;
				}
				return e_failure;
			}
			virtual void unlock() override
			{
				::LeaveCriticalSection(&m_cs);
			}
		};
	}
}
#endif
