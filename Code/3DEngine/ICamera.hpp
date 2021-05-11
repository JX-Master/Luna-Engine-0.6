// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ICamera.hpp
* @author JXMaster
* @date 2020/5/14
*/
#pragma once
#include <Scene/Scene.hpp>
namespace Luna
{
	namespace E3D
	{
		enum class ECameraType : u32
		{
			perspective = 0,
			orthographic = 1,
		};

		struct ICamera : public Scene::IComponent
		{
		public:
			luiid("{52a7e67a-5d11-4ebd-be33-0f5ab0170ced}");

			//! Default: perspective.
			virtual ECameraType camera_type() = 0;

			virtual void set_camera_type(ECameraType type) = 0;

			//! Only for perspective mode. Default: PI / 3.
			virtual f32 field_of_view() = 0;
			//! Only for perspective mode.
			virtual void set_field_of_view(f32 value) = 0;
			//! Only for orthographic mode. Default: 5.
			virtual f32 size() = 0;
			//! Only for orthographic mode.
			virtual void set_size(f32 value) = 0;

			//! Default: 0.3.
			virtual f32 near_clipping_plane() = 0;
			//! Default: 1000.
			virtual f32 far_clipping_plane() = 0;
			virtual void set_near_clipping_plane(f32 value) = 0;
			virtual void set_far_clipping_plance(f32 value) = 0;

			//! Default: 1.777777 (16/9).
			virtual f32 aspect_ratio() = 0;
			virtual void set_aspect_ratio(f32 aspect_ratio) = 0;

			//! Computes the projection matrix based on the current camera parameter.
			virtual Float4x4 projection_matrix() = 0;
		};
	}
}