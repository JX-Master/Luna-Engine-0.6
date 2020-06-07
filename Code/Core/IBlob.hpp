// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IBlob.hpp
* @author JXMaster
* @date 2018/10/27
 */
#pragma once
#include <Base/IObject.hpp>

namespace luna
{
	//! @interface IBlob
	//! @threadsafe
	//! BLOB represents Binary Large OBject. An object that implements `IBlob` interface
	//! can be used to store any kind of binary data, the data is reference-counted
	//! so it is suitable for passing some plain data to others.
	struct IBlob : public IObject
	{
		luiid("{ddd4d7b1-d70d-4261-9ebc-eed6b832a6d7}");

		//! Get a pointer to the underlying binary data.
		virtual const void* data() = 0;
		//! Get the size of the data in bytes.
		virtual size_t size() = 0;
	};
}