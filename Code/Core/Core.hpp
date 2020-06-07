// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Core.hpp
* @author JXMaster
* @date 2019/1/8
* @brief Core module 1.0 include file.
 */
#pragma once
#include <Base/Interface.hpp>
#include <Base/Base64.hpp>
#include <Base/Unicode.hpp>

#include "IFile.hpp"
#include "IAsyncFile.hpp"
#include "IFileIterator.hpp"
#include "IHeap.hpp"
#include "IRandom.hpp"
#include "ISignal.hpp"
#include "IMutex.hpp"
#include "ISemaphore.hpp"
#include "IReadWriteLock.hpp"
#include "IRunnable.hpp"
#include "IThread.hpp"
#include "IFiber.hpp"
#include "IName.hpp"
#include "IStringBuffer.hpp"
#include "IBlob.hpp"
#include "IMutPath.hpp"
#include "ILogger.hpp"
#include "ILogHandler.hpp"
#include "IVariant.hpp"
#include "ISerializable.hpp"
#include "IBuffer.hpp"
#include "IFileSystem.hpp"
#include "IEncoder.hpp"
#include "IDecoder.hpp"
#include "IMemoryStream.hpp"
#include "IError.hpp"
#include "IDispatchQueue.hpp"

#ifndef LUNA_CORE_API
#define LUNA_CORE_API
#endif

namespace luna
{
	enum class ESystemInfo : uint32
	{
		processors_count = 1,	// uint32* Reports the number of processors on the platform.
	};

	enum class ECPUFeatureFlag : uint64
	{
		none = 0x00,
		MMX = 0x01,		// The MMX instruction set is available. 
		SSE = 0x02,		// The SSE instruction set is available.
		SSE2 = 0x04,	// The SSE2 instruction set is available.
		SSE3 = 0x08,	// The SSE3 instruction set is available.
	};

	enum class EHeapCreationFlag : uint32
	{
		none = 0x00,
		enable_execute = 0x01,	// If the data placed in this heap can be executed.
		no_thread_safe = 0x02,	// If the access to the heap is thread-safe. Methods of the heap is thread-safe unless this is set.
	};

	enum class EPageProtection : uint32
	{
		no_access = 1,
		read = 2,
		read_write = 3,
		execute = 4,
		execute_read = 5,
		execute_read_write = 6,
	};

	struct DateTime
	{
		uint16 year;
		uint16 month;
		uint16 day_of_week; // 0-6 -> Sunday-Saturday
		uint16 day;
		uint16 hour;
		uint16 minute;
		uint16 second;
		uint16 milliseconds;
	};

	enum class EFileMatchFlag : uint32
	{
		none = 0,			// Does not ignore anything.
		no_file = 1,		// Ignore any file that is not a directory.
		no_dir = 2,			// Ignore any file that is a directory.
		no_placeholder = 4,	// Ignore "." (current directory) and ".." (parent directory) if any.
	};

	using module_deinit_func_t = void(void);

	//---------------------------- Engine APIs ----------------------------
	//	APIs that initializes the engine and modules.

	//! Initializes the engine instance.
	LUNA_CORE_API RV init();

	//! Shutdowns the engine.
	LUNA_CORE_API void shutdown();

	//! Adds one module to the engine. The user should initialize the module before added to the engine.
	//! @param[in] module_name The name of the module. This string must be valid until the module is deinitialized.
	//! One module with the same name will be added only once, the succeeding calls adds the reference count of the 
	//! module.
	//! @param[in] deinit_func The function to be called by system when the module needs to be deinitialized. Only the
	//! deinitialize function provided first time for one module will be used.
	LUNA_CORE_API void add_module(const char* module_name, module_deinit_func_t* deinit_func);

	//! Decreases the reference count of the module by 1, and deinitializes and removes the module if the reference count
	//! drops to 0. The user must be sure that no other modules are referencing the module, and all interfaces created
	//! in the module has been released before deinitialize the module.
	//! 
	//! The engine automatically deinitializes all modules in the reverse order they are initialized when the engine is 
	//! shutdown, so there is usually no need to call this manually.
	LUNA_CORE_API RV remove_module(const char* module_name);

	//---------------------------- Core APIs ----------------------------
	//	APIs that provides core functionalities.

	// ---------------------------------------------------------------------------------------------------------
	//		DEBUG FEATURE
	// ---------------------------------------------------------------------------------------------------------

		//! Prints a message to the debug console attached to this process.
	LUNA_CORE_API RV debug_printf(const char* fmt, ...);

	//! Prints a message to the debug console attached to this process with `va_list` as arguments.
	LUNA_CORE_API RV debug_vprintf(const char* fmt, va_list args);

	// ---------------------------------------------------------------------------------------------------------
	//		FILE SYSTEM
	// ---------------------------------------------------------------------------------------------------------

	//! Opens one file.
	//! @param[in] filename The path of the file.
	//! @param[in] flags The file open flags.
	//! @param[in] creation Specify whether to create a file if the file does not exist.
	//! @return If succeeded, returns the new opened file object. The result will be one of the following:
	//! * s_ok
	//! * e_bad_arguments
	//! * e_access_denied
	//! * e_file_not_exist
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RP<IFile>	platform_open_file(const char* filename, EFileOpenFlag flags, EFileCreationMode creation);

