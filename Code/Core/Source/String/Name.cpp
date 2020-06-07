// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Name.cpp
* @author JXMaster
* @date 2019/8/6
*/
#include "Name.hpp"
#include "StringLib.hpp"
namespace luna
{
	Name::Name(IAllocator* alloc) :
		luibind(alloc) {}

	Name::~Name()
	{
		auto& slib = strlib::m_names.get();
		strlib::m_lock->write_lock();
		// move this to cached map.
		auto iter = strlib::m_names.get().find(m_h);
		if (iter != strlib::m_names.get().end())
		{
			strlib::m_names.get().erase(iter);
		}
		strlib::m_lock->write_unlock();
	}
}