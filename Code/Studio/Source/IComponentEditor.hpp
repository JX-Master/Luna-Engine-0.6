// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IComponentEditor.hpp
* @author JXMaster
* @date 2020/5/20
*/
#pragma once
#include "StudioHeader.hpp"

namespace Luna
{
	namespace editor
	{
		struct IComponentEditor : public IObject
		{
			luiid("{c61e20f5-346c-4ec0-995f-9c6bdc273418}");

			virtual Name type_name() = 0;

			virtual void on_render(ImGui::IContext* ctx) = 0;
		};
	}
}