// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IAssetEditor.hpp
* @author JXMaster
* @date 2020/5/8
*/
#pragma once
#include "StudioHeader.hpp"

namespace Luna
{
	namespace editor
	{
		//! @interface IAssetEditor
		//! Represents a window of the editor.
		struct IAssetEditor : public IObject
		{
			luiid("{410f7868-38b5-4e3f-b291-8e58d2cb7372}");

			virtual void on_render(ImGui::IContext* ctx) = 0;
			virtual bool closed() = 0;
		};
	}
}