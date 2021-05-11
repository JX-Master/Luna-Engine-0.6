// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file OS.hpp
* @author JXMaster
* @date 2020/8/20
* @brief This file contains the Operation System Independent Layer (or simply OS layer), 
* which is an os-agnostic programming interface between the underlying platform/os and the Luna Runtime. 
* For every platform/os that wants to run Luna Runtime, all functions in the interface layer should be implemented.
* 
* All functions in this layer are very low-level and are designed to be called by the Luna Runtime internally, 
* they are NOT designed to be called by the business code(like the game code). These functions are mapped directly 
* to the system so that they are not managed by the Luna Runtime, and for most of the time, the Luna Runtime provides 
* more advanced alternatives to be used by the business code. Also, all functions in this layer cannot be called by script, 
* only the alternatives provided by the Luna Runtime are exported to the scripting layer.
*/
#pragma once
#include "../Result.hpp"
#include "../Time.hpp"
namespace Luna
{
	namespace OS
	{
		//! Initializaes the os/platform layer. This is called by the Luna Runtime when Luna::init is called.
		void init();

		//! Closes the os/platform layer. This is called by the Luna Runtime when Luna::close is called.
		void close();

		//! Fetches the number of logical processors on the platform.
		u32 get_num_processors();

		//! Reports an assertion failure information to the underlying OS or CRT.
		//! This function works in all builds, and can be called even if the runtime is not initialized.
		//! The behavior of this function depends on the OS/CRT implementation, but in general it will 
		//! present an error message box and then terminate the program.
		//! @param[in] msg The UTF-8 error message to show.
		//! @param[in] file The UTF-8 name of the file that causes the panic.
		//! @param[in] line The code line the assertion is placed.
		void assert_fail(const c8* msg, const c8* file, u32 line);

		//! Triggers a debug break, pauses the program and attaches the debugger to the program.
		//! This only works in debug build. In release build, this function returns directly.
		void debug_break();

		enum class EPageProtection : u32
		{
			no_access = 1,
			read = 2,
			read_write = 3,
			execute = 4,
			execute_read = 5,
			execute_read_write = 6,
		};

		//! Prints a message to the debug console attached to this process.
		void debug_printf(const c8* fmt, ...);

		//! Prints a message to the debug console attached to this process with `VarList` as arguments.
		void debug_vprintf(const c8* fmt, VarList args);

		//! Reserves a region of pages in the virtual address space of the calling process.
		//! @param[in] address The starting address of the region to reserve. If this parameter is NULL, the system 
		//! determines where to reserve the region.
		//! @param[in] size The size to reserve in bytes, if the size is not times of the page size, it will be rounded to times of page size.
		//! @return Returns a pointer to the first address of the reserved address space, or error code if this operation fails.
		R<void*> virtual_reserve(void* address, usize size);

		//! Commit the page in the reserved space to physical memory to enable them for accessing.
		//! @param[in] address The starting address of the region to commit. The address must be reserved by calling `virtual_reserve` first.
		//! @param[in] size The size to reserve in bytes, if the size is not times of the page size, it will be rounded to times of page size.
		//! @param[in] protection The protection settings for the committed page.
		//! @return Returns a pointer to the first address of the committed address space, or error code if this operation fails.
		RV virtual_commit(void* address, usize size, EPageProtection protection = EPageProtection::read_write);

		//! Decommit the memory range committed to the physical memory and release all pages in the range.
		//! @param[in] address The starting address of the region to decommit.
		//! @param[in] size The size to decommit in bytes, if the size is not times of the page size, it will be rounded to times of page size.
		//! @return Returns `s_ok` if this operation succeeds, or error code if this operation fails.
		RV virtual_decommit(void* address, usize size);

		//! Release the region of pages reserved by calling `virtual_reserve`.
		//! All committed pages in this range will be decommitted.
		//! @param[in] address The memory address returned by `virtual_reserve`.
		//! @param[in] size The size specified when calling `virtual_reserve`.
		//! @return Returns `s_ok` if this operation succeeds, or error code if this operation fails.
		RV virtual_release(void* address, usize size);

