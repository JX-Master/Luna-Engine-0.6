// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Core.hpp
* @author JXMaster
* @date 2019/1/8
* @brief Core module 2.0 include file.
 */
#pragma once
#include "IFile.hpp"
#include "IFileIterator.hpp"
//#include "IRandom.hpp"
#include "ISignal.hpp"
#include "IMutex.hpp"
#include "ISemaphore.hpp"
#include "IRunnable.hpp"
#include "IThread.hpp"
#include "IFiber.hpp"
//#include "ILogger.hpp"
//#include "ILogHandler.hpp"
#include "ISerializable.hpp"
#include "IFileSystem.hpp"
#include "IEncoder.hpp"
#include "IDecoder.hpp"
#include "IMemoryStream.hpp"
#include "IDispatchQueue.hpp"
#include "Error.hpp"

#ifndef LUNA_CORE_API
#define LUNA_CORE_API
#endif

namespace Luna
{
	enum class EFileMatchFlag : u32
	{
		none = 0,			// Does not ignore anything.
		no_file = 1,		// Ignore any file that is not a directory.
		no_dir = 2,			// Ignore any file that is a directory.
		no_placeholder = 4,	// Ignore "." (current directory) and ".." (parent directory) if any.
	};

	//---------------------------- Core APIs ----------------------------
	//	APIs that provides core functionalities.

	// ---------------------------------------------------------------------------------------------------------
	//		FILE SYSTEM
	// ---------------------------------------------------------------------------------------------------------

	//! Opens one file.
	//! @param[in] filename The path of the file.
	//! @param[in] flags The file open flags.
	//! @param[in] creation Specify whether to create a file if the file does not exist.
	//! @return If succeeded, returns the new opened file object. If failed, returns one of the following error codes:
	//! * BasicError::bad_arguments
	//! * BasicError::access_denied
	//! * BasicError::not_found
	//! * BasicError::not_directory
	//! * BasicError::bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RP<IFile>	platform_open_file(const c8* filename, EFileOpenFlag flags, EFileCreationMode creation);
	//! Gets the file or directory attribute.
	//! @param[in] filename The path of the file to check.
	//! @return Returns the file attribute structure, or one of the following error codes if failed:
	//! * BasicError::access_denied
	//! * BasicError::not_found
	//! * BasicError::not_directory
	//! * BasicError::bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API R<FileAttribute> platform_file_attribute(const c8* filename);
	//! Copies the file from the source path to the destination path.
	//! @param[in] from_filename Source file path.
	//! @param[in] to_filename Destination file path.
	//! @param[in] fail_if_exists If this is `true` and the file named `to_filename` already exists,
	//! this call fails with return value `e_item_already_exists`. If this is `false` and the file named `to_filename` 
	//! already exists, the file named `to_filename` will be overwritten.
	//! @return This call returns one of the following results:
	//! * s_ok
	//! * e_bad_arguments
	//! * e_file_already_exists
	//! * e_access_denied
	//! * e_file_not_exist
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RV	platform_copy_file(const c8* from_filename, const c8* to_filename, bool fail_if_exists = false);
	//! Moves the file from the source path to the destination path. This call can also be used to rename a file.
	//! @param[in] from_filename Source file path.
	//! @param[in] to_filename Destination file path.
	//! @param[in] allow_copy If the `from_filename` cannot be moved `to_filename` directly, set this to `true` allows 
	//! the system to copy the file from `from_filename` to `to_filename` and delete file in `from_filename`. If this 
	//! is `false` and the file cannot be moved, this call failed.
	//! @param[in] fail_if_exists If this is `true` and the file named `to_filename` already exists,
	//! this call fails with return value `e_item_already_exists`. If this is `false` and the file named `to_filename` 
	//! already exists, the file named `to_filename` will be overwritten.
	//! @return This call returns one of the following results:
	//! * s_ok
	//! * e_bad_arguments
	//! * e_file_already_exists
	//! * e_access_denied
	//! * e_file_not_exist
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RV	platform_move_file(const c8* from_filename, const c8* to_filename, bool allow_copy = true, bool fail_if_exists = false);
	//! Deletes the specified file.
	//! @param[in] filename The file to delete.
	//! @return This call returns one of the following results:
	//! * s_ok
	//! * e_bad_arguments
	//! * e_file_not_exist
	//! * e_access_denied
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RV	platform_delete_file(const c8* filename);
	//! Creates a file iterator that can be used to iterate all files in the specified directory.
	//! @param[in] dir_path The directory path to open.
	//! @return Returns a file iterator object if succeeded. The result will be one of the following:
	//! * s_ok
	//! * e_file_not_exist
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RP<IFileIterator> platform_open_dir(const c8* dir_path);
	//! Creates one directory.
	//! @param[in] pathname The path of the directory.
	//! @return This call returns one of the following results:
	//! * s_ok
	//! * e_path_already_exists
	//! * e_path_not_found
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RV	platform_create_dir(const c8* pathname);
	//! Removes one directory.
	//! @param[in] pathname The path of the directory.
	//! @return This call returns one of the following results:
	//! * s_ok
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RV	platform_remove_dir(const c8* pathname);

