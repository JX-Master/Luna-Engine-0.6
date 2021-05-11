// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Platform.cpp
* @author JXMaster
* @date 2021/4/25
* @brief Platform APIs.
*/
#include "../PlatformDefines.hpp"
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "../Platform.hpp"
#include "OS.hpp"
namespace Luna
{
	namespace Platform
	{
		LUNA_RUNTIME_API u32 get_num_processors()
		{
			return OS::get_num_processors();
		}
		LUNA_RUNTIME_API void debug_printf(const c8* fmt, ...)
		{
			VarList args;
			va_start(args, fmt);
			OS::debug_vprintf(fmt, args);
			va_end(args);
		}
		LUNA_RUNTIME_API void debug_vprintf(const c8* fmt, VarList args)
		{
			OS::debug_vprintf(fmt, args);
		}
		LUNA_RUNTIME_API R<handle_t> load_library(const c8* filename)
		{
			return OS::load_library(filename);
		}
		LUNA_RUNTIME_API RV free_library(handle_t handle)
		{
			return OS::free_library(handle);
		}
		LUNA_RUNTIME_API R<void*> get_proc_address(handle_t handle, const c8* symbol_name)
		{
			return OS::get_proc_address(handle, symbol_name);
		}
		LUNA_RUNTIME_API handle_t new_signal(bool manual_reset)
		{
			return OS::new_signal(manual_reset);
		}
		LUNA_RUNTIME_API void delete_signal(handle_t sig)
		{
			OS::delete_signal(sig);
		}
		LUNA_RUNTIME_API void wait_signal(handle_t sig)
		{
			OS::wait_signal(sig);
		}
		LUNA_RUNTIME_API RV try_wait_signal(handle_t sig)
		{
			return OS::try_wait_signal(sig);
		}
		LUNA_RUNTIME_API void trigger_signal(handle_t sig)
		{
			OS::trigger_signal(sig);
		}
		LUNA_RUNTIME_API void reset_signal(handle_t sig)
		{
			OS::reset_signal(sig);
		}
		LUNA_RUNTIME_API handle_t new_mutex()
		{
			return OS::new_mutex();
		}
		LUNA_RUNTIME_API void delete_mutex(handle_t mtx)
		{
			OS::delete_mutex(mtx);
		}
		LUNA_RUNTIME_API void lock_mutex(handle_t mtx)
		{
			OS::lock_mutex(mtx);
		}
		LUNA_RUNTIME_API RV try_lock_mutex(handle_t mtx)
		{
			return OS::try_lock_mutex(mtx);
		}
		LUNA_RUNTIME_API void unlock_mutex(handle_t mtx)
		{
			OS::unlock_mutex(mtx);
		}
		LUNA_RUNTIME_API handle_t new_semaphore(i32 initial_count, i32 max_count)
		{
			return OS::new_semaphore(initial_count, max_count);
		}
		LUNA_RUNTIME_API void delete_semaphore(handle_t sema)
		{
			OS::delete_semaphore(sema);
		}
		LUNA_RUNTIME_API void acquire_semaphore(handle_t sema)
		{
			OS::acquire_semaphore(sema);
		}
		LUNA_RUNTIME_API RV try_acquire_semaphore(handle_t sema)
		{
			return OS::try_acquire_semaphore(sema);
		}
		LUNA_RUNTIME_API void release_semaphore(handle_t sema)
		{
			OS::release_semaphore(sema);
		}
		LUNA_RUNTIME_API u64 get_ticks()
		{
			return OS::get_ticks();
		}
		LUNA_RUNTIME_API f64 get_ticks_per_second()
		{
			return OS::get_ticks_per_second();
		}
		LUNA_RUNTIME_API i64 get_timestamp()
		{
			return OS::get_timestamp();
		}
		LUNA_RUNTIME_API DateTime timestamp_to_local_datetime(i64 timestamp)
		{
			return OS::timestamp_to_local_datetime(timestamp);
		}
		LUNA_RUNTIME_API DateTime timestamp_to_utc_datetime(i64 timestamp)
		{
			return OS::timestamp_to_utc_datetime(timestamp);
		}
		LUNA_RUNTIME_API i64 local_datetime_to_timestamp(const DateTime& datetime)
		{
			return OS::local_datetime_to_timestamp(datetime);
		}
		LUNA_RUNTIME_API i64 utc_datetime_to_timestamp(const DateTime& datetime)
		{
			return OS::utc_datetime_to_timestamp(datetime);
		}
		LUNA_RUNTIME_API R<handle_t> new_thread(thread_callback_func_t* callback, void* params, usize stack_size)
		{
			return OS::new_thread(callback, params, stack_size);
		}
		LUNA_RUNTIME_API RV set_thread_priority(handle_t thread, ThreadPriority priority)
		{
			return OS::set_thread_priority(thread, (OS::ThreadPriority)priority);
		}
		LUNA_RUNTIME_API void wait_thread(handle_t thread)
		{
			OS::wait_thread(thread);
		}
		LUNA_RUNTIME_API RV try_wait_thread(handle_t thread)
		{
			return OS::try_wait_thread(thread);
		}
		LUNA_RUNTIME_API void detach_thread(handle_t thread)
		{
			OS::detach_thread(thread);
		}
		LUNA_RUNTIME_API handle_t get_current_thread() 
		{
			return OS::get_current_thread();
		}
		LUNA_RUNTIME_API void sleep(u32 time_milliseconds)
		{
			OS::sleep(time_milliseconds);
		}
		LUNA_RUNTIME_API void fast_sleep(u32 time_microseconds)
		{
			OS::fast_sleep(time_microseconds);
		}
		LUNA_RUNTIME_API void yield_current_thread()
		{
			OS::yield_current_thread();
		}
		LUNA_RUNTIME_API R<handle_t> convert_thread_to_fiber()
		{
			return OS::convert_thread_to_fiber();
		}
		LUNA_RUNTIME_API RV convert_fiber_to_thread()
		{
			return OS::convert_fiber_to_thread();
		}
		LUNA_RUNTIME_API R<handle_t> new_fiber(thread_callback_func_t* callback, void* params, usize stack_size)
		{
			return OS::new_fiber(callback, params, stack_size);
		}
		LUNA_RUNTIME_API void delete_fiber(handle_t fiber)
		{
			OS::delete_fiber(fiber);
		}
		LUNA_RUNTIME_API void switch_to_fiber(handle_t fiber)
		{
			OS::switch_to_fiber(fiber);
		}
		LUNA_RUNTIME_API R<handle_t> open_file(const c8* path, FileOpenFlag flags, FileCreationMode creation)
		{
			return OS::open_file(path, (OS::FileOpenFlag)flags, (OS::FileCreationMode)creation);
		}
		LUNA_RUNTIME_API void close_file(handle_t file)
		{
			OS::close_file(file);
		}
		LUNA_RUNTIME_API RV read_file(handle_t file, void* buffer, usize size, usize* read_bytes)
		{
			return OS::read_file(file, buffer, size, read_bytes);
		}
		LUNA_RUNTIME_API RV write_file(handle_t file, const void* buffer, usize size, usize* write_bytes)
		{
			return OS::write_file(file, buffer, size, write_bytes);
		}
		LUNA_RUNTIME_API R<u64> get_file_size(handle_t file)
		{
			return OS::get_file_size(file);
		}
		LUNA_RUNTIME_API RV set_file_size(handle_t file, u64 sz)
		{
			return OS::set_file_size(file, sz);
		}
		LUNA_RUNTIME_API R<u64> get_file_cursor(handle_t file)
		{
			return OS::get_file_cursor(file);
		}
		LUNA_RUNTIME_API RV set_file_cursor(handle_t file, i64 offset, SeekMode mode)
		{
			return OS::set_file_cursor(file, offset, (OS::SeekMode)mode);
		}
		LUNA_RUNTIME_API RV flush_file(handle_t file)
		{
			return OS::flush_file(file);
		}
		LUNA_RUNTIME_API R<handle_t> open_buffered_file(const c8* path, FileOpenFlag flags, FileCreationMode creation)
		{
			return OS::open_buffered_file(path, (OS::FileOpenFlag)flags, (OS::FileCreationMode)creation);
		}
		LUNA_RUNTIME_API void close_buffered_file(handle_t file)
		{
			OS::close_buffered_file(file);
		}
		LUNA_RUNTIME_API RV read_buffered_file(handle_t file, void* buffer, usize size, usize* read_bytes)
		{
			return OS::read_buffered_file(file, buffer, size, read_bytes);
		}
		LUNA_RUNTIME_API RV write_buffered_file(handle_t file, const void* buffer, usize size, usize* write_bytes)
		{
			return OS::write_buffered_file(file, buffer, size, write_bytes);
		}
		LUNA_RUNTIME_API R<u64> get_buffered_file_size(handle_t file)
		{
			return OS::get_buffered_file_size(file);
		}
		LUNA_RUNTIME_API RV set_buffered_file_size(handle_t file, u64 sz)
		{
			return OS::set_buffered_file_size(file, sz);
		}
		LUNA_RUNTIME_API R<u64> get_buffered_file_cursor(handle_t file)
		{
			return OS::get_buffered_file_cursor(file);
		}
		LUNA_RUNTIME_API RV set_buffered_file_cursor(handle_t file, i64 offset, SeekMode mode)
		{
			return OS::set_buffered_file_cursor(file, offset, (OS::SeekMode)mode);
		}
		LUNA_RUNTIME_API RV flush_buffered_file(handle_t file)
		{
			return OS::flush_buffered_file(file);
		}
		LUNA_RUNTIME_API R<FileAttribute> file_attribute(const c8* path)
		{
			auto r = OS::file_attribute(path);
			if (r.valid())
			{
				FileAttribute attr;
				memcpy(&attr, &r.get(), sizeof(FileAttribute));
				return attr;
			}
			return r.errcode();
		}
		LUNA_RUNTIME_API RV	copy_file(const c8* src_path, const c8* dest_path, bool fail_if_exists)
		{
			return OS::copy_file(src_path, dest_path, fail_if_exists);
		}
		LUNA_RUNTIME_API RV	move_file(const c8* src_path, const c8* dest_path, bool allow_copy, bool fail_if_exists)
		{
			return OS::move_file(src_path, dest_path, allow_copy, fail_if_exists);
		}
		LUNA_RUNTIME_API RV	delete_file(const c8* path)
		{
			return OS::delete_file(path);
		}
		LUNA_RUNTIME_API R<handle_t> open_dir(const c8* path)
		{
			return OS::open_dir(path);
		}
		LUNA_RUNTIME_API void close_dir(handle_t dir_iter)
		{
			OS::close_dir(dir_iter);
		}
		LUNA_RUNTIME_API bool dir_iterator_valid(handle_t dir_iter)
		{
			return OS::dir_iterator_valid(dir_iter);
		}
		LUNA_RUNTIME_API const c8* dir_iterator_filename(handle_t dir_iter)
		{
			return OS::dir_iterator_filename(dir_iter);
		}
		LUNA_RUNTIME_API EFileAttributeFlag dir_iterator_attribute(handle_t dir_iter)
		{
			return (EFileAttributeFlag)OS::dir_iterator_attribute(dir_iter);
		}
		LUNA_RUNTIME_API bool dir_iterator_move_next(handle_t dir_iter)
		{
			return OS::dir_iterator_move_next(dir_iter);
		}
		LUNA_RUNTIME_API RV	create_dir(const c8* path)
		{
			return OS::create_dir(path);
		}
		LUNA_RUNTIME_API RV	remove_dir(const c8* path)
		{
			return OS::remove_dir(path);
		}
		LUNA_RUNTIME_API u32 get_current_dir(u32 buffer_length, c8* buffer)
		{
			return OS::get_current_dir(buffer_length, buffer);
		}
		LUNA_RUNTIME_API RV set_current_dir(const c8* path)
		{
			return OS::set_current_dir(path);
		}
		LUNA_RUNTIME_API const c8* get_process_path()
		{
			return OS::get_process_path();
		}
	}
}