		//! Changes the protection on a region of committed pages in the virtual address space of the calling process.
		//! @param[in] address The starting address of the region to change protection.
		//! @param[in] size The size of the range.
		//! @param[in] protection The new protection to set.
		//! @return Returns `s_ok` if this operation succeeds, or error code if this operation fails.
		RV virtual_protect(void* address, usize size, EPageProtection protection);

		//! Allocates memory blocks from system heap.
		//! @param[in] size The number of bytes to allocate. If this is 0, no memory will be allocated and the return value will be `nullptr`.
		//! @param[in] alignment Optional. The required alignment of the allocated memory block. 
		//! 
		//! If this is 0 (default), then the memory is allocated with no additional alignment requirement. In such case, the memory address is 
		//! aligned to times of 8 in 32-bit platform, and times of 16 in 64-bit platform. 
		//! 
		//! If this is not 0, then the memory is allocated with the specified alignment requirement satisfied. The alignment value must be powers
		//! of 2 (32, 64, 128, 256, etc).
		//! 
		//! Note that you shall use the same alignment value for the same memory block in `allocate`, `free`, `reallocate` and `size` function.
		//! 
		//! @return Returns a pointer to the first available memory address allocated, or `nullptr` if failed to allocate one from this allocator.
		void* memalloc(usize size, usize alignment = 0);

		//! Frees memory blocks allocated by `OS::memalloc` or `OS::memrealloc`.
		//! @param[in] ptr The pointer returned by `OS::memalloc` or `OS::memrealloc` method. If this is `nullptr`, this function does nothing.
		//! @param[in] alignment Optional. The alignment requirement specified when allocating the memory block. Default is 0.
		void memfree(void* ptr, usize alignment = 0);

		//! Reallocates memory blocks. This function allocates a new memory block with the specified size and alignment
		//! requirement, copies the data from the old memory block to the new one, and frees the old memory block.
		//! 
		//! The old memory block shall be the block allocated from `OS::memalloc` or `OS::memrealloc`.
		//! 
		//! @param[in] ptr The pointer to the former allocated memory allocated by `OS::memalloc` or `OS::memrealloc`. This will be freed if the new  
		//! memory block is successfully allocated. If this parameter is `nullptr`, this method behaves the same as `OS::memalloc`.
		//! @param[in] size The new number of bytes to allocate.
		//! @param[in] alignment Optional. The alignment requirement for the old memory block. Default is 0. 
		//! 
		//! The new and old memory block use the same alignment requirement, which is decided when the old memory block is allocated.
		//! @return Returns a pointer to the reallocated memory block. If the allocation is failed, returns `nullptr` and
		//! the old memory block (if have) is not changed.
		void* memrealloc(void* ptr, usize size, usize alignment = 0);

		//! Gets the allocated size of the memory block allocated by `OS::memalloc` or `OS::memrealloc`. 
		//! The returned size is the size that is available for the user to use. 
		//! Note that the allocated size may be bigger than the size required to specify alignment and padding requirements.
		//! @param[in] ptr The pointer returned by `OS::memalloc` or `OS::memrealloc`.
		//! @param[in] alignment Optional. The alignment requirement specified for the memory block during allocation. Default is 0. 
		//! @return Returns the size of bytes of the memory block. If `ptr` is `nullptr`, the returned value is 0.
		usize memsize(void* ptr, usize alignment = 0);

		//! Global object creation function.
		template <typename _Ty, typename... _Args>
		_Ty* memnew(_Args&&... args)
		{
			_Ty* o = reinterpret_cast<_Ty*>(memalloc(sizeof(_Ty), alignof(_Ty)));
			if (o)
			{
				new (o) _Ty(forward<_Args>(args)...);
				return o;
			}
			return nullptr;
		}

		//! Global object deleting function.
		template <typename _Ty>
		void memdelete(_Ty* o)
		{
			o->~_Ty();
			memfree(o, alignof(_Ty));
		}

		//! Loads one dynamic-link library file into the process space.
		R<handle_t> load_library(const c8* filename);

		//! Frees one loaded dynamic-link library file.
		RV free_library(handle_t handle);

		//! Gets the exposed procedure address of the specified symbol in the library.
		R<void*> get_proc_address(handle_t handle, const c8* symbol_name);

		//! Creates one signal object.
		handle_t new_signal(bool manual_reset);