	//! Opens one file in asynchronous mode. That is, read and write calls to the file object will be asynchronous.
	//! @param[in] filename The path of the file.
	//! @param[in] flags The file open flags.
	//! @param[in] creation Specify whether to create a file if the file does not exist.
	//! @return If succeeded, returns the new opened file object. The result will be one of the following:
	//! * s_ok
	//! * e_bad_arguments
	//! * e_access_denied
	//! * e_file_not_exist
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RP<IAsyncFile> platform_open_file_async(const char* filename, EFileOpenFlag flags, EFileCreationMode creation);

	//! Gets the file attribute.
	LUNA_CORE_API RV platform_file_attribute(const char* filename, FileAttribute& attribute);

	//! Checks if a file or directory exists.
	//! @return s_ok If file exists, e_file_not_exist if the file does not exist, any other error value if an error occurred
	//! when trying to find file.
	LUNA_CORE_API RV platform_file_exists(const char* filename);

	//! Checks if a directory exists.
	//! @return s_ok If directory exists, e_dir_not_exist if the directory does not exist, any other error value if an error occurred
	//! when trying to find directory.
	LUNA_CORE_API RV platform_dir_exists(const char* dir_name);

	//! Gets the size of the file.
	//! @param[in] filename The path of the file.
	//! @return Returns the size of the file, or 0 if failed to read the size of the file. The information will be one of the following:
	//! * s_ok
	//! * e_bad_arguments
	//! * e_access_denied
	//! * e_file_not_exist
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API R<uint64>	platform_file_size(const char* filename);
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
	LUNA_CORE_API RV	platform_copy_file(const char* from_filename, const char* to_filename, bool fail_if_exists = false);
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
	LUNA_CORE_API RV	platform_move_file(const char* from_filename, const char* to_filename, bool allow_copy = true, bool fail_if_exists = false);
	//! Deletes the specified file.
	//! @param[in] filename The file to delete.
	//! @return This call returns one of the following results:
	//! * s_ok
	//! * e_bad_arguments
	//! * e_file_not_exist
	//! * e_access_denied
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RV	platform_delete_file(const char* filename);
	//! Creates a file iterator that can be used to iterate all files in the specified directory.
	//! @param[in] dir_path The directory path to open.
	//! @return Returns a file iterator object if succeeded. The result will be one of the following:
	//! * s_ok
	//! * e_file_not_exist
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RP<IFileIterator> platform_open_dir(const char* dir_path);
	//! Creates one directory.
	//! @param[in] pathname The path of the directory.
	//! @return This call returns one of the following results:
	//! * s_ok
	//! * e_path_already_exists
	//! * e_path_not_found
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RV	platform_create_dir(const char* pathname);
	//! Removes one directory.
	//! @param[in] pathname The path of the directory.
	//! @return This call returns one of the following results:
	//! * s_ok
	//! * e_bad_system_call for all errors that cannot be identified.
	LUNA_CORE_API RV	platform_remove_dir(const char* pathname);

	//! Get the current working directory path for the underlying system.
	//! The default current working directory is set to the path that contains the executable file.
	//! @param[in] buffer_length The length of the buffer for the current directory string, including the null terminator.
	//! @param[in] A pointer to the buffer that receives the current directory string. To gets the required buffer size, specify
	//! `buffer` to `nullptr` and `buffer_length` to 0.
	//! @return Returns the number of characters copied into the buffer, including the null terminator. If `buffer_length` is 0 and
	//! `buffer` is `nullptr`, returns the required buffer size to fetch the current directory, including the null terminator. 
	LUNA_CORE_API uint32	get_current_dir(uint32 buffer_length, char* buffer);
	//! Set the current working directory path for the underlying system. The current directory will be set for the process scope.
	LUNA_CORE_API RV set_current_dir(const char* path);
	//! Get the application executable file's absolute directory, ended with application executable name.
	//! @return Returns the process path. The string is static and valid until the engine is closed.
	LUNA_CORE_API const char* get_process_path();

	// ---------------------------------------------------------------------------------------------------------
	//		SYSTEM INFO QUERY
	// ---------------------------------------------------------------------------------------------------------

	//! Gets the information about the underlying system / platform.
	//! @param[in] system_info The system information you want to query.
	//! @param[out] data The data type to fetch the system info. See ESystemInfo for details about which data 
	//! type to pass into.
	//! @return Returns `s_ok` if the system information is successfully queried.
	LUNA_CORE_API RV get_system_info(ESystemInfo system_info, void* data);

	// ---------------------------------------------------------------------------------------------------------
	//		MEMORY MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------
	//! Gets the global memory heap. The global heap is always presents until the engine shutdown.
	LUNA_CORE_API IHeap* get_global_heap();