	//! Get the current working directory path for the underlying system.
	//! The default current working directory is set to the path that contains the executable file.
	//! @param[in] buffer_length The length of the buffer for the current directory string, including the null terminator.
	//! @param[in] A pointer to the buffer that receives the current directory string. To gets the required buffer size, specify
	//! `buffer` to `nullptr` and `buffer_length` to 0.
	//! @return Returns the number of characters copied into the buffer, including the null terminator. If `buffer_length` is 0 and
	//! `buffer` is `nullptr`, returns the required buffer size to fetch the current directory, including the null terminator. 
	LUNA_CORE_API u32	get_current_dir(u32 buffer_length, c8* buffer);
	//! Set the current working directory path for the underlying system. The current directory will be set for the process scope.
	LUNA_CORE_API RV set_current_dir(const c8* path);
	//! Get the application executable file's absolute directory, ended with application executable name.
	//! @return Returns the process path. The string is static and valid until the engine is closed.
	LUNA_CORE_API const c8* get_process_path();

	// ---------------------------------------------------------------------------------------------------------
	//		RANDOM NUMBER GENERATION
	// ---------------------------------------------------------------------------------------------------------

	//! Creates a new random number generator.
	//LUNA_CORE_API P<IRandom> new_random(u32 initial_seed);

	LUNA_CORE_API u32 random_u32();
	LUNA_CORE_API i32 random_i32();
	LUNA_CORE_API u64 random_u64();
	LUNA_CORE_API i64 random_i64();
	LUNA_CORE_API f32 random_f32(f32 range_begin, f32 range_end);
	LUNA_CORE_API f64 random_f64(f64 range_begin, f64 range_end);
	LUNA_CORE_API Guid random_guid();

	// ---------------------------------------------------------------------------------------------------------
	//		SYNCHRONIZATION OBJECTS CREATION
	// ---------------------------------------------------------------------------------------------------------

	//! Create a new signal object.
	//! @param[in] manual_reset If we need to manually reset the trigger state of the signal object.
	//! If set to `false`, the signal will be automatically reset to not triggered state when a single thread that waiting for this
	//! signal is passed.
	//! If set to `true`, user needs to manually reset the state by calling `ISignal::Reset`.
	//! @return Returns the new created signal object.
	LUNA_CORE_API P<ISignal> new_signal(bool manual_reset);
	//! Create a new mutex object.
	//! @return Returns the new created mutex object.
	LUNA_CORE_API P<IMutex> new_mutex();
	//! Create a new semaphore object.
	//! @param[in] initial_count The initial count of the semaphore.
	//! @param[in] max_count The max count the specified semaphore can reach to.
	//! @return Returns the new created semaphore object.
	LUNA_CORE_API P<ISemaphore> new_semaphore(i32 initial_count, i32 max_count);

	// ---------------------------------------------------------------------------------------------------------
	//		THREAD AND FIBER MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------

	//! Create a new system thread and make it run the callback function. The thread will be closed when the callback function returns.
	//! @param[in] callback The callback object to invoke.
	//! @param[in] params The parameter object passed to callback function Invoke method as parameter. The system keeps a reference to the parameter
	//! object until the thread is closed.
	//! @param[in] stackSize The stack size for new thread's call stack.
	//! @return Returns an interface representing the new created thread, or `nullptr` if failed. 
	LUNA_CORE_API RP<IThread> new_thread(IRunnable* callback, u32 stack_size = 0);

	//! Gets the thread object of current running thread.
	//! @return The thread object of the current thread.
	LUNA_CORE_API IThread* get_current_thread();

	//! Gets the thread object of the main thread.
	//! @return The thread object of the main thread.
	LUNA_CORE_API IThread* get_main_thread();

	//! Converts the current thread into a fiber. You must convert a thread into a fiber before you can schedule other fibers.
	//! Returns the fiber object used to switch back to the main thread. The return code is 
	//! `s_ok` for the first time this is called, or `s_already_done` if the current thread has already been converted to fiber.
	LUNA_CORE_API RP<IFiber> convert_thread_to_fiber();

