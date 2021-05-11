// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file InputSystem.cpp
* @author JXMaster
* @date 2020/2/8
*/
#include "InputSystem.hpp"
#include <Core/Core.hpp>
#include <Runtime/Module.hpp>

namespace Luna
{
	namespace Input
	{
		Unconstructed<Vector<Pair<Name, P<IInputDevice>>>> m_devices;
		P<IMutex> m_mtx;
		P<IObject> m_platform_devices;

		void remove_empty()
		{
			auto iter = m_devices.get().begin();
			while (iter != m_devices.get().end())
			{
				if (iter->second.empty())
				{
					iter = m_devices.get().erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}

		RV init()
		{
			m_mtx = new_mutex();
			m_devices.construct();
			RV r = platform_input_init();
			if (!r.valid())
			{
				m_devices.destruct();
				m_mtx = nullptr;
				return r;
			}
			return RV();
		}

		void deinit()
		{
			m_devices.destruct();
			m_mtx = nullptr;
			m_platform_devices = nullptr;
		}

		StaticRegisterModule m(u8"Input", u8"Core", init, deinit);

		void update()
		{
			MutexGuard g(m_mtx);
			remove_empty();
			for (auto& i : m_devices.get())
			{
				i.second->update();
			}
		}

		Vector<Name> get_devices()
		{
			MutexGuard g(m_mtx);
			Vector<Name> names;
			remove_empty();
			names.reserve(m_devices.get().size());
			for (auto& i : m_devices.get())
			{
				names.push_back(i.first);
			}
			return names;
		}

		RP<IInputDevice> get_device(const Name& device_name)
		{
			MutexGuard g(m_mtx);
			remove_empty();
			for (auto& i : m_devices.get())
			{
				if (i.first == device_name)
				{
					return i.second;
				}
			}
			return BasicError::not_found();
		}

		RV mount_device(const Name& device_name, IInputDevice* device)
		{
			lucheck(device);
			MutexGuard g(m_mtx);
			remove_empty();
			for (auto& i : m_devices.get())
			{
				if (i.first == device_name)
				{
					if (!i.second.empty())
					{
						return BasicError::already_exists();
					}
					i.second = device;
					return RV();
				}
			}
			m_devices.get().push_back(make_pair(device_name, makewp(device)));
			return RV();
		}

		RV unmount_device(const Name& device_name)
		{
			MutexGuard g(m_mtx);
			remove_empty();
			auto iter = m_devices.get().begin();
			while (iter != m_devices.get().end())
			{
				if (iter->first == device_name)
				{
					m_devices.get().erase(iter);
					return RV();
				}
				else
				{
					++iter;
				}
			}
			return BasicError::not_found();
		}
	}
}