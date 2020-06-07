// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file UpdateLoop.cpp
* @author JXMaster
* @date 2019/12/9
*/
#include "UpdateLoop.hpp"
namespace luna
{
	namespace update_loop
	{
		uint64 m_frame_ticks[256];	// the frame end ticks of last 256 frames.
		uint64 m_frame_ticks_raw[256];
		uint64 m_frame_governing_ticks;
		uint64 m_frame_clamping_ticks;
		uint64 m_frame_begin_ticks;		// The start time point of current frame.
		uint64 m_frame_ticks_sum120;	// The sum of frame ticks in prior 120 frames.
		uint64 m_frame_ticks_raw_sum120;
		uint32 m_last_frame_index;		// current frame index at frame ticks.
		uint32 m_frame_index;			// frame index start from 0.
		bool m_frame_governing_enabled;
		bool m_min_frame_fps_clamping_enabled;

		void init()
		{
			memzero(m_frame_ticks, sizeof(uint64) * 256);
			memzero(m_frame_ticks_raw, sizeof(uint64) * 256);
			m_frame_governing_ticks = (uint64)(1.0f / 60.0f * get_ticks_per_second());
			m_frame_governing_ticks = (uint64)(1.0f / 10.0f * get_ticks_per_second());
			m_frame_begin_ticks = get_elapsed_ticks();
			m_frame_ticks_sum120 = 0;
			m_frame_ticks_raw_sum120 = 0;
			m_last_frame_index = 0;
			m_frame_index = 0;
			m_frame_governing_enabled = false;
			m_min_frame_fps_clamping_enabled = false;
		}
		void new_frame()
		{
			m_frame_index++;

			uint64 time_now = get_ticks();
			uint64 time_delta = time_now - m_frame_begin_ticks;

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
				int64 waittimeus = (m_frame_begin_ticks + m_frame_governing_ticks - time_now) * 1000000 / get_ticks_per_second();
				if (waittimeus > 0)
				{
					fast_sleep((uint32)waittimeus);
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
		uint32 get_frame_index()
		{
			return m_frame_index;
		}
		uint64 get_last_frame_ticks()
		{
			return m_frame_ticks[m_last_frame_index];
		}
		float64 get_last_frame_seconds()
		{
			return (float64)(m_frame_ticks[m_last_frame_index]) / (float64)(get_ticks_per_second());
		}
		uint64 get_last_frame_raw_ticks()
		{
			return m_frame_ticks_raw[m_last_frame_index];
		}
		uint64 get_frame_ticks(uint32 frame_index)
		{
			uint32 index = (m_last_frame_index + 256 - frame_index) % 256;
			return m_frame_ticks[index];
		}
		uint64 get_frame_ticks_raw(uint32 frame_index)
		{
			uint32 index = (m_last_frame_index + 256 - frame_index) % 256;
			return m_frame_ticks_raw[index];
		}
		float32 get_fps()
		{
			return 1.0f / ((float32)(m_frame_ticks[m_last_frame_index]) / (float32)get_ticks_per_second());
		}
		float32 get_fps_raw()
		{
			return 1.0f / ((float32)(m_frame_ticks_raw[m_last_frame_index]) / (float32)get_ticks_per_second());
		}
		float32 get_fps_average()
		{
			return 1.0f / ((float32)m_frame_ticks_sum120 / 120.0f / (float32)get_ticks_per_second());
		}
		float32 get_fps_average_raw()
		{
			return 1.0f / ((float32)m_frame_ticks_raw_sum120 / 120.0f / (float32)get_ticks_per_second());
		}
		uint64 get_frame_elapsed_ticks()
		{
			return get_ticks() - m_frame_begin_ticks;
		}
		float64 get_frame_elapsed_seconds()
		{
			return (float64)(get_ticks() - m_frame_begin_ticks) / ((float64)get_ticks_per_second());
		}
		void set_frame_governing_enabled(bool enabled)
		{
			m_frame_governing_enabled = enabled;
		}
		bool is_frame_governing_enabled()
		{
			return m_frame_governing_enabled;
		}
		RV set_frame_governing_fps(float32 frames_per_second)
		{
			if (frames_per_second <= FLT_EPSILON)
			{
				return e_bad_arguments;
			}
			float64 frame_period = 1.0 / (float64)frames_per_second;
			m_frame_governing_ticks = (uint64)(frame_period * get_ticks_per_second());
			return s_ok;
		}
		float32 get_frame_governing_fps()
		{
			return 1.0f / (float32)((float64)m_frame_governing_ticks / (float64)get_ticks_per_second());
		}
		void set_min_frame_fps_clamping_enabled(bool enabled)
		{
			m_min_frame_fps_clamping_enabled = enabled;
		}
		bool is_min_frame_fps_clamping_enabled()
		{
			return m_min_frame_fps_clamping_enabled;
		}
		void set_min_frame_fps(float32 frames_per_second)
		{
			if (frames_per_second <= FLT_EPSILON)
			{
				m_frame_clamping_ticks = UINT64_MAX;
				return;
			}
			float64 frame_period = 1.0 / (float64)frames_per_second;
			m_frame_clamping_ticks = (uint64)(frame_period * get_ticks_per_second());
		}
		float32 get_min_frame_fps()
		{
			return 1.0f / (float32)((float64)m_frame_clamping_ticks / (float64)get_ticks_per_second());
		}
	}
}