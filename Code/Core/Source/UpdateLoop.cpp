// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file UpdateLoop.cpp
* @author JXMaster
* @date 2019/12/9
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_DLL_EXPORT
#include "../Core.hpp"
#include <Runtime/Platform.hpp>
#include <Runtime/Thread.hpp>
namespace Luna
{
	u64 m_frame_ticks[256];	// the frame end ticks of last 256 frames.
	u64 m_frame_ticks_raw[256];
	u64 m_frame_governing_ticks;
	u64 m_frame_clamping_ticks;
	u64 m_frame_begin_ticks;		// The start time point of current frame.
	u64 m_frame_ticks_sum120;	// The sum of frame ticks in prior 120 frames.
	u64 m_frame_ticks_raw_sum120;
	u32 m_last_frame_index;		// current frame index at frame ticks.
	u32 m_frame_index;			// frame index start from 0.
	bool m_frame_governing_enabled;
	bool m_min_frame_fps_clamping_enabled;

	void update_loop_init()
	{
		memzero(m_frame_ticks, sizeof(u64) * 256);
		memzero(m_frame_ticks_raw, sizeof(u64) * 256);
		m_frame_governing_ticks = (u64)(1.0f / 60.0f * Platform::get_ticks_per_second());
		m_frame_governing_ticks = (u64)(1.0f / 10.0f * Platform::get_ticks_per_second());
		m_frame_begin_ticks = get_ticks();
		m_frame_ticks_sum120 = 0;
		m_frame_ticks_raw_sum120 = 0;
		m_last_frame_index = 0;
		m_frame_index = 0;
		m_frame_governing_enabled = false;
		m_min_frame_fps_clamping_enabled = false;
	}
	LUNA_CORE_API void new_frame()
	{
		m_frame_index++;

		u64 time_now = get_ticks();
		u64 time_delta = time_now - m_frame_begin_ticks;

		// update raw data.
		m_last_frame_index++;
		m_last_frame_index = m_last_frame_index % 256;
		m_frame_ticks_raw[m_last_frame_index] = time_delta;
		m_frame_ticks_raw_sum120 += time_delta;
		m_frame_ticks_raw_sum120 -= m_frame_ticks_raw[(m_last_frame_index + 256 - 120) % 256];

		// clamp the min ticks if needed.
		if (m_min_frame_fps_clamping_enabled)
		{
			time_delta = time_delta < m_frame_clamping_ticks ? time_delta : m_frame_clamping_ticks;
		}

		// wait if needed.
		if (m_frame_governing_enabled && (time_delta < m_frame_governing_ticks))
		{
			time_now = get_ticks();
			i64 waittimeus = i64((m_frame_begin_ticks + m_frame_governing_ticks - time_now) * 1000000 / get_ticks_per_second());
			if (waittimeus > 0)
			{
				fast_sleep((u32)waittimeus);
			}
			time_now = get_ticks();
			time_delta = time_now - m_frame_begin_ticks;
		}

		// update modified data.
		m_frame_ticks[m_last_frame_index] = time_delta;
		m_frame_ticks_sum120 += time_delta;
		m_frame_ticks_sum120 -= m_frame_ticks_raw[(m_last_frame_index + 256 - 120) % 256];
		m_frame_begin_ticks = time_now;
	}
	LUNA_CORE_API u32 get_frame_index()
	{
		return m_frame_index;
	}
	LUNA_CORE_API u64 get_last_frame_ticks()
	{
		return m_frame_ticks[m_last_frame_index];
	}
	LUNA_CORE_API f64 get_last_frame_seconds()
	{
		return (f64)(m_frame_ticks[m_last_frame_index]) / (f64)(get_ticks_per_second());
	}
	LUNA_CORE_API u64 get_last_frame_ticks_raw()
	{
		return m_frame_ticks_raw[m_last_frame_index];
	}
	LUNA_CORE_API u64 get_last_n_frame_ticks(u32 frame_index)
	{
		u32 index = (m_last_frame_index + 256 - frame_index) % 256;
		return m_frame_ticks[index];
	}
	LUNA_CORE_API u64 get_last_n_frame_ticks_raw(u32 frame_index)
	{
		u32 index = (m_last_frame_index + 256 - frame_index) % 256;
		return m_frame_ticks_raw[index];
	}
	LUNA_CORE_API f32 get_fps()
	{
		return 1.0f / ((f32)(m_frame_ticks[m_last_frame_index]) / (f32)get_ticks_per_second());
	}
	LUNA_CORE_API f32 get_fps_raw()
	{
		return 1.0f / ((f32)(m_frame_ticks_raw[m_last_frame_index]) / (f32)get_ticks_per_second());
	}
	LUNA_CORE_API f32 get_fps_averaged()
	{
		return 1.0f / ((f32)m_frame_ticks_sum120 / 120.0f / (f32)get_ticks_per_second());
	}
	LUNA_CORE_API f32 get_fps_averaged_raw()
	{
		return 1.0f / ((f32)m_frame_ticks_raw_sum120 / 120.0f / (f32)get_ticks_per_second());
	}
	LUNA_CORE_API u64 get_frame_elapsed_ticks()
	{
		return get_ticks() - m_frame_begin_ticks;
	}
	LUNA_CORE_API f64 get_frame_elapsed_seconds()
	{
		return (f64)(get_ticks() - m_frame_begin_ticks) / ((f64)get_ticks_per_second());
	}
	LUNA_CORE_API void set_frame_governing_enabled(bool enabled)
	{
		m_frame_governing_enabled = enabled;
	}
	LUNA_CORE_API bool is_frame_governing_enabled()
	{
		return m_frame_governing_enabled;
	}
	LUNA_CORE_API RV set_frame_governing_fps(f32 frames_per_second)
	{
		if (frames_per_second <= FLT_EPSILON)
		{
			return BasicError::bad_arguments();
		}
		f64 frame_period = 1.0 / (f64)frames_per_second;
		m_frame_governing_ticks = (u64)(frame_period * get_ticks_per_second());
		return RV();
	}
	LUNA_CORE_API f32 get_frame_governing_fps()
	{
		return 1.0f / (f32)((f64)m_frame_governing_ticks / (f64)get_ticks_per_second());
	}
	LUNA_CORE_API void set_min_frame_fps_clamping_enabled(bool enabled)
	{
		m_min_frame_fps_clamping_enabled = enabled;
	}
	LUNA_CORE_API bool is_min_frame_fps_clamping_enabled()
	{
		return m_min_frame_fps_clamping_enabled;
	}
	LUNA_CORE_API void set_min_frame_fps(f32 frames_per_second)
	{
		if (frames_per_second <= FLT_EPSILON)
		{
			m_frame_clamping_ticks = UINT64_MAX;
			return;
		}
		f64 frame_period = 1.0 / (f64)frames_per_second;
		m_frame_clamping_ticks = (u64)(frame_period * get_ticks_per_second());
	}
	LUNA_CORE_API f32 get_min_frame_fps()
	{
		return 1.0f / (f32)((f64)m_frame_clamping_ticks / (f64)get_ticks_per_second());
	}
}