	//! Creates a new heap.
	//! @param[in] heap_creation_flags A combination of EHeapCreationFlag enum.
	//! @param[in] initial_size The init committed page for the new heap, specify 0 to commit one page.
	//! @param[in] maximum_size The maximum size this heap can commit, specify 0 to create a unlimited heap that
	//! can expend address space range automatically when the memory is not enough. 
	//! The maximum size of the blocks the heap can allocate is restricted to slightly less than 512KB for 32-bit 
	//! process and slightly less than 1024KB for 64-bit process on Windows, set this to 0 if you want to allocate large
	//! blocks.
	//! @param[in] name The name of the heap. This name is only used for debugging purpose, and can be set to `nullptr` if
	//! you don't want this.
	//! @return The created heap object.
	LUNA_CORE_API RP<IHeap> new_heap(EHeapCreationFlag heap_creation_flags = EHeapCreationFlag::none, size_t initial_size = 0, size_t maximum_size = 0, const char* name = nullptr);

	//! Reserves a region of pages in the virtual address space of the calling process.
	//! @param[in] address The starting address of the region to reserve. If this parameter is NULL, the system 
	//! determines where to reserve the region.
	//! @param[in] size The size to reserve in bytes, if the size is not times of the page size, it will be rounded to times of page size.
	LUNA_CORE_API void* virtual_reserve(void* address, size_t size);

	//! Commit the page in the reserved space to physical memory to enable them for accessing.
	//! @param[in] address The starting address of the region to commit. The address must be reserved by calling `virtual_reserve` first.
	//! @param[in] size The size to reserve in bytes, if the size is not times of the page size, it will be rounded to times of page size.
	//! @param[in] protection The protection settings for the committed page.
	LUNA_CORE_API void* virtual_commit(void* address, size_t size, EPageProtection protection = EPageProtection::read_write);

	//! Decommit the memory range committed to the physical memory and release all pages in the range.
	//! @param[in] address The starting address of the region to decommit.
	//! @param[in] size The size to decommit in bytes, if the size is not times of the page size, it will be rounded to times of page size.
	LUNA_CORE_API RV virtual_decommit(void* address, size_t size);

	//! Release the region of pages reserved by calling `virtual_reserve`.
	//! All committed pages in this range will be decommitted.
	//! @param[in] address The memory address returned by `virtual_reserve`.
	LUNA_CORE_API RV virtual_release(void* address);

	//! Changes the protection on a region of committed pages in the virtual address space of the calling process.
	//! @param[in] address The starting address of the region to change protection.
	//! @param[in] size The size of the range.
	//! @param[in] new_protection The new protection to set.
	//! @param[in] old_protection A variable to retrieve the old protection. Set to `nullptr` if you do not care the old protection value.
	LUNA_CORE_API RV virtual_protect(void* address, size_t size, EPageProtection new_protection, EPageProtection* old_protection = nullptr);

	// ---------------------------------------------------------------------------------------------------------
	//		RANDOM NUMBER GENERATION
	// ---------------------------------------------------------------------------------------------------------

	//! Creates a new random number generator.
	LUNA_CORE_API P<IRandom> new_random(uint32 initial_seed);

	LUNA_CORE_API uint32 random_u32();
	LUNA_CORE_API int32 random_i32();
	LUNA_CORE_API uint64 random_u64();
	LUNA_CORE_API int64 random_i64();
	LUNA_CORE_API float32 random_f32(float32 range_begin, float32 range_end);
	LUNA_CORE_API float64 random_f64(float64 range_begin, float64 range_end);
	LUNA_CORE_API Guid random_guid();

	// ---------------------------------------------------------------------------------------------------------
	//		SHARED LIBRARY LOADING
	// ---------------------------------------------------------------------------------------------------------

	//! Loads one dynamic-link library file into the process space.
	LUNA_CORE_API R<handle_t> load_library(const char* filename);
	//! Frees one loaded dynamic-link library file.
	LUNA_CORE_API RV free_library(handle_t handle);
	//! Gets the exposed procedure address of the specified symbol in the library.
	LUNA_CORE_API void* get_proc_address(handle_t handle, const char* symbol_name);

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
	LUNA_CORE_API P<ISemaphore> new_semaphore(int32 initial_count, int32 max_count);
	//! Creates a new read write lock object.
	LUNA_CORE_API P<IReadWriteLock> new_read_write_lock();

	// ---------------------------------------------------------------------------------------------------------
	//		THREAD AND FIBER MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------

	//! Create a new system thread and make it run the callback function. The thread will be closed when the callback function returns.
	//! @param[in] callback The callback object to invoke.
	//! @param[in] params The parameter object passed to callback function Invoke method as parameter. The system keeps a reference to the parameter
	//! object until the thread is closed.
	//! @param[in] stackSize The stack size for new thread's call stack.
	//! @return Returns an interface representing the new created thread, or `nullptr` if failed. 
	LUNA_CORE_API RP<IThread> new_thread(IRunnable* callback, uint32 stack_size = 0);

	//! Gets the thread object of current running thread.
	//! @return The thread object of the current thread.
	LUNA_CORE_API IThread* get_current_thread();

