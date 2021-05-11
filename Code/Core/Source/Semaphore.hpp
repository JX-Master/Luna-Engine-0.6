// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Semaphore.hpp
* @author JXMaster
* @date 2018/12/22
*/
#pragma once
#include <Runtime/PlatformDefines.hpp>
#include <Core/ISemaphore.hpp>
#include <Core/Interface.hpp>
#include <Runtime/Platform.hpp>

namespace Luna
{
	class Semaphore final : public ISemaphore
	{
	public:
		lucid("{4d155da3-acdb-4ac6-aecb-70e43a5faedf}");
		luiimpl(Semaphore, ISemaphore, IWaitable, IObject);

		handle_t m_handle;

		Semaphore(i32 initial_count, i32 max_count)
		{
			m_handle = Platform::new_semaphore(initial_count, max_count);
		}
		~Semaphore()
		{
			Platform::delete_semaphore(m_handle);
		}

		virtual void wait() override
		{
			Platform::acquire_semaphore(m_handle);
		}
		virtual RV try_wait() override
		{
			return Platform::try_acquire_semaphore(m_handle);
		}
		virtual void unacquire(i32 release_count) override
		{
			Platform::release_semaphore(m_handle);
		}
	};
}