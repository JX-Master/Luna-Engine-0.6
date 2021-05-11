// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Signal.hpp
* @author JXMaster
* @date 2018/12/22
*/
#pragma once
#include <Runtime/PlatformDefines.hpp>
#include <Core/ISignal.hpp>
#include <Core/Interface.hpp>
#include <Runtime/Platform.hpp>

namespace Luna
{
	class Signal final : public ISignal
	{
	public:
		lucid("{95a2e5b2-d48a-4f19-bfb8-22c273c0ad4b}");
		luiimpl(Signal, ISignal, IWaitable, IObject);

		handle_t m_handle;

		Signal(bool manual_reset)
		{
			m_handle = Platform::new_signal(manual_reset);
		}
		~Signal()
		{
			Platform::delete_signal(m_handle);
			m_handle = nullptr;
		}
		virtual void wait() override
		{
			Platform::wait_signal(m_handle);
		}
		virtual RV try_wait() override
		{
			return Platform::try_wait_signal(m_handle);
		}
		virtual void trigger() override
		{
			Platform::trigger_signal(m_handle);
		}
		virtual void reset() override
		{
			Platform::reset_signal(m_handle);
		}
	};
}