// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IEncoder.hpp
* @author JXMaster
* @date 2020/1/28
*/
#pragma once
#include "Variant.hpp"
#include "IStream.hpp"
#include <Runtime/Result.hpp>
namespace Luna
{
	struct IEncoder : public IObject
	{
		luiid("{8770709b-51f1-4be8-be4c-3cfef819f9bb}");

		virtual RV encode(const Variant& variant, IStream* stream) = 0;
	};
}