	//! Gets the thread object of the main thread.
	//! @return The thread object of the main thread.
	LUNA_CORE_API IThread* get_main_thread();

	//! Suspends current thread for a specific period of time. The actual suspended time may be longer than required.
	//! @param[in] timeMillisecounds The time, in milliseconds, that this thread needs to suspend.
	LUNA_CORE_API void sleep(uint32 time_milliseconds);
	//! Delay the execution of this thread for a very shout time by yielding this thread several times.
	//! This is more accurate to `sleep` method and will not suspend current thread unless the specified time is larger than several milliseconds.
	//! @param[in] time_microseconds The time, in microseconds, that this thread needs to delay.
	LUNA_CORE_API void fast_sleep(uint32 time_microseconds);
	//! Yields the remain time slice of the current thread and let OS to schedule other threads.
	//! There is no way to resume a thread from user mode, since threads are scheduled by OS automatically.
	LUNA_CORE_API void yield_current_thread();

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
	LUNA_CORE_API RP<IFiber> new_fiber(IRunnable* callback, uint32 stack_size = 0);

	//! Gets the the fiber object of current running fiber.
	//! @return Returns the fiber object of the current thread if a fiber is running, 
	//! or `nullptr` if no fiber is running on this thread (the main thread entry function is running, which is not a fiber).
	LUNA_CORE_API IFiber* get_current_fiber();

	//! Resumes the target fiber.
	LUNA_CORE_API RV resume_fiber(IFiber* fiber);

	//! Yields the current fiber.
	LUNA_CORE_API RV yield_current_fiber();

	//! Allocate a thread local storage (TLS) slot to every thread running on this process. After the handle is returned, every thread
	//! can set a thread-local value to the slot.
	//! @param[out] handle The handle to the TLS slot.
	//! @return s_ok if succeed, e_failure otherwise.
	LUNA_CORE_API RV tls_alloc(handle_t* handle);

	//! Free the TLS slot allocated by `tls_alloc`. The handle will be invalid after this call for every thread.
	//! @param[in] handle The handle returned by `tls_alloc`.
	//! @return s_ok if succeed, e_failure otherwise.
	LUNA_CORE_API RV tls_free(handle_t handle);

	//! Set the data bound to the current thread's TLS slot specified by `handle`.
	//! @param[in] handle The handle of the slot specified.
	//! @param[in] ptr The pointer value to set to this slot.
	//! @return s_ok if succeed, e_failure otherwise.
	LUNA_CORE_API RV tls_set(handle_t handle, void* ptr);

	//! Get the value bound to the TLS slot of current thread.
	//! @param[in] handle The handle of the slot to query.
	//! @param[out] ptr The pointer to retrieve the data.
	//! @return s_ok if succeed, e_failure otherwise.
	LUNA_CORE_API RV tls_get(handle_t handle, void** ptr);

	// ---------------------------------------------------------------------------------------------------------
	//		TIME MEASUREMENT
	// ---------------------------------------------------------------------------------------------------------

	//! Query the ticks of the high-performance counter of CPU.
	//! @return Returns the current ticks of the CPU.
	LUNA_CORE_API uint64 get_ticks();

	//! Query the resolution of high-performance counter of CPU represented by
	//! number of ticks per second.
	//! @return Returns the number of ticks per second.
	LUNA_CORE_API uint64 get_ticks_per_second();

	//! Get the number of ticks elapsed since the engine starts.
	//! @return The number of ticks elapsed since the system initialize.
	LUNA_CORE_API uint64 get_elapsed_ticks();

	//! Get current time in local time zone.
	//! The returned time is in UNIX time stamp format (number of milliseconds from Jan 1st, 1970).
	LUNA_CORE_API uint64 get_local_timestamp();

	//! Get current time in UTC time zone.
	//! The returned time is in UNIX time stamp format (number of milliseconds from Jan 1st, 1970).
	LUNA_CORE_API uint64 get_utc_timestamp();

	//! Converts a timestamp to a date time.
	LUNA_CORE_API void timestamp_to_data_time(uint64 timestamp, DateTime& datetime);

	//! Converts a date time to a timestamp.
	LUNA_CORE_API uint64 date_time_to_timestamp(const DateTime& datetime);

	// ---------------------------------------------------------------------------------------------------------
	//		STRING LIBRARY
	// ---------------------------------------------------------------------------------------------------------

	//! Creates or gets a name object.
	//! @param[in] name The name string to intern. If the name object exists, this call increments the reference
	//! count and returns the name object directly; if the name object does not exist, this call creates a new name
	//! object and returns the new name object.
	//! @return The name object.
	LUNA_CORE_API P<IName> intern_name(const char* name, size_t len = size_t_max_v);

	//! Checks if the name object exists in the name system.
	//! @param[in] name The name string to check. 
	//! @return The name object identified by name string exists, this call increments the reference
	//! count and returns the name object. If the name object does not exist, this call returns `nullptr` without 
	//! creating a new one.
	LUNA_CORE_API RP<IName> check_name(const char* name, size_t len = size_t_max_v);

