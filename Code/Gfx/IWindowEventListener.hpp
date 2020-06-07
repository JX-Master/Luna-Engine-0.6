// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IWindowEventListener.hpp
* @author JXMaster
* @date 2019/7/10
*/
#pragma once
#include "WindowEvent.hpp"
namespace luna
{
	namespace gfx
	{
		struct IWindow;
		//! @interface IWindowEventListener
		struct IWindowEventListener : public IObject
		{
			luiid("{e81723d8-2ed6-45f6-9195-885edf50b078}");

			//! Called when a window event is triggered. All window events are fetched and dispatched by window input device  
			//! when input system gets updated.
			//! @param[in] window The window that receives the event, or `nullptr` if the event is an application scope event.
			//! @param[in] e The event type.
			//! @param[in] data One pointer to the data structure, cast to specific type based on the event type.
			//! The data structure only exists before this function returns.
			//! @return Returns `true` if the event is handled by this listener and the listener does not want to use the 
			//! default event handling procedure, returns `false` otherwise.
			virtual bool on_window_event(IWindow* window, EWindowEvent e, void* data) = 0;
		};
	}
}