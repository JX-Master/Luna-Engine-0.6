// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IModelRenderer.hpp
* @author JXMaster
* @date 2020/5/21
*/
#pragma once
#include <Scene/Scene.hpp>
#include "IModel.hpp"
namespace Luna
{
	namespace E3D
	{
		struct IModelRenderer : public Scene::IComponent
		{
			luiid("{378ef507-3204-4fd0-9706-82a3be288241}");

			//! Gets the mesh this renderer component references to.
			virtual Asset::PAsset<IModel> model() = 0;

			//! Sets the mesh this renderer component references to.
			virtual void set_model(Asset::PAsset<IModel> model) = 0;
		};
	}
}