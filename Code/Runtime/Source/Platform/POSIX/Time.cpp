// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Time.hpp
* @author JXMaster
* @date 2020/9/23
*/
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#include "../../OS.hpp"
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#ifdef LUNA_PLATFORM_MACOS
#include <mach/mach_time.h>
#endif

namespace Luna
{
    namespace OS
    {
#ifdef LUNA_PLATFORM_MACOS
		f64 g_ticks_per_second;
#endif
		int g_minutes_west;

		void time_init()
		{
#ifdef LUNA_PLATFORM_MACOS
			mach_timebase_info_data_t tb_info;
			luna_eval_and_assert_equal(mach_timebase_info(&tb_info), KERN_SUCCESS);
			g_ticks_per_second = (1000000000.0 * (f64)tb_info.denom) / (f64)tb_info.numer;
#endif
			g_start_ticks = get_ticks();
			timeval tv;
			struct timezone tz;
			gettimeofday(&tv, &tz);
			g_minutes_west = tz.tz_minuteswest;
		}

		u64 get_ticks()
		{
#ifdef LUNA_PLATFORM_MACOS
			return mach_absolute_time();
#else
			timespec        spec;
			clock_gettime(CLOCK_MONOTONIC, &spec);
			return (RawTimeValue(spec.tv_sec) * 1000000000U + spec.tv_nsec);
#endif
		}
		f64 get_ticks_per_second()
		{
#ifdef LUNA_PLATFORM_MACOS
			return g_ticks_per_second;
#else
			return 1000000000.0;
#endif
		}
		i64 get_timestamp()
		{
			time_t t = time(nullptr);
			return (i64)t;
		}
		DateTime timestamp_to_local_datetime(i64 timestamp)
		{
			tm dt;
			time_t t = (time_t)timestamp;
			localtime_r(&t, &dt);
			DateTime dt_r;
			dt_r.year = dt.tm_year + 1900;
			dt_r.month = dt.tm_mon + 1;
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
			time_t t = (time_t)timestamp;
			gmtime_r(&t, &dt);
			DateTime dt_r;
			dt_r.year = dt.tm_year + 1900;
			dt_r.month = dt.tm_mon + 1;
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
			dt.tm_year = datetime.year - 1900;
			dt.tm_mon = datetime.month - 1;
			dt.tm_mday = datetime.day;
			dt.tm_hour = datetime.hour;
			dt.tm_min = datetime.minute;
			dt.tm_sec = datetime.second;
			dt.tm_isdst = 0;
			return (i64)mktime(&dt);
		}
		i64 utc_datetime_to_timestamp(const DateTime& datetime)
		{
			return OS::local_datetime_to_timestamp(datetime) - g_minutes_west * 60;
		}
    }
}

#endif