		//! Destroys one signal object.
		void delete_signal(handle_t sig);

		//! Waits for the signal to be triggered.
		void wait_signal(handle_t sig);

		//! Try to wait for the signal is triggered, returns immediately if the signal is not triggered.
		RV try_wait_signal(handle_t sig);

		//! Triggers the signal.
		void trigger_signal(handle_t sig);

		//! Resets the signal to non-triggered state. 
		//! This operation is valid only for the signal with `manual_reset` set to `true` when creating the signal.
		void reset_signal(handle_t sig);

		//! Creates one mutex object.
		handle_t new_mutex();

		//! Destroys one mutex object.
		void delete_mutex(handle_t mtx);

		//! Locks the mutex object.
		void lock_mutex(handle_t mtx);

		//! Tries to lock the mutex object, returns immediately if the mutex is already locked by another thread.
		RV try_lock_mutex(handle_t mtx);

		//! Releases the mutex object.
		void unlock_mutex(handle_t mtx);

		//! Creates one semaphore object.
		handle_t new_semaphore(i32 initial_count, i32 max_count);

		//! Destroys one semaphore object.
		void delete_semaphore(handle_t sema);

		//! Acquires one semaphore object.
		void acquire_semaphore(handle_t sema);

		//! Tries to acquire one semaphore object.
		RV try_acquire_semaphore(handle_t sema);

		//! Releases the semaphore acquired.
		void release_semaphore(handle_t sema);

		//! Queries the ticks of the high-performance counter of CPU.
		//! @return Returns the current ticks of the CPU.
		u64 get_ticks();

		//! Queries the resolution of high-performance counter of CPU represented by
		//! number of ticks per second.
		//! @return Returns the number of ticks per second.
		f64 get_ticks_per_second();

		//! Gets current timestamp.
		//! The returned time is in UNIX time stamp format (number of seconds from Jan 1st, 1970, UTC).
		i64 get_timestamp();

		//! Converts an UNIX timestamp to a date time represented in the current platform's time zone.
		DateTime timestamp_to_local_datetime(i64 timestamp);

		//! Converts an UNIX timestamp to a data time in UTC time zone.
		DateTime timestamp_to_utc_datetime(i64 timestamp);

		//! Converts a date time in the current time zone to an UNIX timestamp.
		i64 local_datetime_to_timestamp(const DateTime& datetime);

		//! Converts a date time in UTC time zone to an UNIX timestamp.
		i64 utc_datetime_to_timestamp(const DateTime& datetime);

		using thread_callback_func_t = void(void* params);

		//! Create a new system thread and make it run the callback function.
		//! @param[in] callback The callback function to be called by the new thread.
		//! @param[in] params The parameter object passed to callback function as parameter.
		//! @param[in] stack_size The stack size for new thread's call stack.
		//! @return Returns an interface representing the new created thread, or `nullptr` if failed.
		R<handle_t> new_thread(thread_callback_func_t* callback, void* params, usize stack_size);

		enum class ThreadPriority : i32
		{
			low = 0,
			normal = 1,
			high = 2,
			critical = 3
		};

		//! Sets the thread schedule priority.
		//! @param[in] thread The thread handle.
		//! @param[in] priority The priority to set.
		RV set_thread_priority(handle_t thread, ThreadPriority priority);

		//! Waits for the thread to finish.
		void wait_thread(handle_t thread);

		//! Try to wait for the thread to finish.
		RV try_wait_thread(handle_t thread);

		//! Closes the thread handle. 
		void detach_thread(handle_t thread);

		//! Gets the handle of the current thread.
		handle_t get_current_thread();

		//! Gets the thread ID of the current thread.
		u32 get_current_thread_id();

		//! Suspends current thread for a specific period of time. The actual suspended time may be longer than required.
		//! @param[in] time_milliseconds The time, in milliseconds, that this thread needs to suspend.
		void sleep(u32 time_milliseconds);

		//! Delay the execution of this thread for a very shout time by yielding this thread several times.
		//! This is more accurate to `sleep` method and will not suspend current thread unless the specified time is larger than several milliseconds.
		//! @param[in] time_microseconds The time, in microseconds, that this thread needs to delay.
		void fast_sleep(u32 time_microseconds);

