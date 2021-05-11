// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Time.hpp
* @author JXMaster
* @date 2020/8/16
* @brief Runtime System Time APIs.
*/
#pragma once
#include "Result.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	struct DateTime
	{
		u16 year;
		u8 month;
		u8 day;
		u8 hour;
		u8 minute;
		u8 second;
		u8 day_of_week;
	};

	//! Queries the ticks of the high-performance counter of CPU.
	//! @return Returns the current ticks of the CPU.
	LUNA_RUNTIME_API u64 get_ticks();

	//! Queries the resolution of high-performance counter of CPU represented by
	//! number of ticks per second.
	//! @return Returns the number of ticks per second.
	LUNA_RUNTIME_API f64 get_ticks_per_second();

	//! Gets current timestamp.
	//! The returned time is in UNIX time stamp format (number of seconds from Jan 1st, 1970, UTC).
	LUNA_RUNTIME_API i64 get_timestamp();

	//! Converts an UNIX timestamp to a date time represented in the current platform's time zone.
	LUNA_RUNTIME_API DateTime timestamp_to_local_datetime(i64 timestamp);

	//! Converts an UNIX timestamp to a data time in UTC time zone.
	LUNA_RUNTIME_API DateTime timestamp_to_utc_datetime(i64 timestamp);

	//! Converts a date time in the current time zone to an UNIX timestamp.
	LUNA_RUNTIME_API i64 local_datetime_to_timestamp(const DateTime& datetime);

	//! Converts a date time in UTC time zone to an UNIX timestamp.
	LUNA_RUNTIME_API i64 utc_datetime_to_timestamp(const DateTime& datetime);

	//! Get the number of ticks elapsed since the engine starts.
	//! @return The number of ticks elapsed since the system initialize.
	LUNA_RUNTIME_API u64 get_elapsed_ticks();

	//! Get the current frame index. Every time `ISystem::update` is called, the frame index will be increased by 1.
	//! The engine starts the first frame immediately after initialized, so the first frame is 1 before the first call to `ISystem::update`.
	LUNA_RUNTIME_API u32 get_frame_index();

	//! Get the delta time of last frame, that is, the elapsed time between the last 2 `ISystem::update` call.
	//! The time is clamped by min govern time and max govern time.
	LUNA_RUNTIME_API u64 get_last_frame_ticks();

	//! Get the actual delta time of last frame, that is, the frame time before modified by frame governing feature 
	//! and max frame time clamping feature.
	//! This frame time is for debug use, not for updating the system.
	LUNA_RUNTIME_API u64 get_last_frame_ticks_raw();

	//! Get the frame ticks of the specified frames. Luna Engine records the most recent 256 frame ticks for profiling use
	//! in every debug level build.
	//! @param[in] frame_index The index of frame to check. 0 means the latest frame (the last frame) and 255 means the 
	//! furthest frame.
	LUNA_RUNTIME_API u64 get_last_n_frame_ticks(u32 frame_index);

	//! Get the actual frame ticks before modified by frame governing feature and max frame time clamping feature.
	//! This frame time is for debug use, not for updating the system.
	//! @param[in] frame_index The index of frame to check. 0 means the latest frame (the last frame) and 255 means the 
	//! furthest frame.
	LUNA_RUNTIME_API u64 get_last_n_frame_ticks_raw(u32 frame_index);

	//! Get the frames per second value judged from last frame ( 1.0f / get_last_frame_seconds ).
	LUNA_RUNTIME_API f32 get_fps();

	//! Get the frames per second value judged from last frame's raw ticks ( 1.0f / (get_last_frame_raw_ticks() / get_ticks_per_second()))
	LUNA_RUNTIME_API f32 get_fps_raw();

	//! Get the frames per second value averaged from last 120 frames.
	LUNA_RUNTIME_API f32 get_fps_averaged();

	//! Get the frames per second value averaged from last 120 frame's raw data.
	LUNA_RUNTIME_API f32 get_fps_averaged_raw();

	//! Get the elapsed ticks from last call to `ISystem::update`.
	LUNA_RUNTIME_API u64 get_frame_elapsed_ticks();

	//! Set frame governing feature enabled/disabled. This feature is disabled by default.
	//! When the frame governing is enabled, the system will try to control the frame time between two
	//! `ISystem::update` call to a specified value set_char by `ITimeSystem::set_frame_governing_fps'.
	//! If the frame time is smaller than the govern period value, the system will suspend the thread or
	//! yield the thread several times until the govern frame time is reached. If the frame time is greater
	//! than the govern frame time, the system does nothing.
	LUNA_RUNTIME_API void set_frame_governing_enabled(bool enabled);

	//! Check if the frame governing feature is enabled.
	LUNA_RUNTIME_API bool is_frame_governing_enabled();

	//! Set the govern value used by frame governing features.
	//! The value is set_char to 60.0f by default.
	LUNA_RUNTIME_API RV set_frame_governing_fps(f32 frames_per_second);

	//! Get the govern value used by frame governing features.
	LUNA_RUNTIME_API f32 get_frame_governing_fps();

	//! Set min frame fps clamping enabled/disabled. This feature is disabled by default.
	//! Min frame fps clamping will clamp the frame time to a specified value if the frame time is larger than
	//! the specified value. This is necessary in order to keep some time-related system (like physical simulation)
	//! stable and prevent a large frame time peak ruins the system when the system runs too slow or paused by debug break.
	LUNA_RUNTIME_API void set_min_frame_fps_clamping_enabled(bool enabled);

	//! Check if the max frame time clamping feature is enabled.
	LUNA_RUNTIME_API bool is_min_frame_fps_clamping_enabled();

	//! Set the value used to clamping max frame time. Default value is 10.0f (at least 10 frames per second).
	LUNA_RUNTIME_API void set_min_frame_fps(f32 frames_per_second);

	//! Get the value used to clamping max frame time.
	LUNA_RUNTIME_API f32 get_min_frame_fps();
}
