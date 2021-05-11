// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file VfsTest.cpp
* @author JXMaster
* @date 2020/2/20
*/
#include "TestCommon.hpp"

namespace Luna
{
	void vfs_test()
	{
		// mount.

		mount_platfrom_path(u8"/Platform/", u8".");

		{
			// Test mount.
			auto fs = get_fs(u8"/Platform/").get();
			lutest(fs->native_path(u8".").get().equal_to(u8"."));
		}

		const char s[] = u8"Sample String";

		{
			// Try to open one file from vfs and writes to it.
			auto file = open_file(u8"/Platform/SampleFile.txt",
				EFileOpenFlag::write, EFileCreationMode::create_always).get();
			file->write(s, sizeof(s) - sizeof(char));
			file = nullptr;
		}

		{
			// try to open file from platform directly to make sure it is successfully written.
			auto file = platform_open_file(u8"SampleFile.txt",
				EFileOpenFlag::read, EFileCreationMode::open_existing).get();
			char str[32];
			file->read(str, 13 * sizeof(char));
			str[13] = 0;
			lutest(!strcmp(s, str));
			file = nullptr;

			// Clean up.
			delete_file(u8"/Platform/SampleFile.txt");
		}

		// unmount.
		unmount_fs(u8"/Platform/");
	}
}