		//! Yields the remain time slice of the current thread and let OS to schedule other threads.
		//! There is no way to resume a thread from user mode, since threads are scheduled by OS automatically.
		void yield_current_thread();

		//! Converts the current thread into a fiber. You must convert a thread into a fiber before you can schedule other fibers.
		//! Returns the fiber object used to switch back to the main thread.
		//! @return Returns the created fiber context for the current thread, or error code on failure.
		R<handle_t> convert_thread_to_fiber();

		//! Converts the current fiber to thread. This call destroys the fiber context for the specified thread.
		//! If you don't do this, the fiber context will be destroyed when the thread is destroyed.
		RV convert_fiber_to_thread();

		//! Create a new fiber context. The created fiber will be executed the first time you call `switch_to_fiber` on any of the threads.
		//! @param[in] callback The callback function to be called by the new fiber.
		//! @param[in] params The parameter object passed to callback function as parameter.
		//! @param[in] stack_size The stack size for new fiber's call stack.
		//! @return Returns the fiber context that can be used to switch to, or error code on failure.
		//! 
		//! The error code may be:
		//!	* e_bad_system_call
		R<handle_t> new_fiber(thread_callback_func_t* callback, void* params, usize stack_size);

		//! Destroys the specified fiber context. The fiber context must be created by `new_fiber`, for contexts created by `convert_thread_to_fiber`, 
		//! call `convert_fiber_to_thread` instead.
		void delete_fiber(handle_t fiber);

		//! Suspends the current fiber context and switches to the target fiber context. 
		//! @param[in] fiber The fiber to switch to.
		//! @remark This function returns when the original fiber is switched back. Make sure the thread's original fiber is executing 
		//! when the thread exits to avoid resource leaking.
		void switch_to_fiber(handle_t fiber);

		using tls_destructor = void(void*);

		//! Allocates one thread local storage (TLS) slot for every thread running in this process, including the thread that is currently not being 
		//! created yet. After the handle is returned, every thread can set a thread-local value to this slot using this handle.
		//! 
		//! The allocated slot is large enough to store one pointer to the real thread-local data. The pointer is `nullptr` for every thread before it 
		//! is firstly modified by that thread.
		//! @param[in] destructor The optional destructor to use for this TLS slot. If this is not `nullptr`, this destructor will be called on one thread
		//! when that thread exits and the TLS pointer value of this slot for that thread is not `nullptr`.
		//! 
		//! When the destructor is called, the system resets the TLS value to `nullptr` and passes the original TLS value to the destructor. This process may
		//! be repeated several times until the TLS value is `nullptr` after the destructor returns.
		//! 
		//! @return Returns the handle to the TLS slot, or error code if failed.
		R<handle_t> tls_alloc(tls_destructor* destructor = nullptr);

		//! Free the TLS slot allocated by `tls_alloc`. The handle will be invalid after this call and the pointer stored for every 
		//! thread will be discarded.
		//! 
		//! Make sure to free all resources bound to the specified slot manually before calling this, or they will never be freed.
		//! @param[in] handle The handle returned by `tls_alloc`.
		//! @remark Note that calling `tls_free` will not call the destructor registered for this slot on any thread. After `tls_free` is called, the 
		//! destructor will be cleared and will not be called any more.
		void tls_free(handle_t handle);

		//! Set the data bound to the current thread's TLS slot specified by `handle`.
		//! @param[in] handle The handle of the slot specified.
		//! @param[in] ptr The pointer value to set to this slot.
		RV tls_set(handle_t handle, void* ptr);

		//! Get the value bound to the TLS slot of current thread.
		//! @param[in] handle The handle of the slot to query.
		//! @return Returns the pointer set, or `nullptr` if no pointer is set to this slot.
		void* tls_get(handle_t handle);

		enum class FileOpenFlag : u32
		{
			none = 0x00,
			//! Grants read access to the file so that `read` operations can be performed.
			read = 0x01,
			//! Grants write access to the file so that `write` operations can be performed.
			write = 0x02,
		};

