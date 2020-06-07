// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformTime.cpp
* @author JXMaster
* @date 2019/12/26
*/
#include "../../Platform.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
namespace luna
{
	namespace platform
	{
		LARGE_INTEGER g_ticks_per_second;
		LARGE_INTEGER g_start_ticks;
		uint64 g_unix_epoch;	// 100-nanoseconds.

		void platform_time_init()
		{
			::QueryPerformanceFrequency(&g_ticks_per_second);
			::QueryPerformanceCounter(&g_start_ticks);
			SYSTEMTIME t;
			t.wDay = 1;
			t.wDayOfWeek = 4;
			t.wHour = 0;
			t.wMilliseconds = 0;
			t.wMinute = 0;
			t.wMonth = 1;
			t.wSecond = 0;
			t.wYear = 1970;
			FILETIME ft;
			::SystemTimeToFileTime(&t, &ft);
			g_unix_epoch = (uint64)ft.dwLowDateTime | ((uint64)ft.dwHighDateTime << 32);
		}
		uint64 file_time_to_timestamp(const FILETIME* filetime)
		{
			uint64 ft = (uint64)filetime->dwLowDateTime | ((uint64)filetime->dwHighDateTime << 32);
			return (ft - g_unix_epoch) / 10000;
		}
		void timestamp_to_filetime(uint64 timestamp, FILETIME* filetime)
		{
			uint64 ft = (timestamp * 10000 + g_unix_epoch);
			filetime->dwLowDateTime = (DWORD)(ft & 0xFFFFFFFF);
			filetime->dwHighDateTime = (DWORD)((ft >> 32) & 0xFFFFFFFF);
		}
		uint64 get_ticks()
		{
			LARGE_INTEGER i;
			::QueryPerformanceCounter(&i);
			return i.QuadPart;
		}
		uint64 get_ticks_per_second()
		{
			return g_ticks_per_second.QuadPart;
		}
		uint64 get_elapsed_ticks()
		{
			return get_ticks() - g_start_ticks.QuadPart;
		}
		uint64 get_local_timestamp()
		{
			SYSTEMTIME t;
			::GetLocalTime(&t);
			FILETIME ft;
			::SystemTimeToFileTime(&t, &ft);
			uint64 nowT = (uint64)ft.dwLowDateTime | ((uint64)ft.dwHighDateTime << 32);
			uint64 ret = nowT - g_unix_epoch;
			ret /= 10000;	// 100-nanoseconds to 1 millisecond. Ref:https://docs.microsoft.com/en-us/windows/desktop/api/minwinbase/ns-minwinbase-filetime
			return ret;
		}
		uint64 get_utc_timestamp()
		{
			SYSTEMTIME t;
			::GetSystemTime(&t);
			FILETIME ft;
			::SystemTimeToFileTime(&t, &ft);
			uint64 nowT = (uint64)ft.dwLowDateTime | ((uint64)ft.dwHighDateTime << 32);
			uint64 ret = nowT - g_unix_epoch;
			ret /= 10000;	// 100-nanoseconds to 1 millisecond. Ref:https://docs.microsoft.com/en-us/windows/desktop/api/minwinbase/ns-minwinbase-filetime
			return ret;
		}
		void timestamp_to_data_time(uint64 timestamp, DateTime& datetime)
		{
			FILETIME ft;
			SYSTEMTIME st;
			timestamp_to_filetime(timestamp, &ft);
			::FileTimeToSystemTime(&ft, &st);
			datetime.year = st.wYear;
			datetime.month = st.wMonth;
			datetime.day = st.wDay;
			datetime.day_of_week = st.wDayOfWeek;
			datetime.hour = st.wHour;
			datetime.minute = st.wMinute;
			datetime.second = st.wSecond;
			datetime.milliseconds = st.wMilliseconds;
		}
		uint64 date_time_to_timestamp(const DateTime& datetime)
		{
			SYSTEMTIME st;
			FILETIME ft;
			st.wYear = datetime.year;
			st.wMonth = datetime.month;
			st.wDay = datetime.day;
			st.wDayOfWeek = datetime.day_of_week;
			st.wHour = datetime.hour;
			st.wMinute = datetime.minute;
			st.wSecond = datetime.second;
			st.wMilliseconds = datetime.milliseconds;
			::SystemTimeToFileTime(&st, &ft);
			return file_time_to_timestamp(&ft);
		}
	}
}

#endif