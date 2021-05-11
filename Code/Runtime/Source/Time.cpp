// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Time.cpp
* @author JXMaster
* @date 2020/12/10
*/
#include "../PlatformDefines.hpp"
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "OS.hpp"
namespace Luna
{
	u64 g_start_ticks;

	void time_init()
	{
		g_start_ticks = get_ticks();
	}

	LUNA_RUNTIME_API u64 get_ticks()
	{
		return OS::get_ticks();
	}
	LUNA_RUNTIME_API f64 get_ticks_per_second()
	{
		return OS::get_ticks_per_second();
	}
	LUNA_RUNTIME_API i64 get_timestamp()
	{
		return OS::get_timestamp();
	}
	LUNA_RUNTIME_API DateTime timestamp_to_local_datetime(i64 timestamp)
	{
		return OS::timestamp_to_local_datetime(timestamp);
	}
	LUNA_RUNTIME_API DateTime timestamp_to_utc_datetime(i64 timestamp)
	{
		return OS::timestamp_to_utc_datetime(timestamp);
	}
	LUNA_RUNTIME_API i64 local_datetime_to_timestamp(const DateTime& datetime)
	{
		return OS::local_datetime_to_timestamp(datetime);
	}
	LUNA_RUNTIME_API i64 utc_datetime_to_timestamp(const DateTime& datetime)
	{
		return OS::utc_datetime_to_timestamp(datetime);
	}
	LUNA_RUNTIME_API u64 get_elapsed_ticks()
	{
		return get_ticks() - g_start_ticks;
	}
}
