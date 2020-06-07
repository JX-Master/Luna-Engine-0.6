// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file 3DEngine.hpp
* @author JXMaster
* @date 2020/5/10
*/
#pragma once
#include "IMesh.hpp"
#include "ICamera.hpp"
#include "ITransform.hpp"
#include "ILight.hpp"
#include "ISceneRenderer.hpp"
#include "IMaterial.hpp"
#include "IModel.hpp"
#include "IModelRenderer.hpp"

#ifndef LUNA_3DENGINE_API
#define LUNA_3DENGINE_API 
#endif

namespace luna
{
	namespace e3d
	{
		LUNA_3DENGINE_API RV init();
		LUNA_3DENGINE_API RP<IMesh> new_mesh();
		LUNA_3DENGINE_API RP<IMaterial> new_material();
		LUNA_3DENGINE_API RP<IModel> new_model();
	}
}