	//! Converts the current fiber to thread. This call destroys the fiber context for the specified thread.
	//! If you don't do this, the fiber context will be destroyed when the thread is destroyed.
	LUNA_CORE_API RV convert_fiber_to_thread();

	//! Create a new fiber context. The created fiber will be executed the first time you call `switch_to_fiber` on any of the threads.
	//! @param[in] callback The callback object to be executed.
	//! @param[in] stack_size Optional. The stack size of the new fiber's call stack. Specify 0 to let system choose a proper value.
	//! @return Returns the fiber interface that can be used to switch to, or `nullptr` if failed to create the fiber.
	//! 
	//! The error code may be:
	//!	* e_bad_system_call
	LUNA_CORE_API RP<IFiber> new_fiber(IRunnable* callback, u32 stack_size = 0);

	//! Gets the the fiber object of current running fiber.
	//! @return Returns the fiber object of the current thread if a fiber is running, 
	//! or `nullptr` if no fiber is running on this thread (the main thread entry function is running, which is not a fiber).
	LUNA_CORE_API IFiber* get_current_fiber();

	//! Resumes the target fiber.
	LUNA_CORE_API RV resume_fiber(IFiber* fiber);

	//! Yields the current fiber.
	LUNA_CORE_API RV yield_current_fiber();

	// ---------------------------------------------------------------------------------------------------------
	//		LOG SYSTEM
	// ---------------------------------------------------------------------------------------------------------

	//! Creates a new logger.
	//! @param[in] channel The channel that the logger will log the message to.
	/*LUNA_CORE_API P<ILogger> new_logger(IName* channel);

	//! Creates a new log handler. 
	//! The system holds one weak reference to the handler, and the handler holds one weak reference to the 
	//! callback as well. So the user should keep a strong reference to both handler object and delegate object
	//! to make it work.
	LUNA_CORE_API P<ILogHandler> new_log_handler(IName* name, ILogCallback* callback);

	//! Gets the log handler with its name.
	LUNA_CORE_API RP<ILogHandler> get_log_handler(IName* name);*/

	// ---------------------------------------------------------------------------------------------------------
	//		UPDATE LOOP MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------

	//! Indicates the system that a new frame is started. This should be called once 
	//! per frame.
	//! This must be called by the main thread.
	LUNA_CORE_API void new_frame();

	//! Get the current frame index. Every time `ISystem::update` is called, the frame index will be increased by 1.
	//! The engine starts the first frame immediately after initialized, so the first frame is 1 before the first call to `ISystem::update`.
	LUNA_CORE_API u32 get_frame_index();

	//! Get the delta time of last frame, that is, the elapsed time between the last 2 `ISystem::update` call.
	//! The time is clamped by min govern time and max govern time.
	LUNA_CORE_API u64 get_last_frame_ticks();

	//! Get the delta time of last frame, that is, the elapsed time between the last 2 `ISystem::update` call.
	//! The time is clamped by min govern time and max govern time.
	LUNA_CORE_API f64 get_last_frame_seconds();

	//! Get the actual delta time of last frame, that is, the frame time before modified by frame governing feature 
	//! and max frame time clamping feature.
	//! This frame time is for debug use, not for updating the system.
	LUNA_CORE_API u64 get_last_frame_ticks_raw();

	//! Get the frame ticks of the specified frames. Luna Engine records the most recent 256 frame ticks for profiling use
	//! in every debug level build.
	//! @param[in] frame_index The index of frame to check. 0 means the latest frame (the last frame) and 255 means the 
	//! furthest frame.
	LUNA_CORE_API u64 get_last_n_frame_ticks(u32 frame_index);

	//! Get the actual frame ticks before modified by frame governing feature and max frame time clamping feature.
	//! This frame time is for debug use, not for updating the system.
	//! @param[in] frame_index The index of frame to check. 0 means the latest frame (the last frame) and 255 means the 
	//! furthest frame.
	LUNA_CORE_API u64 get_last_n_frame_ticks_raw(u32 frame_index);

	//! Get the frames per second value judged from last frame ( 1.0f / get_last_frame_seconds ).
	LUNA_CORE_API f32 get_fps();

	//! Get the frames per second value judged from last frame's raw ticks ( 1.0f / (get_last_frame_raw_ticks() / get_ticks_per_second()))
	LUNA_CORE_API f32 get_fps_raw();

	//! Get the frames per second value averaged from last 120 frames.
	LUNA_CORE_API f32 get_fps_averaged();

