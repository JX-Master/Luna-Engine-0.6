// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file DataSystem.hpp
* @author JXMaster
* @date 2020/1/1
*/
#pragma once
#include "../CoreHeader.hpp"
namespace luna
{
	namespace data
	{
		P<IVariant> new_var(EVariantType t);
		P<IVariant> new_var1(EVariantType t, size_t d1);
		P<IVariant> new_var2(EVariantType t, size_t d1, size_t d2);
		P<IVariant> new_var3(EVariantType t, size_t d1, size_t d2, size_t d3);
		P<IVariant> new_var4(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4);
		P<IVariant> new_var5(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5);
		P<IVariant> new_var6(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6);
		P<IVariant> new_var7(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7);
		P<IVariant> new_var8(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7, size_t d8);

		P<IEncoder> new_text_encoder();
		P<IDecoder> new_text_decoder();
	}
}