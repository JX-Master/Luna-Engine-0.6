// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ISwapChain.hpp
* @author JXMaster
* @date 2019/9/20
*/
#pragma once
#include "GraphicDefines.hpp"
#include "IGraphicDeviceChild.hpp"

namespace luna
{
	namespace gfx
	{
		struct IResource;
		struct IWindow;

		struct SwapChainDesc
		{
			//! The width of the swap chain back buffer. 
			//! Specify 0 will determine the size from the bounding window's native size.
			uint32 width;
			//! The width of the swap chain back buffer. 
			//! Specify 0 will determine the size from the bounding window's native size.
			uint32 height;
			EResourceFormat format;
			uint32 buffer_count;
			bool windowed;

			SwapChainDesc() = default;
			SwapChainDesc(
				uint32 _width,
				uint32 _height,
				EResourceFormat _format,
				uint32 _buffer_count,
				bool _windowed
			) :
				width(_width),
				height(_height),
				format(_format),
				buffer_count(_buffer_count),
				windowed(_windowed) {}
		};

		//! @interface ISwapChain
		//! The swap chain is used to present the rendering result to the output window on the platform's screen.
		//! The swap chain holds a pool of special texture resources, which are called back buffers. The number of back buffers
		//! the swap chain holds is determined when the swap chain is created by specifying `buffer_count` in `SwapChainDesc`, and all
		//! back buffers share the same width, height and format specified in `SwapChainDesc`. At any given time, only one back buffer is connected
		//! to the frame buffer of the monitor, which holds the data that is displayed to user on screen, and only one another back buffer is connected
		//! to the graphic device, which supports writing to, so it can be displayed on the monitor in next `present` call.
		//! 
		//! The scheduling order for back buffers is determined by the windowing system of the platform, and may vary in different platforms and in 
		//! different configurations when you create the swap chain. The only thing the Gfx guarantees is that at any given time, there is exactly 
		//! on back buffer that supports writing to by the command queue of the graphic device, and the buffer will exist until the next `present` call.
		//! 
		//! You cannot access the back buffer resource directly, instead, when you present the back buffer, you need to specify one 2D texture resource 
		//! where the back buffer gets the data from. The data will be copied to the back buffer and be displayed to the user in this call.
		struct ISwapChain : public IGraphicDeviceChild, public IWaitable
		{
			luiid("{cc455fba-646d-4a64-83e4-149f004a5ea0}");
			luiprimary(IGraphicDeviceChild);

			//! Gets the window that this swap chain bounds to.
			virtual IWindow* bounding_window() = 0;

			//! Gets the desc object.
			virtual SwapChainDesc desc() = 0;

			//! Schedule a present command in the command queue bound with this swap chain. The present command swaps the back buffer and presents
			//! the rendered image to the screen.
			//! @param[in] resource The resource which holds the data to be displayed. The data should stay valid until the present call is finished.
			//! @param[in] subresource The index of the subresource of the resource to present. The subresource must be a 2-D texture, if the texture
			//! size does not match the back buffer size, the texture will be stretched.
			//! @param[in] sync_interval Specify the sync interval of the presentation.
			//! If the interval is 0, the GPU command queue will execute present command immediately without checking if the previous back buffer
			//! is reading by the monitor control clip. The previous buffer will be discarded immediately and the monitor starts reading from the new
			//! buffer immediately. This may cause image tearing.
			//! If the interval is 1-4, the GPU command queue will waits n-vertical blacks before submitting the present operation. This may blocks the
			//! GPU command queue if the presenting speed is higher than the monitor refresh rate.
			//!
			//! @remark This call is unsynchronized, the call submits the present request to the command queue and returns immediately. Use `ISwapChain::wait`
			//! or `ISwapChain::try_wait` to wait for the present call to be processed. If another present call is submitted before the last present call 
			//! gets processed, the current blocks until the last present call gets finished to prevent error.
			virtual RV present(IResource* resource, uint32 subresource, uint32 sync_interval) = 0;

			//! Changes the count, size or format of the back buffer. This should be called when the window is resized.
			//! @param[in] buffer_count The new buffer count. Set to 0 to preserve the former buffer count.
			//! @param[in] width The width of the back buffer. Specify 0 will determine the size from the bounding window's native size.
			//! @param[in] height The height of the back buffer. Specify 0 will determine the size from the bounding window's native size.
			//! @param[in] new_format The new format of the back buffer.
			//! @return Returns success if the resize operation succeeded, failure otherwise.
			virtual RV resize_buffers(uint32 buffer_count, uint32 width, uint32 height, EResourceFormat new_format) = 0;
		};
	}
}