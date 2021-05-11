// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IRectPackContext.hpp
* @author JXMaster
* @date 2019/10/9
*/
#pragma once
#include <Core/IObject.hpp>
#include <Runtime/Math.hpp>

namespace Luna
{
	namespace RectPack
	{
		struct PackRect
		{
			//! The width of the rect.
			u32 width;
			//! The height of the rect.
			u32 height;
			//! The outputted x position of the rect.
			u32 x;
			//! The outputted y position of the rect.
			u32 y;
			//! `true` if successfully packed this rect.
			bool packed;
		};
		struct IRectPackContext : public IObject
		{
			luiid("{a04110a8-9681-4379-bccb-27edc20fbe58}");

			//! Resets the packing context to prepare for a new series of packing calls.
			virtual void reset(u32 width, u32 height) = 0;

			//! Packs an array of rects into this big rect.
			//! @param[in] rects An array of rects to pack.
			//! The x, y position is relative to the top-left point of the big rect region, with x-points-right and
			//! y-points-down.
			//! @param[in] num_rects The number of rects to pack, which is the number
			//! of rects in the `rects` array.
			//! @return Returns success if all rects required are successfully packed into the rect, 
			//! returns failure otherwise.
			virtual RV pack_rects(PackRect* rects, u32 num_rects) = 0;
		};
	}
}