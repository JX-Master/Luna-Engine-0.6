// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformRandom.cpp
* @author JXMaster
* @date 2019/12/26
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT
#include "Random.hpp"
#include "../Core.hpp"
#include <Runtime/Time.hpp>
namespace Luna
{
	std::mt19937 g_random_engine;
	P<IMutex> g_random_mutex;

	void random_init()
	{
		g_random_engine.seed((unsigned int)get_ticks());
		g_random_mutex = new_mutex();
	}

	void random_deinit()
	{
		g_random_mutex = nullptr;
	}

	LUNA_CORE_API P<IRandom> new_random(u32 initial_seed)
	{
		auto ret = newobj<Random>();
		ret->set_seed(initial_seed);
		return box_ptr<IRandom>(ret);
	}

	LUNA_CORE_API u32 random_u32()
	{
		g_random_mutex->wait();
		u32 r = g_random_engine();
		g_random_mutex->unlock();
		return r;
	}
	LUNA_CORE_API i32 random_i32()
	{
		g_random_mutex->wait();
		i32 r = (i32)g_random_engine();
		g_random_mutex->unlock();
		return r;
	}
	LUNA_CORE_API u64 random_u64()
	{
		g_random_mutex->wait();
		u64 r = ((u64)g_random_engine()) + (((u64)g_random_engine()) << 32);
		g_random_mutex->unlock();
		return r;
	}
	LUNA_CORE_API i64 random_i64()
	{
		g_random_mutex->wait();
		i64 r = ((i64)g_random_engine()) + (((i64)g_random_engine()) << 32);
		g_random_mutex->unlock();
		return r;
	}
	LUNA_CORE_API f32 random_f32(f32 range_begin, f32 range_end)
	{
		g_random_mutex->wait();
		f32 r = (((f32)random_u32() / (f32)UINT32_MAX) - range_begin) * (range_end - range_begin);
		g_random_mutex->unlock();
		return r;
	}
	LUNA_CORE_API f64 random_f64(f64 range_begin, f64 range_end)
	{
		g_random_mutex->wait();
		f64 r = (((f64)random_u64() / (f64)UINT64_MAX) - range_begin) * (range_end - range_begin);
		g_random_mutex->unlock();
		return r;
	}
	LUNA_CORE_API Guid random_guid()
	{
		g_random_mutex->wait();
		Guid guid;
		guid.low = random_u64();
		guid.high = random_u64();
		g_random_mutex->unlock();
		return guid;
	}
}