		enum class FileCreationMode : u32
		{
			//! Always creates a new file and opens it. If the file already exists, the old file content
			//! will be discarded and the file is treated as a new empty file.
			create_always = 1,
			//! Only creates a file and opens it when it does not exist. If the file already exists, the
			//! call fails with `e_file_already_exists`.
			create_new = 2,
			//! Always opens a file. If the file already exists, the file will be open with its data preserved,
			//! if the file does not exist, it will be created and opened.
			open_always = 3,
			//! Only opens a file when it does exists. If the file does not exist, the call fails with `e_file_not_exist`.
			open_existing = 4,
			//! Only opens a file when it does exists, and discards the file data so the file is treated as a new
			//! file.
			open_existing_as_new = 5
		};

		enum class EFileAttributeFlag : u32
		{
			none = 0x00,
			//! This file is read-only.
			read_only = 0x01,
			//! This file is hidden.
			hidden = 0x02,
			//! This file is a directory.
			directory = 0x04,
			//! This file is an character special file on UNIX/Linux systems.
			character_special = 0x08,
			//! This file is an block special file on UNIX/Linux systems.
			block_special = 0x10,
		};

		struct FileAttribute
		{
			//! The size of the file in bytes.
			u64 size;
			//! The creation time of the file in UNIX timestamp.
			//! This is set to 0 if the underlying file system does not report this (this is the case for Linux/UNIX systems, exclude macOS).
			u64 creation_time;
			//! The last access time of the file in UNIX timestamp.
			u64 last_access_time;
			//! The write time of the file in in UNIX timestamp.
			u64 last_write_time;
			//! The file attributes.
			EFileAttributeFlag attributes;
		};

		enum class SeekMode : u32
		{
			//! The offset is relative to the beginning of the stream.
			begin = 1,
			//! The offset is relative to the current position of the cursor.
			current = 2,
			//! The offset is relative to the end of the stream.
			end = 3
		};

		//! Opens unbuffered file.
		//! @param[in] path The path of the file.
		//! @param[in] flags The file open flags.
		//! @param[in] creation Specify whether to create a file if the file does not exist.
		//! @return Returns the new opened file handle if succeeds. Returns one error code if failed.
		//! Possible errors:
		//! * BasicError::bad_arguments
		//! * BasicError::access_denied
		//! * BasicError::not_found
		//! * BasicError::bad_system_call for all errors that cannot be identified.
		R<handle_t> open_file(const c8* path, FileOpenFlag flags, FileCreationMode creation);

		//! Closes one file opened by `open_buffered_file`.
		//! @param[in] file The file handle opened by `open_file`.
		void close_file(handle_t file);

		//! Reads data from the current position the cursor is pointing to and offsets the cursor back. If the data to be
		//! read is not ready, the platform suspends this thread until the data is ready.
		//! @param[in] file The file handle opened by `open_file`.
		//! @param[in] buffer The buffer used to store the read data. The buffer should be large enough to hold
		//! at least `size` bytes of data.
		//! @param[in] size The size of the data to read in bytes.
		//! @param[out] read_bytes If this is not `nullptr`, the system sets the actual size of bytes being read to the buffer
		//! to this parameter.
		//! The actual size of bytes being read may be smaller than the size of bytes required to be read if the cursor
		//! reaches the end of the stream, but this is NOT an error. Specially, if one read operation is performed when
		//! the cursor is beyond or at the end of the stream, the read operation succeeds with 0 bytes being read. This 
		//! can be considered as an EOF symbol in stdlib.
		//! @return Returns success on success, returns the following error code on failure:
		//! * BasicError::not_supported
		RV read_file(handle_t file, void* buffer, usize size, usize* read_bytes = nullptr);

		//! Writes data to the current position the cursor is pointing to and offsets the cursor back. This call returns after
		//! all data have been written.
		//! @param[in] file The file handle opened by `open_file`.
		//! @param[in] buffer The buffer that holds the data to be written.
		//! @param[in] size The size of the data to write in bytes.
		//! @param[out] write_bytes If not `nullptr`, the system sets the actual size of bytes being written to this parameter.
		//! Mostly, if the cursor goes beyond the end of the stream buffer while writing data, the stream will be expanded so
		//! the succeeding data can be written, so unless an error occurs, the size of bytes written will always equal to the 
		//! size of bytes required by the user to write. However, if an error occurs while writing data, some of the data may have 
		//! already be written while others are not, in such case the `write_bytes` reported by system may not be equal to `size` 
		//! specified by the user.
		RV write_file(handle_t file, const void* buffer, usize size, usize* write_bytes = nullptr);

