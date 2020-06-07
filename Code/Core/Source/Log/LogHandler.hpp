// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file LogHandler.hpp
* @author JXMaster
* @date 2019/12/23
*/
#pragma once
#include "LogSystem.hpp"
#include <Base/Vector.hpp>
#include <Base/TSAssert.hpp>
namespace luna
{
	class LogHandler : public ILogHandler
	{
	public:
		lucid("{167aa071-f966-46c1-a92c-840ef3d7b8f6}");
		luiimpl(LogHandler, ILogHandler, IObject);
		lutsassert_lock();

		P<IReadWriteLock> m_mtx;
		Vector<P<IName>> m_channels;
		P<IName> m_name;
		WP<ILogCallback> m_callback;
		ELogType m_pass_types;
		bool m_enabled;
		bool m_listen_all;

		LogHandler(IAllocator* alloc) :
			luibind(alloc),
			m_channels(alloc) {}
		~LogHandler();

		virtual IName* name() override
		{
			return m_name;
		}
		virtual bool valid() override
		{
			return !m_callback.empty();
		}
		virtual ELogType pass_types() override
		{
			ReadGuard mtx(m_mtx.get());
			return m_pass_types;
		}
		virtual void set_pass_types(ELogType pass_types) override
		{
			WriteGuard mtx(m_mtx.get());
			m_pass_types = pass_types;
		}
		virtual bool enabled() override
		{
			ReadGuard mtx(m_mtx.get());
			return m_enabled;
		}
		virtual void set_enabled(bool enabled) override
		{
			WriteGuard mtx(m_mtx.get());
			m_enabled = enabled;
		}
		virtual bool listening_all() override
		{
			ReadGuard mtx(m_mtx.get());
			return m_listen_all;
		}
		virtual void set_listening_all(bool listen_all) override
		{
			WriteGuard mtx(m_mtx.get());
			m_listen_all = listen_all;
		}
		virtual P<ILogCallback> callback() override
		{
			return m_callback.lock();
		}
		virtual R<Vector<P<IName>>> channels() override;
		virtual void set_channels(const Vector<P<IName>>& channels) override;
		virtual void remove_all_channels() override
		{
			WriteGuard mtx(m_mtx.get());
			m_channels.clear();
		}
	};
}