	//! Creates a new string buffer.
	LUNA_CORE_API P<IStringBuffer> new_string_buffer();

	//!	Creates an immutable string.
	LUNA_CORE_API P<IString> new_string(const char* data, size_t len = size_t_max_v);

	//! Creates an immutable blob object.
	LUNA_CORE_API P<IBlob> new_blob(const void* data, size_t len, IAllocator* alloc = nullptr);

	//! Creates a new buffer.
	LUNA_CORE_API P<IBuffer> new_buffer(size_t buffer_size, size_t alignment = 0, IAllocator* alloc = nullptr);

	//! Creates a new buffer object and using the buffer passed in as its backed memory rather than allocating a new one.
	//! The `buffer` must be a memory buffer allocated by `alloc`, and the `alloc` will be used to free or reallocate
	//! new buffers.
	LUNA_CORE_API P<IBuffer> box_buffer(void* buffer, size_t buffer_size, IAllocator* alloc, size_t alignment = 0);

	//! Creates a new memory stream.
	LUNA_CORE_API P<IMemoryStream> new_memory_stream(size_t initial_buffer_size = 0, size_t alignment = 0, IAllocator* alloc = nullptr);

	//! Creates a new memory stream by attaching a buffer object.
	LUNA_CORE_API P<IMemoryStream> new_memory_stream_from_buffer(IBuffer* buffer);

	//! Creates a new path object.
	LUNA_CORE_API P<IMutPath> new_path(const char* path_string = nullptr, size_t len = size_t_max_v);

	// ---------------------------------------------------------------------------------------------------------
	//		LOG SYSTEM
	// ---------------------------------------------------------------------------------------------------------

	//! Creates a new logger.
	//! @param[in] channel The channel that the logger will log the message to.
	LUNA_CORE_API P<ILogger> new_logger(IName* channel);

	//! Creates a new log handler. 
	//! The system holds one weak reference to the handler, and the handler holds one weak reference to the 
	//! callback as well. So the user should keep a strong reference to both handler object and delegate object
	//! to make it work.
	LUNA_CORE_API P<ILogHandler> new_log_handler(IName* name, ILogCallback* callback);

	//! Gets the log handler with its name.
	LUNA_CORE_API RP<ILogHandler> get_log_handler(IName* name);

	// ---------------------------------------------------------------------------------------------------------
	//		UPDATE LOOP MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------

	//! Indicates the system that a new frame is started. This should be called once 
	//! per frame.
	//! This must be called by the main thread.
	LUNA_CORE_API void new_frame();

	//! Get the current frame index. Every time `ISystem::update` is called, the frame index will be increased by 1.
	//! The engine starts the first frame immediately after initialized, so the first frame is 1 before the first call to `ISystem::update`.
	LUNA_CORE_API uint32 get_frame_index();

	//! Get the delta time of last frame, that is, the elapsed time between the last 2 `ISystem::update` call.
	//! The time is clamped by min govern time and max govern time.
	LUNA_CORE_API uint64 get_last_frame_ticks();

	//! Get the actual delta time of last frame, that is, the frame time before modified by frame governing feature 
	//! and max frame time clamping feature.
	//! This frame time is for debug use, not for updating the system.
	LUNA_CORE_API uint64 get_last_frame_ticks_raw();

	//! Get the frame ticks of the specified frames. Luna Engine records the most recent 256 frame ticks for profiling use
	//! in every debug level build.
	//! @param[in] frame_index The index of frame to check. 0 means the latest frame (the last frame) and 255 means the 
	//! furthest frame.
	LUNA_CORE_API uint64 get_last_n_frame_ticks(uint32 frame_index);

	//! Get the actual frame ticks before modified by frame governing feature and max frame time clamping feature.
	//! This frame time is for debug use, not for updating the system.
	//! @param[in] frame_index The index of frame to check. 0 means the latest frame (the last frame) and 255 means the 
	//! furthest frame.
	LUNA_CORE_API uint64 get_last_n_frame_ticks_raw(uint32 frame_index);

	//! Get the frames per second value judged from last frame ( 1.0f / get_last_frame_seconds ).
	LUNA_CORE_API float32 get_fps();

	//! Get the frames per second value judged from last frame's raw ticks ( 1.0f / (get_last_frame_raw_ticks() / get_ticks_per_second()))
	LUNA_CORE_API float32 get_fps_raw();

	//! Get the frames per second value averaged from last 120 frames.
	LUNA_CORE_API float32 get_fps_averaged();

	//! Get the frames per second value averaged from last 120 frame's raw data.
	LUNA_CORE_API float32 get_fps_averaged_raw();

	//! Get the elapsed ticks from last call to `ISystem::update`.
	LUNA_CORE_API uint64 get_frame_elapsed_ticks();

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
	LUNA_CORE_API RV set_frame_governing_fps(float32 frames_per_second);

	//! Get the govern value used by frame governing features.
	LUNA_CORE_API float32 get_frame_governing_fps();

