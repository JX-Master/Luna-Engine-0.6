// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ISceneComponentEditor.hpp
* @author JXMaster
* @date 2020/5/21
*/
#pragma once
#include "StudioHeader.hpp"

namespace luna
{
	namespace editor
	{
		struct ISceneComponentEditor : public IObject
		{
			luiid("{a75b9e3c-7518-4e92-8101-146e3478ea33}");

			virtual IName* type_name() = 0;

			virtual void on_render(imgui::IContext* ctx) = 0;
		};

	}
}