	//! Get the frames per second value averaged from last 120 frame's raw data.
	LUNA_CORE_API f32 get_fps_averaged_raw();

	//! Get the elapsed ticks from last call to `new_frame()`.
	LUNA_CORE_API u64 get_frame_elapsed_ticks();

	//! Get the elapsed ticks from last call to `new_frame()`.
	LUNA_CORE_API f64 get_frame_elapsed_seconds();

	//! Set frame governing feature enabled/disabled. This feature is disabled by default.
	//! When the frame governing is enabled, the system will try to control the frame time between two
	//! `ISystem::update` call to a specified value set_char by `ITimeSystem::set_frame_governing_fps'.
	//! If the frame time is smaller than the govern period value, the system will suspend the thread or
	//! yield the thread several times until the govern frame time is reached. If the frame time is greater
	//! than the govern frame time, the system does nothing.
	LUNA_CORE_API void set_frame_governing_enabled(bool enabled);

	//! Check if the frame governing feature is enabled.
	LUNA_CORE_API bool is_frame_governing_enabled();

	//! Set the govern value used by frame governing features.
	//! The value is set_char to 60.0f by default.
	LUNA_CORE_API RV set_frame_governing_fps(f32 frames_per_second);

	//! Get the govern value used by frame governing features.
	LUNA_CORE_API f32 get_frame_governing_fps();

	//! Set min frame fps clamping enabled/disabled. This feature is disabled by default.
	//! Min frame fps clamping will clamp the frame time to a specified value if the frame time is larger than
	//! the specified value. This is necessary in order to keep some time-related system (like physical simulation)
	//! stable and prevent a large frame time peak ruins the system when the system runs too slow or paused by debug break.
	LUNA_CORE_API void set_min_frame_fps_clamping_enabled(bool enabled);

	//! Check if the max frame time clamping feature is enabled.
	LUNA_CORE_API bool is_min_frame_fps_clamping_enabled();

	//! Set the value used to clamping max frame time. Default value is 10.0f (at least 10 frames per second).
	LUNA_CORE_API void set_min_frame_fps(f32 frames_per_second);

	//! Get the value used to clamping max frame time.
	LUNA_CORE_API f32 get_min_frame_fps();

	// ---------------------------------------------------------------------------------------------------------
	//		VIRTUAL FILE SYSTEM
	// ---------------------------------------------------------------------------------------------------------

	//! Opens one file from the LUNA_CORE_API file system.
	//! @param[in] filename The path of the file relative to the root path of the file system.
	//! @param[in] flags The file open flags.
	//! @param[in] creation Specify whether to create a file if the file does not exist.
	//! @return If succeeded, returns the new opened file object.
	LUNA_CORE_API RP<IFile>	open_file(const Path& filename, EFileOpenFlag flags, EFileCreationMode creation);

	//! Gets the file attribute from the LUNA_CORE_API file system.
	LUNA_CORE_API R<FileAttribute> file_attribute(const Path& filename);

	//! Copies the file from the source path to the destination path in the LUNA_CORE_API file system.
	//! If the source file system and the destination file system is not the same, the file will always be copied
	//! by loading all of its content into the temp buffer and writes them to the destination file system.
	//! @param[in] from_filename Source file path.
	//! @param[in] to_filename Destination file path.
	//! @param[in] fail_if_exists If this is `true` and the file named `to_filename` already exists,
	//! this call fails with return value `e_item_already_exists`. If this is `false` and the file named `to_filename` 
	//! already exists, the file named `to_filename` will be overwritten.
	LUNA_CORE_API RV	copy_file(const Path& from_filename, const Path& to_filename, bool fail_if_exists = false);

	//! Moves the file from the source path to the destination path in the virtual file system. This call can also 
	//! be used to rename a file.
	//! If the source file system and the destination file system is not the same, the file will always be copied
	//! to the new file system and deletes the old one. In such case, `allow_copy` = `false` will fail.
	//! @param[in] from_filename Source file path.
	//! @param[in] to_filename Destination file path.
	//! @param[in] allow_copy If the `from_filename` cannot be moved `to_filename` directly, set this to `true` allows 
	//! the system to copy the file from `from_filename` to `to_filename` and delete file in `from_filename`. If this 
	//! is `false` and the file cannot be moved, this call failed.
	//! @param[in] fail_if_exists If this is `true` and the file named `to_filename` already exists,
	//! this call fails with return value `e_item_already_exists`. If this is `false` and the file named `to_filename` 
	//! already exists, the file named `to_filename` will be overwritten.
	LUNA_CORE_API RV	move_file(const Path& from_filename, const Path& to_filename, bool allow_copy = true, bool fail_if_exists = false);

