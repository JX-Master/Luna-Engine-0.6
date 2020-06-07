// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Random.hpp
* @author JXMaster
* @date 2019/3/24
*/
#pragma once
#include "../../Platform.hpp"
#ifdef LUNA_PLATFORM_WINDOWS
#include <Base/TSAssert.hpp>
#include <random>
namespace luna
{
	namespace win
	{
		class Random final : public IRandom
		{
		public:
			lucid("{4f09c790-fa3c-4613-b511-2d0175e15582}");
			luiimpl(Random, IRandom, IObject);
			lutsassert_lock();

			std::mt19937 m_engine;

			Random(IAllocator* alloc) :
				luibind(alloc) {}

			virtual void set_seed(uint32 seed) override
			{
				lutsassert();
				m_engine.seed(seed);
			}

			virtual uint32 gen_u32() override
			{
				lutsassert();
				return m_engine();
			}
			virtual int32 gen_i32() override
			{
				lutsassert();
				return (int32)m_engine();
			}
			virtual uint64 gen_u64() override
			{
				lutsassert();
				return ((uint64)m_engine()) + (((uint64)m_engine()) << 32);
			}
			virtual int64 gen_i64() override
			{
				lutsassert();
				return ((int64)m_engine()) + (((int64)m_engine()) << 32);
			}
			virtual float32 gen_f32(float32 range_begin, float32 range_end) override
			{
				return (((float32)gen_u32() / (float32)UINT32_MAX) - range_begin) * (range_end - range_begin);
			}
			virtual float64 gen_f64(float64 range_begin, float64 range_end) override
			{
				return (((float64)gen_u64() / (float64)UINT64_MAX) - range_begin) * (range_end - range_begin);
			}
			virtual Guid gen_guid() override
			{
				Guid guid;
				guid.low = gen_u64();
				guid.high = gen_u64();
				return guid;
			}
		};
	}
}

#endif