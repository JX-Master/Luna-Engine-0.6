// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file UpdateLoop.hpp
* @author JXMaster
* @date 2019/12/9
*/
#pragma once
#include "../CoreHeader.hpp"

namespace luna
{
	namespace update_loop
	{
		void init();

		void new_frame();
		uint32 get_frame_index();
		uint64 get_last_frame_ticks();
		float64 get_last_frame_seconds();
		uint64 get_last_frame_raw_ticks();
		uint64 get_frame_ticks(uint32 frame_index);
		uint64 get_frame_ticks_raw(uint32 frame_index);
		float32 get_fps();
		float32 get_fps_raw();
		float32 get_fps_average();
		float32 get_fps_average_raw();
		uint64 get_frame_elapsed_ticks();
		float64 get_frame_elapsed_seconds();
		void set_frame_governing_enabled(bool enabled);
		bool is_frame_governing_enabled();
		RV set_frame_governing_fps(float32 frames_per_second);
		float32 get_frame_governing_fps();
		void set_min_frame_fps_clamping_enabled(bool enabled);
		bool is_min_frame_fps_clamping_enabled();
		void set_min_frame_fps(float32 frames_per_second);
		float32 get_min_frame_fps();
	}
}