// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Gfx.hpp
* @author JXMaster
* @date 2019/7/10
*/
#pragma once
#include <Core/Core.hpp>

#ifndef LUNA_GFX_API
#define LUNA_GFX_API
#endif

#include "WindowEvent.hpp"
#include "IGraphicAdapter.hpp"
#include "IGraphicDevice.hpp"
#include "IWindow.hpp"
#include "IWindowInputDevice.hpp"
#include "ISwapChain.hpp"

namespace luna
{
	namespace gfx
	{
		enum class EWindowCreationFlag : uint32
		{
			none = 0x00,
			no_resize = 0x01,		// window cannot be resized by dragging the border.
			borderless = 0x02,	// window is borderless.
			hidden = 0x03,		// window will not displayed immediately after creation.
		};

		struct IWindow;

		enum class EShaderSourceType : uint32
		{
			platform_default = 0,
			hlsl = 1
		};

		enum class EShaderTargetType : uint32
		{
			platform_default = 0,
			dx_bytecode = 1,
		};

		enum class EShaderType : uint32
		{
			vertex = 1,
			pixel = 2,
			compute = 3,
			hull = 4,
			domain = 5,
			geometry = 6
		};

		enum class EShaderModel : uint32
		{
			sm_5_0,
		};

		enum class EShaderCompileFlag : uint32
		{
			none = 0x00,
			debug_info = 0x01,
			skip_validation = 0x02,
		};

		enum class EShaderOptimizationLevel : uint32
		{
			none,
			speed,
			full
		};

		struct ShaderCompileDesc
		{
			//! The data of the shader source.
			const void* src_data;
			//! The size of the data of the shader source in bytes.
			size_t src_size;
			//! The debug name of the source code (file name), set to `nullptr` if this
			//! is not needed.
			const char* src_name;
			//! The name of the entry point of the shader.
			const char* entry_point;
			//! The source code type.
			EShaderSourceType src_type;
			//! The target binary code type.
			EShaderTargetType target_type;
			//! The type of the shader to compile to.
			EShaderType shader_type;
			//! The shader model to compile to.
			EShaderModel shader_model;
			//! The optimization level used when compiling shaders.
			EShaderOptimizationLevel optimization_level;
			//! Additional flags to set.
			EShaderCompileFlag flags;

			ShaderCompileDesc() = default;
			ShaderCompileDesc(
				const void* _src_data,
				size_t _src_size,
				const char* _src_name,
				const char* _entry_point,
				EShaderSourceType _src_type,
				EShaderTargetType _target_type,
				EShaderType _shader_type,
				EShaderModel _shader_model,
				EShaderOptimizationLevel _optimization_level,
				EShaderCompileFlag _flags
			) :
				src_data(_src_data),
				src_size(_src_size),
				src_name(_src_name),
				entry_point(_entry_point),
				src_type(_src_type),
				target_type(_target_type),
				shader_type(_shader_type),
				shader_model(_shader_model),
				optimization_level(_optimization_level),
				flags(_flags) {}

		};

		enum class EMessageBoxType : uint32
		{
			ok = 1,
			ok_cancel = 2,
			retry_cancel = 3,
			yes_no = 4,
			yes_no_cancel = 5,
		};

		enum class EMessageBoxIcon : uint32
		{
			none = 0,
			information = 1,
			warning = 2,
			question = 3,
			error = 4
		};

		enum class EMessageBoxButton : uint32
		{
			ok = 1,
			cancel,
			retry,
			yes,
			no
		};

		constexpr int32 window_default_v = 0x80000000;

		struct IGraphicAdapter;
		struct IGraphicDevice;
		struct ISwapChain;
		struct ICommandQueue;

		enum class EFileOpenDialogFlag : uint32
		{
			none = 0,
			//! Allows multiple files to be selected.
			multi_select = 0x01,
		};

		LUNA_GFX_API RV init();

