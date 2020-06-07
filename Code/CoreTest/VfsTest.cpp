// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file VfsTest.cpp
* @author JXMaster
* @date 2020/2/20
*/
#include "TestCommon.hpp"

namespace luna
{
	void vfs_test()
	{
		// mount.

		mount_platfrom_path(new_path("/Platform/"), new_path("."));

		{
			// Test mount.
			auto fs = get_fs(new_path("/Platform/")).get();
			lutest(fs->native_path(new_path(".")).get()->equal_to(new_path(".")));
		}

		const char s[] = "Sample String";

		{
			// Try to open one file from vfs and writes to it.
			auto file = open_file(new_path("/Platform/SampleFile.txt"),
				EFileOpenFlag::write, EFileCreationMode::create_always).get();
			file->write(s, sizeof(s) - sizeof(char));
			file = nullptr;
		}

		{
			// try to open file from platform directly to make sure it is successfully written.
			auto file = platform_open_file("SampleFile.txt", 
				EFileOpenFlag::read, EFileCreationMode::open_existing).get();
			char str[32];
			file->read(str, 13 * sizeof(char));
			str[13] = 0;
			lutest(!strcmp(s, str));
			file = nullptr;

			// Clean up.
			delete_file(new_path("/Platform/SampleFile.txt"));
		}

		// unmount.
		unmount_fs(new_path("/Platform/"));
	}
}