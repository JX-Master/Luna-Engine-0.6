// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IPlatform.hpp
* @author JXMaster
* @date 2020/1/8
 */
#pragma once
#include "CoreHeader.hpp"

namespace luna
{
	//! Platform Independent Layer
	//! The platform implementation should implement the following functions
	//! In order to let Luna Engine run on that platform.
	namespace platform
	{
		//! Called when the Core object is creating.
		RV init();

		//! Called when the last reference to the core object is released.
		void close();

		RV debug_printf(const char* fmt, ...);
		RV debug_vprintf(const char* fmt, va_list args);
		RP<IFile>	open_file(const char* filename, EFileOpenFlag flags, EFileCreationMode creation);
		RP<IAsyncFile> open_file_async(const char* filename, EFileOpenFlag flags, EFileCreationMode creation);
		RV file_attribute(const char* filename, FileAttribute& attribute);
		RV file_exists(const char* filename);
		RV dir_exists(const char* dir_name);
		R<uint64>	file_size(const char* filename);
		RV	copy_file(const char* from_filename, const char* to_filename, bool fail_if_exists);
		RV	move_file(const char* from_filename, const char* to_filename, bool allow_copy, bool fail_if_exists);
		RV	delete_file(const char* filename);
		RP<IFileIterator> open_dir(const char* filename);
		RV	create_dir(const char* pathname);
		RV	remove_dir(const char* pathname);
		uint32	get_current_dir(uint32 buffer_length, char* buffer);
		RV set_current_dir(const char* path);
		const char*	get_process_path();
		RV get_system_info(ESystemInfo system_info, void* data);
		IHeap* get_global_heap();
		RP<IHeap> new_heap(EHeapCreationFlag heap_creation_flags, size_t initial_size, size_t maximum_size, const char* name);
		void* virtual_reserve(void* address, size_t size);
		void* virtual_commit(void* address, size_t size, EPageProtection protection = EPageProtection::read_write);
		RV virtual_decommit(void* address, size_t size);
		RV virtual_release(void* address);
		RV virtual_protect(void* address, size_t size, EPageProtection new_protection, EPageProtection* old_protection);
		P<IRandom> new_random(uint32 initial_seed);
		uint32 random_u32();
		int32 random_i32();
		uint64 random_u64();
		int64 random_i64();
		float32 random_f32(float32 range_begin, float32 range_end);
		float64 random_f64(float64 range_begin, float64 range_end);
		Guid random_guid();
		R<handle_t> load_library(const char* filename);
		RV free_library(handle_t handle);
		void* get_proc_address(handle_t handle, const char* symbol_name);
		P<ISignal> new_signal(bool manual_reset);
		P<IMutex> new_mutex();
		P<ISemaphore> new_semaphore(int32 initial_count, int32 max_count);
		P<IReadWriteLock> new_read_write_lock();
		RP<IThread> new_thread(IRunnable* callback, uint32 stack_size);
		IThread* get_current_thread();
		IThread* get_main_thread();
		void sleep(uint32 time_milliseconds);
		void fast_sleep(uint32 time_microseconds);
		void yield_current_thread();
		RP<IFiber> convert_thread_to_fiber();
		RV convert_fiber_to_thread();
		RP<IFiber> new_fiber(IRunnable* callback, uint32 stack_size);
		IFiber* get_current_fiber();
		RV resume_fiber(IFiber* fiber);
		RV yield_current_fiber();
		RV tls_alloc(handle_t* handle);
		RV tls_free(handle_t handle);
		RV tls_set(handle_t handle, void* ptr);
		RV tls_get(handle_t handle, void** ptr);
		uint64 get_ticks();
		uint64 get_ticks_per_second();
		uint64 get_elapsed_ticks();
		uint64 get_local_timestamp();
		uint64 get_utc_timestamp();
		void timestamp_to_data_time(uint64 timestamp, DateTime& datetime);
		uint64 date_time_to_timestamp(const DateTime& datetime);

		// The current implementation embeds the error object system directly into system level.
		void clear_err();
		void set_verr(result_t result_code, const char* msgfmt, va_list args);
		void set_verr_var(result_t result_code, IVariant* variant, const char* msgfmt, va_list args);
		void set_verr_user(const Guid& result_domain, result_t result_code, const char* msgfmt, va_list args);
		void set_verr_var_user(const Guid& result_domain, result_t result_code, IVariant* variant, const char* msgfmt, va_list args);
		const char* err_msg();
		const Guid& err_domain();
		result_t err_result();
		IVariant* err_var();
	}
}