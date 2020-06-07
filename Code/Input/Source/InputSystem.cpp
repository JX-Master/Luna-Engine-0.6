// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file InputSystem.cpp
* @author JXMaster
* @date 2020/2/8
*/
#include "InputSystem.hpp"

namespace luna
{
	namespace input
	{
		Unconstructed<Vector<Pair<P<IName>, P<IInputDevice>>>> m_devices;
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
			m_devices.construct(get_global_heap());
			result_t r = platform_input_init();
			if (failed(r))
			{
				m_devices.destruct();
				m_mtx = nullptr;
				return r;
			}
			add_module("Input", deinit);
			return s_ok;
		}

		void deinit()
		{
			m_devices.destruct();
			m_mtx = nullptr;
		}

		void update()
		{
			MutexGuard g(m_mtx);
			remove_empty();
			for (auto& i : m_devices.get())
			{
				i.second->update();
			}
		}

		Vector<P<IName>> get_devices()
		{
			MutexGuard g(m_mtx);
			Vector<P<IName>> names(get_module_allocator());
			remove_empty();
			names.reserve(m_devices.get().size());
			for (auto& i : m_devices.get())
			{
				names.push_back(i.first);
			}
			return names;
		}

		RP<IInputDevice> get_device(IName* device_name)
		{
			luassert_usr(device_name);
			MutexGuard g(m_mtx);
			remove_empty();
			for (auto& i : m_devices.get())
			{
				if (i.first.get() == device_name)
				{
					return i.second;
				}
			}
			return e_item_not_exist;
		}

		RV mount_device(IName* device_name, IInputDevice* device)
		{
			luassert_usr(device_name && device);
			MutexGuard g(m_mtx);
			remove_empty();
			for (auto& i : m_devices.get())
			{
				if (i.first.get() == device_name)
				{
					if (!i.second.empty())
					{
						return e_item_already_exists;
					}
					i.second = device;
					return s_ok;
				}
			}
			m_devices.get().push_back(make_pair(makep(device_name), makewp(device)));
			return s_ok;
		}

		RV unmount_device(IName* device_name)
		{
			luassert_usr(device_name);
			MutexGuard g(m_mtx);
			remove_empty();
			auto iter = m_devices.get().begin();
			while (iter != m_devices.get().end())
			{
				if (iter->first.get() == device_name)
				{
					m_devices.get().erase(iter);
					return s_ok;
				}
				else
				{
					++iter;
				}
			}
			return e_item_not_exist;
		}
	}
}