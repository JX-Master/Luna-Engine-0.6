// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Random.hpp
* @author JXMaster
* @date 2019/3/24
*/
#pragma once
#include "../IRandom.hpp"
#include <Runtime/TSAssert.hpp>
#include <Core/Interface.hpp>
#include <random>
namespace Luna
{
	class Random final : public IRandom
	{
	public:
		lucid("{4f09c790-fa3c-4613-b511-2d0175e15582}");
		luiimpl(Random, IRandom, IObject);
		lutsassert_lock();

		std::mt19937 m_engine;

		Random() {}

		virtual void set_seed(u32 seed) override
		{
			lutsassert();
			m_engine.seed(seed);
		}

		virtual u32 gen_u32() override
		{
			lutsassert();
			return m_engine();
		}
		virtual i32 gen_i32() override
		{
			lutsassert();
			return (i32)m_engine();
		}
		virtual u64 gen_u64() override
		{
			lutsassert();
			return ((u64)m_engine()) + (((u64)m_engine()) << 32);
		}
		virtual i64 gen_i64() override
		{
			lutsassert();
			return ((i64)m_engine()) + (((i64)m_engine()) << 32);
		}
		virtual f32 gen_f32(f32 range_begin, f32 range_end) override
		{
			return (((f32)gen_u32() / (f32)UINT32_MAX) - range_begin) * (range_end - range_begin);
		}
		virtual f64 gen_f64(f64 range_begin, f64 range_end) override
		{
			return (((f64)gen_u64() / (f64)UINT64_MAX) - range_begin) * (range_end - range_begin);
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