	//! Deletes the specified file in the virtual file system.
	//! @param[in] filename The file to delete.
	LUNA_CORE_API RV	delete_file(const Path& filename);

	//! Creates a file iterator that can be used to iterate all files in the specified directory in the virtual file system.
	//! @param[in] dir_path The directory path to open.
	//! @return Returns a file iterator object if succeeded.
	LUNA_CORE_API RP<IFileIterator> open_dir(const Path& dir_path);

	//! Creates one directory in the virtual file system.
	//! @param[in] pathname The path of the directory.
	LUNA_CORE_API RV	create_dir(const Path& pathname);

	//! Removes one directory in the virtual file system.
	//! @param[in] pathname The path of the directory.
	//! @param[in] recursive If `true`, this call removes the parent folder as well as all contents in the folder. If `false`, 
	//! only the parent folder will be removed, and it must be empty or the call will fail.
	LUNA_CORE_API RV	remove_dir(const Path& pathname, bool recursive = false);

	//! Mounts one file system to the virtual file system. The virtual file system keeps a strong reference to the file system
	//! interface.
	//! 
	//! Note that the later mounted file system will covers the former mounted one, so if one mount point is a sub-path of 
	//! another mount point, be sure to mount the parent file system and then mount the child file system.
	LUNA_CORE_API RV mount_fs(const Path& mount_point, IFileSystem* fs);

	//! Mounts one virtual system's path to another virtual system's path.
	LUNA_CORE_API RV mount_virtual_path(const Path& mount_point, const Path& vfs_path);

	//! Mounts a platform's native path to a virtual system's path.
	LUNA_CORE_API RV mount_platfrom_path(const Path& mount_point, const Path& platform_path);

	//! Unmounts one file system at the specified mount point.
	LUNA_CORE_API RV unmount_fs(const Path& mount_point);

	//! Gets a string array that describes all mount points sorted by their search order.
	LUNA_CORE_API Vector<String> mount_points();

	//! Gets the file system interface mounted on the specified mount point.
	LUNA_CORE_API RP<IFileSystem> get_fs(const Path& mount_point);

	// ---------------------------------------------------------------------------------------------------------
	//		DISPATCHING SYSTEM
	// ---------------------------------------------------------------------------------------------------------

	//! Creates a new dispatch queue.
	//! @param[in] thread_limit Determines how many tasks in the queue can be executed at the same time by 
	//! worker threads. Two special values are 0 and 1: value 0 means no limit, value 1 means the next
	//! task in the queue will not be executed until the last task has been finished, such queue is called 
	//! a serialized queue, because tasks in the queue are executed one by one.
	LUNA_CORE_API P<IDispatchQueue> new_dispatch_queue(u32 concurrency_limit = 0);

	// ---------------------------------------------------------------------------------------------------------
	//		UTILITY
	// ---------------------------------------------------------------------------------------------------------

	//! Creates one new memory stream.
	//! @return Returns the created memory stream.
	LUNA_CORE_API P<IMemoryStream> new_memory_stream();

	//! Creates one text encoder.
	LUNA_CORE_API P<IEncoder> new_text_encoder();

	//! Creates one text decoder.
	LUNA_CORE_API P<IDecoder> new_text_decoder();

	//---------------------------- Auxiliary APIs ----------------------------
	//	APIs that provides convenient functions to use the Core APIs.

	//! Implement this function to enable module scope global logging.
	/*inline ILogger* get_logger();
	
	inline RV log(ELogType log_type, const char* msg)
	{
		return get_logger()->log(log_type, msg);
	}
	inline RV vlogf(ELogType log_type, const char* fmt, va_list args)
	{
		return get_logger()->vlogf(log_type, fmt, args);
	}
	inline RV logf(ELogType log_type, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		RV r = vlogf(log_type, fmt, args);
		va_end(args);
		return r;
	}
	inline RV log_comment(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		RV r = vlogf(ELogType::comment, fmt, args);
		va_end(args);
		return r;
	}
	inline RV log_message(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		RV r = vlogf(ELogType::message, fmt, args);
		va_end(args);
		return r;
	}
	inline RV log_warning(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		RV r = vlogf(ELogType::warning, fmt, args);
		va_end(args);
		return r;
	}
	inline RV log_error(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		RV r = vlogf(ELogType::error, fmt, args);
		va_end(args);
		return r;
	}
	inline RV log_fatal_error(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		RV r = vlogf(ELogType::fatal_error, fmt, args);
		va_end(args);
		return r;
	}*/
}