		//! Create a new window. The window will be displayed unless EWindowCreationFlag::hidden is set.
		//! @param[in] title The title of the window.
		//! @param[in] x The x position of the window in screen space. set to `window_default_v` to let the system choose the proper value.
		//! @param[in] y The y position of the window in screen space. set to `window_default_v` to let the system choose the proper value.
		//! @param[in] w The width of the window. set to `window_default_v` to let the system choose the proper value.
		//! @param[in] h The height of the window. set to `window_default_v` to let the system choose the proper value.
		//! @param[in] window_creation_flags Additional window creation flags.
		//! @return Returns the window object, or `nullptr` if failed.
		LUNA_GFX_API RP<IWindow> new_window(const char* title, int32 x = window_default_v, int32 y = window_default_v,
			int32 w = window_default_v, int32 h = window_default_v, EWindowCreationFlag window_creation_flags = EWindowCreationFlag::none);

		//! Get information about the graphic adapters installed on this platform.
		//! @param[in] index The index of the adapters to be enumerated.
		//! @return The adapter interface, or `nullptr` if the index is invalid.
		LUNA_GFX_API RP<IGraphicAdapter> enum_adapter(uint32 index);

		//! Creates a new graphic device instance and initialize the corresponding graphic subsystem for this application.
		//! @param[in] adapter The graphic adapter to use to create the device. Specify `nullptr` to let system choose the proper value.
		LUNA_GFX_API RP<IGraphicDevice> new_device(IGraphicAdapter* adapter = nullptr);

		//! Creates a swap chain resource and binds it to the specified window.
		//! @param[in] queue The command queue to push the present commands to.
		//! @param[in] window The window this swap chain should be outputted to.
		//! @param[in] desc The descriptor object of the swap chain.
		//! @return Returns the new created swap chain, or `nullptr` if failed to create.
		LUNA_GFX_API RP<ISwapChain> new_swap_chain(ICommandQueue* queue, IWindow* window, const SwapChainDesc& desc);

		//! Compile shader source code.
		//! @param[in] desc The descriptor object of the shader.
		//! @return Returns a blob that contains the compiled shader output, or `nullptr` if failed to compile the shader.
		LUNA_GFX_API RP<IBlob> compile_shader(const ShaderCompileDesc& desc);

		//! Displays one message box dialog. The current thread blocks until the dialog is closed.
		//! @return Returns the clicked button.
		LUNA_GFX_API R<EMessageBoxButton> message_box(const char* text, const char* caption, EMessageBoxType type, EMessageBoxIcon icon = EMessageBoxIcon::none);

		//! Displays one open file dialog and fetches the selecting results.
		//! @param[in] filter The filter string used by the open file dialog. The string contains multiple substrings, each of the substring ends with a NULL character (`\0`), and
		//! the last substring ends with two NULL characters. Every item in the filter contains a pair of substrings, the first substring is the text that describes the filter, and 
		//! the second substring describes the filter (like "*.la"). If the filter contains multiple patterns, they are separated by semicolon (like "*.la;*.lb").
		//! @param[in] initial_dir The initial directory to set the file dialog to. If this is `nullptr`, the system decides the default directory. This path must be a platform native
		//! path if specified.
		//! @param[in] flags See EFileOpenDialogFlag.
		//! @return Returns a list of selected file paths. All paths are platform native, absolute paths. This function returns failure if the user fails to select any file.
		LUNA_GFX_API R<Vector<P<IMutPath>>> open_file_dialog(const char* filter, const char* title = nullptr, IPath* initial_dir = nullptr, EFileOpenDialogFlag flags = EFileOpenDialogFlag::none);

		//! Displays one save file dialog and fetches the selecting results.
		LUNA_GFX_API RP<IMutPath> save_file_dialog(const char* filter, const char* title = nullptr, IPath* initial_file_path = nullptr);

		//! Displays one open directory dialog and fetches the selecting results.
		LUNA_GFX_API RP<IMutPath> open_dir_dialog(const char* title = nullptr, IPath* initial_dir = nullptr);
	}
}