		//! Gets the size of the file in bytes.
		//! @param[in] file The file handle opened by `open_file`.
		//! @return Returns the size of the file, or error code on failure.
		R<u64> get_file_size(handle_t file);

		//! Sets the size of the file in bytes.
		//! If the current file size is smaller than the size to set and this call succeeded, the stream will be extended to the size specified
		//! with data between the last size and current size be uninitialized. If the current file size is greater than the size to set and this 
		//! call succeeded, the stream will be truncated and the data between the last size and current size will be discarded.
		//! @param[in] file The file handle opened by `open_file`.
		//! @param[in] length The size to set, in bytes.
		RV set_file_size(handle_t file, u64 sz);

		//! Gets the current position of the stream cursor. The position is number of bytes relative to the beginning of the 
		//! stream.
		//! @param[in] file The file handle opened by `open_file`.
		R<u64> get_file_cursor(handle_t file);

		//! Moves the cursor to a new position.
		//! @param[in] file The file handle opened by `open_file`.
		//! @param[in] offset Number of bytes to move relative to the position specified by `mode`.
		//! @param[in] mode The seek mode to use.
		//! @remark The cursor position is not bound to the size of the stream. If you move the cursor beyond the end of the stream, the cursor
		//! is still valid with the following behaviors:
		//! 1. All read operations succeed with 0 byte being read, just like EOF behaviors.
		//! 2. All write operations extends the size of the stream before the actual write operation performed.
		//! 
		//! The cursor value cannot be negative, if the new cursor position goes below 0, the seek operation fails.
		RV set_file_cursor(handle_t file, i64 offset, SeekMode mode);

		//! Clears all buffers for this file and causes all buffered data to be written to the underlying device.
		//! @param[in] file The file handle opened by `open_file`.
		RV flush_file(handle_t file);

		//! Same as `open_file`, but opens the file with user-space buffering.
		//! @param[in] path The path of the file.
		//! @param[in] flags The file open flags.
		//! @param[in] creation Specify whether to create a file if the file does not exist.
		//! @return Returns the new opened file object if succeeds. Returns one error code if failed.
		//! Possible errors:
		//! * BasicError::bad_arguments
		//! * BasicError::access_denied
		//! * BasicError::not_found
		//! * BasicError::bad_system_call for all errors that cannot be identified.
		R<handle_t> open_buffered_file(const c8* path, FileOpenFlag flags, FileCreationMode creation);

		//! Closes one file opened by `open_buffered_file`.
		//! @param[in] file The file handle opened by `open_buffered_file`.
		void close_buffered_file(handle_t file);

		//! Reads data from the current position the cursor is pointing to and offsets the cursor back. If the data to be
		//! read is not ready, the platform suspends this thread until the data is ready.
		//! @param[in] file The file handle opened by `open_buffered_file`.
		//! @param[in] buffer The buffer used to store the read data. The buffer should be large enough to hold
		//! at least `size` bytes of data.
		//! @param[in] size The size of the data to read in bytes.
		//! @param[out] read_bytes If this is not `nullptr`, the system sets the actual size of bytes being read to the buffer
		//! to this parameter.
		//! The actual size of bytes being read may be smaller than the size of bytes required to be read if the cursor
		//! reaches the end of the stream, but this is NOT an error. Specially, if one read operation is performed when
		//! the cursor is beyond or at the end of the stream, the read operation succeeds with 0 bytes being read. This 
		//! can be considered as an EOF symbol in stdlib.
		//! @return Returns success on success, returns the following error code on failure:
		//! * e_not_supported
		RV read_buffered_file(handle_t file, void* buffer, usize size, usize* read_bytes = nullptr);

