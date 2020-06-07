// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IModelRenderer.hpp
* @author JXMaster
* @date 2020/5/21
*/
#pragma once
#include <Scene/Scene.hpp>
#include "IModel.hpp"
namespace luna
{
	namespace e3d
	{
		struct IModelRenderer : public scene::IComponent
		{
			luiid("{378ef507-3204-4fd0-9706-82a3be288241}");

			//! Gets the mesh this renderer component references to.
			virtual asset::PAsset<IModel> model() = 0;

			//! Sets the mesh this renderer component references to.
			virtual void set_model(asset::PAsset<IModel> model) = 0;
		};
	}
}