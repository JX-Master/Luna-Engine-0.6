// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ILogHandler.hpp
* @author JXMaster
* @date 2018/12/11
*/
#pragma once
#include "ILogCallback.hpp"
#include "IVariant.hpp"
#include <Base/Vector.hpp>

namespace luna
{

	//! @interface ILogHandler
	//! @threadsafe
	//! Represents a log handler that accepts log.
	struct ILogHandler : public IObject
	{
		luiid("{2c789a3d-d670-4fc3-8a50-1cc3e9a2722a}");

		//! Gets the name of this handler.
		virtual IName* name() = 0;

		//! The handler is valid so long as the delegate object is not expired.
		virtual bool valid() = 0;

		//! A combination of the ELogType types that this log handler accepts.
		virtual ELogType pass_types() = 0;
		virtual void set_pass_types(ELogType pass_types) = 0;

		//! If `true`, then this handler is enabled. The system will not dispatch 
		//! any log to a disabled handler.
		virtual bool enabled() = 0;
		virtual void set_enabled(bool enabled) = 0;

		//! If `true`, then this handler will accepts logs from all channels.
		virtual bool listening_all() = 0;
		virtual void set_listening_all(bool listen_all) = 0;

		//! Gets the registered log callback.
		virtual P<ILogCallback> callback() = 0;

		//! Gets the channels this handler is listening to.
		virtual R<Vector<P<IName>>> channels() = 0;
		//! Sets the channels.
		//! The passed variant will be interpreted as a 1-D name array.
		virtual void set_channels(const Vector<P<IName>>& channels) = 0;
		virtual void remove_all_channels() = 0;
	};
}