		//! Writes data to the current position the cursor is pointing to and offsets the cursor back. This call returns after
		//! all data have been written.
		//! @param[in] file The file handle opened by `open_buffered_file`.
		//! @param[in] buffer The buffer that holds the data to be written.
		//! @param[in] size The size of the data to write in bytes.
		//! @param[out] write_bytes If not `nullptr`, the system sets the actual size of bytes being written to this parameter.
		//! Mostly, if the cursor goes beyond the end of the stream buffer while writing data, the stream will be expanded so
		//! the succeeding data can be written, so unless an error occurs, the size of bytes written will always equal to the 
		//! size of bytes required by the user to write. However, if an error occurs while writing data, some of the data may have 
		//! already be written while others are not, in such case the `write_bytes` reported by system may not be equal to `size` 
		//! specified by the user.
		RV write_buffered_file(handle_t file, const void* buffer, usize size, usize* write_bytes = nullptr);

		//! Gets the size of the file in bytes.
		//! @param[in] file The file handle opened by `open_buffered_file`.
		//! @return Returns the size of the file, or error code on failure.
		R<u64> get_buffered_file_size(handle_t file);

		//! Sets the size of the file in bytes.
		//! If the current file size is smaller than the size to set and this call succeeded, the stream will be extended to the size specified
		//! with data between the last size and current size be uninitialized. If the current file size is greater than the size to set and this 
		//! call succeeded, the stream will be truncated and the data between the last size and current size will be discarded.
		//! @param[in] file The file handle opened by `open_buffered_file`.
		//! @param[in] length The size to set, in bytes.
		RV set_buffered_file_size(handle_t file, u64 sz);

		//! Gets the current position of the stream cursor. The position is number of bytes relative to the beginning of the 
		//! stream.
		//! @param[in] file The file handle opened by `open_buffered_file`.
		R<u64> get_buffered_file_cursor(handle_t file);

		//! Moves the cursor to a new position.
		//! @param[in] file The file handle opened by `open_buffered_file`.
		//! @param[in] offset Number of bytes to move relative to the position specified by `mode`.
		//! @param[in] mode The seek mode to use.
		//! @remark The cursor position is not bound to the size of the stream. If you move the cursor beyond the end of the stream, the cursor
		//! is still valid with the following behaviors:
		//! 1. All read operations succeed with 0 byte being read, just like EOF behaviors.
		//! 2. All write operations extends the size of the stream before the actual write operation performed.
		//! 
		//! The cursor value cannot be negative, if the new cursor position goes below 0, the seek operation fails.
		RV set_buffered_file_cursor(handle_t file, i64 offset, SeekMode mode);

		//! Clears all buffers for this file and causes all buffered data to be written to the underlying device.
		//! @param[in] file The file handle opened by `open_file`.
		RV flush_buffered_file(handle_t file);

		//! Gets the attribute/status of one file or directory.
		//! @param[in] path The path of the file to get.
		//! @return Returns the file attribute structure if succeeded, returns error code if failed.
		//! Possible errors:
		//! * BasicError::access_denied
		//! * BasicError::not_found
		//! * BasicError::bad_system_call for all errors that cannot be identified.
		R<FileAttribute> file_attribute(const c8* path);

		//! Copies the file from the source path to the destination path.
		//! @param[in] src_path Source file path.
		//! @param[in] dest_path Destination file path.
		//! @param[in] fail_if_exists If this is `true` and the file named `to_filename` already exists,
		//! this call fails with return value `e_item_already_exists`. If this is `false` and the file named `to_filename` 
		//! already exists, the file named `to_filename` will be overwritten.
		//! @return Returns one of the following error codes if failed:
		//! * BasicError::bad_arguments
		//! * BasicError::already_exists
		//! * BasicError::access_denied
		//! * BasicError::not_found
		//! * BasicError::bad_system_call for all errors that cannot be identified.
		RV	copy_file(const c8* src_path, const c8* dest_path, bool fail_if_exists = false);