	//! Set min frame fps clamping enabled/disabled. This feature is disabled by default.
	//! Min frame fps clamping will clamp the frame time to a specified value if the frame time is larger than
	//! the specified value. This is necessary in order to keep some time-related system (like physical simulation)
	//! stable and prevent a large frame time peak ruins the system when the system runs too slow or paused by debug break.
	LUNA_CORE_API void set_min_frame_fps_clamping_enabled(bool enabled);

	//! Check if the max frame time clamping feature is enabled.
	LUNA_CORE_API bool is_min_frame_fps_clamping_enabled();

	//! Set the value used to clamping max frame time. Default value is 10.0f (at least 10 frames per second).
	LUNA_CORE_API void set_min_frame_fps(float32 frames_per_second);

	//! Get the value used to clamping max frame time.
	LUNA_CORE_API float32 get_min_frame_fps();

	// ---------------------------------------------------------------------------------------------------------
	//		DATA SYSTEM
	// ---------------------------------------------------------------------------------------------------------

	//! Creates a new variant.
	LUNA_CORE_API P<IVariant> new_var(EVariantType t);
	LUNA_CORE_API P<IVariant> new_var1(EVariantType t, size_t d1);
	LUNA_CORE_API P<IVariant> new_var2(EVariantType t, size_t d1, size_t d2);
	LUNA_CORE_API P<IVariant> new_var3(EVariantType t, size_t d1, size_t d2, size_t d3);
	LUNA_CORE_API P<IVariant> new_var4(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4);
	LUNA_CORE_API P<IVariant> new_var5(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5);
	LUNA_CORE_API P<IVariant> new_var6(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6);
	LUNA_CORE_API P<IVariant> new_var7(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7);
	LUNA_CORE_API P<IVariant> new_var8(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7, size_t d8);

	LUNA_CORE_API P<IEncoder> new_text_encoder();
	LUNA_CORE_API P<IDecoder> new_text_decoder();

	// ---------------------------------------------------------------------------------------------------------
	//		VIRTUAL FILE SYSTEM
	// ---------------------------------------------------------------------------------------------------------

	//! Opens one file from the LUNA_CORE_API file system.
	//! @param[in] filename The path of the file relative to the root path of the file system.
	//! @param[in] flags The file open flags.
	//! @param[in] creation Specify whether to create a file if the file does not exist.
	//! @return If succeeded, returns the new opened file object.
	LUNA_CORE_API RP<IFile>	open_file(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation);

	//! Opens one file in asynchronous mode from the virtual file system. That is, read and write calls to the file object will be asynchronous.
	//! @param[in] filename The path of the file.
	//! @param[in] flags The file open flags.
	//! @param[in] creation Specify whether to create a file if the file does not exist.
	//! @return If succeeded, returns the new opened file object.
	LUNA_CORE_API RP<IAsyncFile> open_file_async(IPath* filename, EFileOpenFlag flags, EFileCreationMode creatione);

	//! Gets the file attribute from the LUNA_CORE_API file system.
	LUNA_CORE_API RV file_attribute(IPath* filename, FileAttribute& attribute);

	//! Checks if a file or directory exists in the virtual file system.
	//! @return s_ok If file exists, e_file_not_exist if the file does not exist, any other error value if an error occurred
	//! when trying to file file.
	LUNA_CORE_API RV file_exists(IPath* filename);

	//! Checks if a directory exists in the virtual file system.
	//! @return s_ok If directory exists, e_dir_not_exist if the directory does not exist, any other error value if an error occurred
	//! when trying to find directory.
	LUNA_CORE_API RV dir_exists(IPath* dir_name);

	//! Gets the size of the file from the virtual file system.
	//! @param[in] filename The path of the file.
	//! @return Returns the size of the file, or 0 if failed to read the size of the file.
	LUNA_CORE_API R<uint64>	file_size(IPath* filename);

	//! Copies the file from the source path to the destination path in the LUNA_CORE_API file system.
	//! If the source file system and the destination file system is not the same, the file will always be copied
	//! by loading all of its content into the temp buffer and writes them to the destination file system.
	//! @param[in] from_filename Source file path.
	//! @param[in] to_filename Destination file path.
	//! @param[in] fail_if_exists If this is `true` and the file named `to_filename` already exists,
	//! this call fails with return value `e_item_already_exists`. If this is `false` and the file named `to_filename` 
	//! already exists, the file named `to_filename` will be overwritten.
	LUNA_CORE_API RV	copy_file(IPath* from_filename, IPath* to_filename, bool fail_if_exists = false);

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
	LUNA_CORE_API RV	move_file(IPath* from_filename, IPath* to_filename, bool allow_copy = true, bool fail_if_exists = false);

	//! Deletes the specified file in the virtual file system.
	//! @param[in] filename The file to delete.
	LUNA_CORE_API RV	delete_file(IPath* filename);

	//! Creates a file iterator that can be used to iterate all files in the specified directory in the virtual file system.
	//! @param[in] dir_path The directory path to open.
	//! @return Returns a file iterator object if succeeded.
	LUNA_CORE_API RP<IFileIterator> open_dir(IPath* dir_path);

