// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file TimeTest.cpp
* @author JXMaster
* @date 2020/9/25
*/
#include "TestCommon.hpp"
#include <Runtime/Time.hpp>
#include <Runtime/Debug.hpp>

namespace Luna
{
	void time_test()
	{
		// Get UNIX timestamp.
		i64 timestamp = get_timestamp();

		// Get local time.
		DateTime local_time = timestamp_to_local_datetime(timestamp);
		
		// Get utc time.
		DateTime utc_time = timestamp_to_utc_datetime(timestamp);

		// Print local and utc time.
		debug_printf("Local Time: %hu-%hu-%hu %hu:%hu:%hu\n", 
			local_time.year, local_time.month, local_time.day, local_time.hour, local_time.minute, local_time.second);
		debug_printf("UTC Time: %hu-%hu-%hu %hu:%hu:%hu\n",
			utc_time.year, utc_time.month, utc_time.day, utc_time.hour, utc_time.minute, utc_time.second);

		i64 local_timestamp = local_datetime_to_timestamp(local_time);
		i64 utc_timestamp = utc_datetime_to_timestamp(utc_time);

		lutest(local_timestamp == timestamp);
		lutest(utc_timestamp == timestamp);
	}
}