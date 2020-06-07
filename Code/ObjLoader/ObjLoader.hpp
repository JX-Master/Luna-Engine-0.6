// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ObjLoader.hpp
* @author JXMaster
* @date 2020/5/12
*/
#pragma once
#include "IObjFile.hpp"

#ifndef LUNA_OBJ_LOADER_API
#define LUNA_OBJ_LOADER_API 
#endif

namespace luna
{
	namespace obj_loader
	{
		LUNA_OBJ_LOADER_API void init();

		//! Loads object file from file.
		//! @param[in] file_name The platform path of the file.
		LUNA_OBJ_LOADER_API RP<IObjFile> load(const char* platform_file_name);
	}
}