	//! Creates one directory in the virtual file system.
	//! @param[in] pathname The path of the directory.
	LUNA_CORE_API RV	create_dir(IPath* pathname);

	//! Removes one directory in the virtual file system.
	//! @param[in] pathname The path of the directory.
	//! @param[in] recursive If `true`, this call removes the parent folder as well as all contents in the folder. If `false`, 
	//! only the parent folder will be removed, and it must be empty or the call will fail.
	LUNA_CORE_API RV	remove_dir(IPath* pathname, bool recursive = false);

	//! Mounts one file system to the virtual file system. The virtual file system keeps a strong reference to the file system
	//! interface.
	//! 
	//! Note that the later mounted file system will covers the former mounted one, so if one mount point is a sub-path of 
	//! another mount point, be sure to mount the parent file system and then mount the child file system.
	LUNA_CORE_API RV mount_fs(IPath* mount_point, IFileSystem* fs);

	//! Mounts one virtual system's path to another virtual system's path.
	LUNA_CORE_API RV mount_virtual_path(IPath* mount_point, IPath* vfs_path);

	//! Mounts a platform's native path to a virtual system's path.
	LUNA_CORE_API RV mount_platfrom_path(IPath* mount_point, IPath* platform_path);

	//! Unmounts one file system at the specified mount point.
	LUNA_CORE_API RV unmount_fs(IPath* mount_point);

	//! Gets a string array that describes all mount points sorted by their search order.
	LUNA_CORE_API Vector<P<IString>> mount_points();

	//! Gets the file system interface mounted on the specified mount point.
	LUNA_CORE_API RP<IFileSystem> get_fs(IPath* mount_point);

	// ---------------------------------------------------------------------------------------------------------
	//		ERROR OBJECT
	// ---------------------------------------------------------------------------------------------------------

	//! Clears the error object of the current thread.
	LUNA_CORE_API void clear_err();

	//! Sets the error object for the current thread.
	LUNA_CORE_API void set_err(result_t result_code, const char* msgfmt, ...);
	LUNA_CORE_API void set_verr(result_t result_code, const char* msgfmt, va_list args);
	LUNA_CORE_API void set_err_user(const Guid& result_domain, result_t result_code, const char* msgfmt, ...);
	LUNA_CORE_API void set_verr_user(const Guid& result_domain, result_t result_code, const char* msgfmt, va_list args);

	//! Sets the error object with an additional variant table to store the error information for the current thread.
	LUNA_CORE_API void set_err_var(result_t result_code, IVariant* variant, const char* msgfmt, ...);
	LUNA_CORE_API void set_verr_var(result_t result_code, IVariant* variant, const char* msgfmt, va_list args);
	LUNA_CORE_API void set_err_var_user(const Guid& result_domain, result_t result_code, IVariant* variant, const char* msgfmt, ...);
	LUNA_CORE_API void set_verr_var_user(const Guid& result_domain, result_t result_code, IVariant* variant, const char* msgfmt, va_list args);

	//! Sets the error object of this thread.
	LUNA_CORE_API void set_err(IError* err_obj);

	//! Gets the error message. This always points to a valid buffer, if no error is recorded, returns "".
	LUNA_CORE_API const char* err_msg();

	//! Gets the error domain. If no error is recorded, returns {0, 0}.
	LUNA_CORE_API const Guid& err_domain();

	//! Gets the error result code. This is set to `s_ok` if no error is recorded.
	LUNA_CORE_API result_t err_result();

	//! Gets the error variant object. This is set to `nullptr` if no error variant is specified.
	LUNA_CORE_API IVariant* err_var();

	//! Creates one uninitialized error object.
	LUNA_CORE_API P<IError> new_err();

	//! Creates an error object that holds the error object of the current thread.
	LUNA_CORE_API P<IError> promote_err();

	//! Returns a user-friendly message string that explains the result code. For system result code (-32768~32767), the returned 
	//! string will always be valid (static string). For user result code (s_user_success or e_user_failure), the returned string
	//! is the same as the string returned by `err_msg` and will be valid when the current thread is not died.
	LUNA_CORE_API const char* explain(result_t result_code);

	//! Creates an error object that holds the error information from the result code.
	//! If result code is not `e_user_failure` or `s_user_success`, the error object contains
	//! the result code and the explained string; else, this call is the same as `promote_err`.
	LUNA_CORE_API P<IError> err_from_code(result_t result_code);

	// ---------------------------------------------------------------------------------------------------------
	//		DISPATCHING SYSTEM
	// ---------------------------------------------------------------------------------------------------------

	//! Creates a new dispatch queue.
	//! @param[in] thread_limit Determines how many tasks in the queue can be executed at the same time by 
	//! worker threads. Two special values are 0 and 1: value 0 means no limit, value 1 means the next
	//! task in the queue will not be executed until the last task has been finished, such queue is called 
	//! a serialized queue, because tasks in the queue are executed one by one.
	LUNA_CORE_API P<IDispatchQueue> new_dispatch_queue(uint32 concurrency_limit = 0);

	//---------------------------- Auxiliary APIs ----------------------------
	//	APIs that provides convenient functions to use the Core APIs.