		//! Moves the file from the source path to the destination path. This call can also be used to rename a file.
		//! @param[in] src_path Source file path.
		//! @param[in] dest_path Destination file path.
		//! @param[in] allow_copy If the `from_filename` cannot be moved `to_filename` directly, set this to `true` allows 
		//! the system to copy the file from `from_filename` to `to_filename` and delete file in `from_filename`. If this 
		//! is `false` and the file cannot be moved, this call failed.
		//! @param[in] fail_if_exists If this is `true` and the file named `to_filename` already exists,
		//! this call fails with return value `e_item_already_exists`. If this is `false` and the file named `to_filename` 
		//! already exists, the file named `to_filename` will be overwritten.
		//! @return Returns one of the following error codes if failed:
		//! * BasicError::bad_arguments
		//! * BasicError::file_already_exists
		//! * BasicError::access_denied
		//! * BasicError::file_not_exist
		//! * BasicError::bad_system_call for all errors that cannot be identified.
		RV	move_file(const c8* src_path, const c8* dest_path, bool allow_copy = true, bool fail_if_exists = false);

		//! Deletes the specified file.
		//! @param[in] path The file to delete.
		//! @return Returns `s_ok` on success, returns one of the following error codes if failed:
		//! * BasicError::bad_arguments
		//! * BasicError::not_found
		//! * BasicError::access_denied
		//! * BasicError::bad_system_call for all errors that cannot be identified.
		RV	delete_file(const c8* path);

		//! Creates a file iterator handle that can be used to iterate all files in the specified directory.
		//! @param[in] path The directory path to open.
		//! @return Returns a file iterator handle if succeeded. Returns one of the following error codes if failed:
		//! * BasicError::access_denied
		//! * BasicError::not_found
		//! * BasicError::bad_system_call for all errors that cannot be identified.
		R<handle_t> open_dir(const c8* path);

		//! Closes the file iterator handle.
		void close_dir(handle_t dir_iter);

		//! Checks if this iterator points to a valid file item in the directory stream.
		//! @param[in] dir_iter The directory iterator handle.
		bool dir_iterator_valid(handle_t dir_iter);

		//! Returns the name of the file the iterator currently points to.
		//! Returns `nullptr` if the file iterator is invalid.
		//! @param[in] dir_iter The directory iterator handle.
		const c8* dir_iterator_filename(handle_t dir_iter);

		//! Returns the file attribute of the file the iterator currently points to.
		//! Returns EFileAttributeFlag::none if the file iterator is invalid.
		//! @param[in] dir_iter The directory iterator handle.
		EFileAttributeFlag dir_iterator_attribute(handle_t dir_iter);

		//! Moves the file iterator to the next file in the directory.
		//! @param[in] dir_iter The directory iterator handle.
		//! @return Returns `true` if the file iterator is valid after this call, 
		//! returns `false` otherwise.
		//! If the return value is `false`, then the iterator has reached the end of the 
		//! directory stream.
		bool dir_iterator_move_next(handle_t dir_iter);

		//! Creates one directory.
		//! @param[in] path The path of the directory to create.
		//! @return Returns on of the following error codes on error:
		//! * BasicError::already_exists
		//! * BasicError::not_found
		//! * BasicError::bad_system_call for all errors that cannot be identified.
		RV	create_dir(const c8* path);

		//! Removes one directory. If the directory contains files, they will also be removed.
		//! @param[in] path The path of the directory to remove.
		//! @return Returns on of the following error codes on error:
		//! * BasicError::bad_system_call for all errors that cannot be identified.
		RV	remove_dir(const c8* path);

		//! Get the current working directory path for the underlying system.
		//! The default current working directory is set to the path that contains the executable file.
		//! @param[in] buffer_length The length of the buffer for the current directory string, including the null terminator.
		//! @param[in] A pointer to the buffer that receives the current directory string. To gets the required buffer size, specify
		//! `buffer` to `nullptr` and `buffer_length` to 0.
		//! @return Returns the number of `c8` characters copied into the buffer, including the null terminator. If `buffer_length` is 0 and
		//! `buffer` is `nullptr`, returns the required buffer size to fetch the current directory, including the null terminator. 
		u32 get_current_dir(u32 buffer_length, c8* buffer);

		//! Set the current working directory path for the underlying system. The current directory will be set for the process scope.
		//! @param[in] path The path to set. This must be a null-terminated string.
		//! @return Returns `s_ok` on success, returns one of the following error codes on failure:
		//! * e_bad_system_call
		RV set_current_dir(const c8* path);

		//! Get the application executable file's absolute directory, ended with application executable name.
		//! @return Returns the process path. The string is static and valid until the engine is closed.
		const c8* get_process_path();
	}
}