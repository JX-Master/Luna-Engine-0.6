// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Mutex.hpp
* @author JXMaster
* @date 2018/12/22
*/
#pragma once
#include <Runtime/PlatformDefines.hpp>
#include <Core/IMutex.hpp>
#include <Core/Interface.hpp>
#include <Runtime/Platform.hpp>

namespace Luna
{
	class Mutex final : public IMutex
	{
	public:
		lucid("{0df3d468-0d98-4aee-b11d-905ad291def2}");
		luiimpl(Mutex, IMutex, IWaitable, IObject);

		handle_t m_handle;

		Mutex()
		{
			m_handle = Platform::new_mutex();
		}
		~Mutex()
		{
			Platform::delete_mutex(m_handle);
		}
		virtual void wait() override
		{
			Platform::lock_mutex(m_handle);
		}
		virtual RV try_wait() override
		{
			return Platform::try_lock_mutex(m_handle);
		}
		virtual void unlock() override
		{
			Platform::unlock_mutex(m_handle);
		}
	};
}
