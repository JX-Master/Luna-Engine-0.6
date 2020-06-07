// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Name.hpp
* @author JXMaster
* @date 2019/8/6
 */
#pragma once
#include "../CoreHeader.hpp"
#include <Base/Hash.hpp>
namespace luna
{
	class Name : public IName
	{
	public:
		lucid("{8fb92314-d65c-4ffb-b760-db28d32fdb36}");
		luiimpl(Name, IName, IString, IObject);

		const char* m_str;
		size_t m_len;
		uint64 m_h;

		Name(IAllocator* alloc);
		~Name();
		virtual const char* c_str() override
		{
			return m_str;
		}
		virtual size_t size() override
		{
			return m_len;
		}
	};
}