	// Implement this function to enable the use of module-scoped global memory allocation.
	inline IAllocator* get_module_allocator();

	inline P<IString> base64_encode_buffered(const void* src, size_t src_size_bytes, IAllocator* alloc = nullptr)
	{
		luassert(src && src_size_bytes);
		if (!alloc)
		{
			alloc = get_module_allocator();
		}
		size_t len = base64_get_encoded_size(src_size_bytes);
		auto new_str = new_string_buffer();
		new_str->resize(len);
		base64_encode(new_str->data(), new_str->size(), src, src_size_bytes);
		return new_str;
	}
	inline P<IBlob> base64_decode_buffered(const char* src, size_t src_size_chars = size_t_max_v, IAllocator* alloc = nullptr)
	{
		luassert(src && src_size_chars);
		if (!alloc)
		{
			alloc = get_module_allocator();
		}
		if (src_size_chars == size_t_max_v)
		{
			src_size_chars = strlen(src);
		}
		size_t dest_size = base64_get_decoded_size(src_size_chars);
		auto buf = new_buffer(dest_size, 0, alloc);
		base64_decode(buf->buffer(), buf->buffer_size(), src, src_size_chars);
		buf->set_data_region(0, dest_size);
		return buf;
	}
	inline RP<IString> utf16_to_utf8_buffered(const char16_t* src, size_t src_size_chars = size_t_max_v, IAllocator* alloc = nullptr)
	{
		luassert(src && src_size_chars);
		if (!alloc)
		{
			alloc = get_module_allocator();
		}
		size_t str_sz = utf16_to_utf8_len(src, src_size_chars);
		auto buf = new_string_buffer();
		buf->resize(str_sz);
		utf16_to_utf8(buf->data(), buf->size(), src, src_size_chars);
		return buf;
	}
	inline P<IBlob> utf8_to_utf16_buffered(const char* src, size_t src_size_chars = size_t_max_v, bool le = true, IAllocator* alloc = nullptr)
	{
		luassert(src && src_size_chars);
		if (!alloc)
		{
			alloc = get_module_allocator();
		}
		size_t str_sz = utf8_to_utf16_len(src, src_size_chars);
		auto buf = new_buffer((str_sz + 1) * sizeof(char16_t));
		utf8_to_utf16((char16_t*)buf->data(), buf->size(), src, le, src_size_chars);
		return buf;
	}

	inline void* memalloc(size_t sz)
	{
		return get_module_allocator()->allocate(sz);
	}
	inline void memfree(void* ptr)
	{
		get_module_allocator()->free(ptr);
	}
	inline void* memrealloc(void* old_ptr, size_t new_size)
	{
		return get_module_allocator()->reallocate(old_ptr, new_size);
	}
	inline size_t memsize(void* ptr)
	{
		return get_module_allocator()->size(ptr);
	}
	inline void* memalloc_aligned(size_t sz, size_t alignment)
	{
		return get_module_allocator()->allocate_aligned(sz, alignment);
	}
	inline void memfree_aligned(void* ptr, size_t alignment)
	{
		get_module_allocator()->free_aligned(ptr, alignment);
	}
	inline void* memrealloc_aligned(void* old_ptr, size_t old_alignment, size_t new_size, size_t new_alignment)
	{
		return get_module_allocator()->reallocate_aligned(old_ptr, old_alignment, new_size, new_alignment);
	}
	inline size_t memsize_aligned(void* ptr, size_t alignment)
	{
		return get_module_allocator()->size_aligned(ptr, alignment);
	}
	template <typename _Ty, typename... _Args>
	inline _Ty* new_obj(_Args&&... args)
	{
		static_assert(alignof(_Ty) <= max_align_v, "Use new_obj_aligned instead!");
		_Ty* o = reinterpret_cast<_Ty*>(memalloc(sizeof(_Ty)));
		if (o)
		{
			new (o) _Ty(forward<_Args>(args)...);
			return o;
		}
		return nullptr;
	}
	template <typename _Ty>
	inline void delete_obj(_Ty* o)
	{
		static_assert(alignof(_Ty) <= max_align_v, "Use deleteobj_aligned instead!");
		o->~_Ty();
		memfree(o);
	}
	template <typename _Ty, typename... _Args>
	_Ty* new_obj_aligned(_Args&&... args)
	{
		static_assert(alignof(_Ty) > max_align_v, "Use new_obj instead!");
		_Ty* o = reinterpret_cast<_Ty*>(memalloc_aligned(sizeof(_Ty), alignof(_Ty)));
		if (o)
		{
			new (o) _Ty(forward<_Args>(args)...);
			return o;
		}
		return nullptr;
	}
	template <typename _Ty>
	void deleteobj_aligned(_Ty* o)
	{
		static_assert(alignof(_Ty) > max_align_v, "Use deleteobj instead!");
		o->~_Ty();
		memfree_aligned(o, alignof(_Ty));
	}

	//! Implement this function to enable module scope global logging.
	inline ILogger* get_logger();
	
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
	}
}