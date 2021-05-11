// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Time.cpp
* @author JXMaster
* @date 2020/8/16
*/
#pragma once
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_WINDOWS
#include "../../OS.hpp"
#include "MiniWin.hpp"

#include <time.h>

namespace Luna
{
	namespace OS
	{
		LARGE_INTEGER g_ticks_per_second;
		long g_time_bias_seconds;
		void time_init()
		{
			::QueryPerformanceFrequency(&g_ticks_per_second);
			_get_timezone(&g_time_bias_seconds);
		}
		u64 get_ticks()
		{
			LARGE_INTEGER i;
			::QueryPerformanceCounter(&i);
			return i.QuadPart;
		}
		f64 get_ticks_per_second()
		{
			return (f64)g_ticks_per_second.QuadPart;
		}
		i64 get_timestamp()
		{
			i64 t = _time64(NULL);
			return t;
		}
		DateTime timestamp_to_local_datetime(i64 timestamp)
		{
			tm dt;
			__time64_t  t = (__time64_t)timestamp;
			_localtime64_s(&dt, &t);
			DateTime dt_r;
			dt_r.year = dt.tm_year;
			dt_r.month = dt.tm_mon;
			dt_r.day = dt.tm_mday;
			dt_r.hour = dt.tm_hour;
			dt_r.minute = dt.tm_min;
			dt_r.second = dt.tm_sec;
			dt_r.day_of_week = dt.tm_wday;
			return dt_r;
		}
		DateTime timestamp_to_utc_datetime(i64 timestamp)
		{
			tm dt;
			__time64_t t = (__time64_t)timestamp;
			_gmtime64_s(&dt, &t);
			DateTime dt_r;
			dt_r.year = dt.tm_year;
			dt_r.month = dt.tm_mon;
			dt_r.day = dt.tm_mday;
			dt_r.hour = dt.tm_hour;
			dt_r.minute = dt.tm_min;
			dt_r.second = dt.tm_sec;
			dt_r.day_of_week = dt.tm_wday;
			return dt_r;
		}
		i64 local_datetime_to_timestamp(const DateTime& datetime)
		{
			tm dt;
			dt.tm_year = datetime.year;
			dt.tm_mon = datetime.month;
			dt.tm_mday = datetime.day;
			dt.tm_hour = datetime.hour;
			dt.tm_min = datetime.minute;
			dt.tm_sec = datetime.second;
			return (i64)_mktime64(&dt);
		}
		i64 utc_datetime_to_timestamp(const DateTime& datetime)
		{
			return OS::local_datetime_to_timestamp(datetime) + g_time_bias_seconds;
		}
	}
}

#endif