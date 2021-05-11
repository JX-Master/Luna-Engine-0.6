// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Vfs.hpp
* @author JXMaster
* @date 2020/1/23
*/
#pragma once
#include <Runtime/Vector.hpp>
#include "PlatformFileSystem.hpp"
#include "VirtualFileSystem.hpp"
#include "../IMutex.hpp"
namespace Luna
{
	// Mount path.
	struct MountPair
	{
		Path m_path;
		P<IFileSystem> m_fs;
	};

	extern P<IMutex> m_lock;
	extern Unconstructed<Vector<MountPair>> m_mounts;

	void vfs_init();
	void vfs_deinit();
}