// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Fiber.hpp
* @author JXMaster
* @date 2018/12/21
*/
#pragma once
#include "../IRunnable.hpp"
#include "../IFiber.hpp"
#include "../Interface.hpp"
#include <Runtime/Platform.hpp>
namespace Luna
{
	class Fiber final : public IFiber
	{
	public:
		lucid("{8b967b57-b251-4ed6-b79d-21d2d1e26474}");
		luiimpl(Fiber, IFiber, IObject);

		handle_t m_fiber;	// The handle of this fiber.
		P<IRunnable> m_callback;
		P<Fiber> m_waiting_fiber;	// The fiber to switch to when yield is called.
		EFiberState m_state;
		bool m_is_native_fiber;	// true if this fiber is created using `convert_thread_to_fiber`


		Fiber() :
			m_is_native_fiber(false),
			m_fiber(NULL),
			m_state(EFiberState::ready) {}

		~Fiber()
		{
			if (m_fiber)
			{
				lucheck_msg((m_state == EFiberState::finished || m_state == EFiberState::ready), "Destroying fiber object when the fiber is not returned. This may cause objects on the fiber call stack being not destructed, trus leading to data inconsistence.");
				if (m_is_native_fiber)
				{
					// Because the threads always hold one reference to the native fiber, this can only happen if the thread is destroyed.
					luassert_always(succeeded(Platform::convert_fiber_to_thread()));
				}
				else
				{
					Platform::delete_fiber(m_fiber);
				}
				m_fiber = NULL;
			}
		}

		virtual EFiberState state() override
		{
			return m_state;
		}
	};
}