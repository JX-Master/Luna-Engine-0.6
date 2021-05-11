// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IDecoder.hpp
* @author JXMaster
* @date 2020/1/26
*/
#pragma once
#include "Variant.hpp"
#include "IStream.hpp"
namespace Luna
{
	struct IDecoder : public IObject
	{
		luiid("{a4ae668e-212d-440b-b4da-8b9cd26680d0}");

		//! Decodes the variant object from the source stream.
		//! If the decoding process succeeds, the cursor of the stream points to
		//! the first byte after the decoded part. if the decoding process fails, 
		//! the cursor position is undefined and most likely points to the place 
		//! where an error occurs.
		//! 
		//! The stream must be readable and seekable in order to used by the decoder.
		virtual R<Variant> decode(IStream* source_stream) = 0;
	};
}