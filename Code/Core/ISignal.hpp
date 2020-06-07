// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ISignal.hpp
* @author JXMaster
* @date 2019/3/14
* @brief ISignal interface represents a signal object.
*/
#pragma once
#include "IWaitable.hpp"

namespace luna
{
	//! @interface ISignal
	//! @threadsafe
	struct ISignal : public IWaitable
	{
		luiid("{79648c17-4685-41e0-a625-6228b0a06509}");
		virtual void trigger() = 0;
		virtual void reset() = 0;
	};
}