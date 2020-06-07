// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformRandom.cpp
* @author JXMaster
* @date 2019/12/26
*/
#include "../../Platform.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
#include "Random.hpp"
namespace luna
{
	namespace platform
	{
		using namespace win;

		std::mt19937 g_random_engine;
		CRITICAL_SECTION g_random_mutex;

		void platform_random_init()
		{
			g_random_engine.seed((unsigned int)get_ticks());
			InitializeCriticalSection(&g_random_mutex);
		}

		P<IRandom> new_random(uint32 initial_seed)
		{
			auto ret = get_global_heap()->new_obj<Random>(get_global_heap());
			ret->set_seed(initial_seed);
			return box_ptr<IRandom>(ret);
		}

		uint32 random_u32()
		{
			EnterCriticalSection(&g_random_mutex);
			uint32 r = g_random_engine();
			LeaveCriticalSection(&g_random_mutex);
			return r;
		}
		int32 random_i32()
		{
			EnterCriticalSection(&g_random_mutex);
			int32 r = (int32)g_random_engine();
			LeaveCriticalSection(&g_random_mutex);
			return r;
		}
		uint64 random_u64()
		{
			EnterCriticalSection(&g_random_mutex);
			uint64 r = ((uint64)g_random_engine()) + (((uint64)g_random_engine()) << 32);
			LeaveCriticalSection(&g_random_mutex);
			return r;
		}
		int64 random_i64()
		{
			EnterCriticalSection(&g_random_mutex);
			int64 r = ((int64)g_random_engine()) + (((int64)g_random_engine()) << 32);
			LeaveCriticalSection(&g_random_mutex);
			return r;
		}
		float32 random_f32(float32 range_begin, float32 range_end)
		{
			EnterCriticalSection(&g_random_mutex);
			float32 r = (((float32)random_u32() / (float32)UINT32_MAX) - range_begin) * (range_end - range_begin);
			LeaveCriticalSection(&g_random_mutex);
			return r;
		}
		float64 random_f64(float64 range_begin, float64 range_end)
		{
			EnterCriticalSection(&g_random_mutex);
			float64 r = (((float64)random_u64() / (float64)UINT64_MAX) - range_begin) * (range_end - range_begin);
			LeaveCriticalSection(&g_random_mutex);
			return r;
		}
		Guid random_guid()
		{
			EnterCriticalSection(&g_random_mutex);
			Guid guid;
			guid.low = random_u64();
			guid.high = random_u64();
			LeaveCriticalSection(&g_random_mutex);
			return guid;
		}
	}
}
#endif