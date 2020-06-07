// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Core.cpp
* @author JXMaster
* @date 2019/1/10
*/
#include <Base/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT

#include "Platform.hpp"
#include "String/StringLib.hpp"
#include "Log/LogSystem.hpp"
#include "UpdateLoop/UpdateLoop.hpp"
#include "Data/DataSystem.hpp"
#include "Vfs/Vfs.hpp"
#include "Error/Error.hpp"
#include "Dispatch/ThreadPool.hpp"
#include "Log/StdLogCallback.hpp"
namespace luna
{
	P<ILogger> g_system_logger;
	P<ILogCallback> g_std_callback;
	P<ILogHandler> g_std_handler;

	namespace core
	{
		ILogger* g_ilogger;
	}

	LUNA_CORE_API RV debug_printf(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		RV r = platform::debug_vprintf(fmt, args);
		va_end(args);
		return r;
	}
	LUNA_CORE_API RV debug_vprintf(const char* fmt, va_list args)
	{
		return platform::debug_vprintf(fmt, args);
	}
	LUNA_CORE_API RP<IFile> platform_open_file(const char* filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		return platform::open_file(filename, flags, creation);
	}
	LUNA_CORE_API RP<IAsyncFile> platform_open_file_async(const char* filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		return platform::open_file_async(filename, flags, creation);
	}
	LUNA_CORE_API RV platform_file_attribute(const char* filename, FileAttribute& attribute)
	{
		return platform::file_attribute(filename, attribute);
	}
	LUNA_CORE_API RV platform_file_exists(const char* filename)
	{
		return platform::file_exists(filename);
	}
	LUNA_CORE_API RV platform_dir_exists(const char* dir_name)
	{
		return platform::dir_exists(dir_name);
	}
	LUNA_CORE_API R<uint64>	platform_file_size(const char* filename)
	{
		return platform::file_size(filename);
	}
	LUNA_CORE_API RV platform_copy_file(const char* from_filename, const char* to_filename, bool fail_if_exists)
	{
		return platform::copy_file(from_filename, to_filename, fail_if_exists);
	}
	LUNA_CORE_API RV platform_move_file(const char* from_filename, const char* to_filename, bool allow_copy, bool fail_if_exists)
	{
		return platform::move_file(from_filename, to_filename, allow_copy, fail_if_exists);
	}
	LUNA_CORE_API RV platform_delete_file(const char* filename)
	{
		return platform::delete_file(filename);
	}
	LUNA_CORE_API RP<IFileIterator> platform_open_dir(const char* dir_path)
	{
		return platform::open_dir(dir_path);
	}
	LUNA_CORE_API RV platform_create_dir(const char* pathname)
	{
		return platform::create_dir(pathname);
	}
	LUNA_CORE_API RV platform_remove_dir(const char* pathname)
	{
		return platform::remove_dir(pathname);
	}
	LUNA_CORE_API uint32 get_current_dir(uint32 buffer_length, char* buffer)
	{
		return platform::get_current_dir(buffer_length, buffer);
	}
	LUNA_CORE_API RV set_current_dir(const char* path)
	{
		return platform::set_current_dir(path);
	}
	LUNA_CORE_API const char* get_process_path()
	{
		return platform::get_process_path();
	}
	LUNA_CORE_API RV get_system_info(ESystemInfo system_info, void* data)
	{
		return platform::get_system_info(system_info, data);
	}
	LUNA_CORE_API IHeap* get_global_heap()
	{
		return platform::get_global_heap();
	}
	LUNA_CORE_API RP<IHeap> new_heap(EHeapCreationFlag heap_creation_flags, size_t initial_size, size_t maximum_size, const char* name)
	{
		return platform::new_heap(heap_creation_flags, initial_size, maximum_size, name);
	}
	LUNA_CORE_API void* virtual_reserve(void* address, size_t size)
	{
		return platform::virtual_reserve(address, size);
	}
	LUNA_CORE_API void* virtual_commit(void* address, size_t size, EPageProtection protection)
	{
		return platform::virtual_commit(address, size, protection);
	}
	LUNA_CORE_API RV virtual_decommit(void* address, size_t size)
	{
		return platform::virtual_decommit(address, size);
	}
	LUNA_CORE_API RV virtual_release(void* address)
	{
		return platform::virtual_release(address);
	}
	LUNA_CORE_API RV virtual_protect(void* address, size_t size, EPageProtection new_protection, EPageProtection* old_protection)
	{
		return platform::virtual_protect(address, size, new_protection, old_protection);
	}
	LUNA_CORE_API P<IRandom> new_random(uint32 initial_seed)
	{
		return platform::new_random(initial_seed);
	}
	LUNA_CORE_API uint32 random_u32()
	{
		return platform::random_u32();
	}
	LUNA_CORE_API int32 random_i32()
	{
		return platform::random_i32();
	}
	LUNA_CORE_API uint64 random_u64()
	{
		return platform::random_u64();
	}
	LUNA_CORE_API int64 random_i64()
	{
		return platform::random_i64();
	}
	LUNA_CORE_API float32 random_f32(float32 range_begin, float32 range_end)
	{
		return platform::random_f32(range_begin, range_end);
	}
	LUNA_CORE_API float64 random_f64(float64 range_begin, float64 range_end)
	{
		return platform::random_f64(range_begin, range_end);
	}
	LUNA_CORE_API Guid random_guid()
	{
		return platform::random_guid();
	}
	LUNA_CORE_API R<handle_t> load_library(const char* filename)
	{
		return platform::load_library(filename);
	}
	LUNA_CORE_API RV free_library(handle_t handle)
	{
		return platform::free_library(handle);
	}
	LUNA_CORE_API void* get_proc_address(handle_t handle, const char* symbol_name)
	{
		return platform::get_proc_address(handle, symbol_name);
	}
	LUNA_CORE_API P<ISignal> new_signal(bool manual_reset)
	{
		return platform::new_signal(manual_reset);
	}
	LUNA_CORE_API P<IMutex> new_mutex()
	{
		return platform::new_mutex();
	}
	LUNA_CORE_API P<ISemaphore> new_semaphore(int32 initial_count, int32 max_count)
	{
		return platform::new_semaphore(initial_count, max_count);
	}
	LUNA_CORE_API P<IReadWriteLock> new_read_write_lock()
	{
		return platform::new_read_write_lock();
	}
	LUNA_CORE_API RP<IThread> new_thread(IRunnable* callback, uint32 stack_size)
	{
		return platform::new_thread(callback, stack_size);
	}
	LUNA_CORE_API IThread* get_current_thread()
	{
		return platform::get_current_thread();
	}
	LUNA_CORE_API IThread* get_main_thread()
	{
		return platform::get_main_thread();
	}
	LUNA_CORE_API void sleep(uint32 time_milliseconds)
	{
		return platform::sleep(time_milliseconds);
	}
	LUNA_CORE_API void fast_sleep(uint32 time_microseconds)
	{
		return platform::fast_sleep(time_microseconds);
	}
	LUNA_CORE_API void yield_current_thread()
	{
		return platform::yield_current_thread();
	}
	LUNA_CORE_API RP<IFiber> convert_thread_to_fiber()
	{
		return platform::convert_thread_to_fiber();
	}
	LUNA_CORE_API RV convert_fiber_to_thread()
	{
		return platform::convert_fiber_to_thread();
	}
	LUNA_CORE_API RP<IFiber> new_fiber(IRunnable* callback, uint32 stack_size)
	{
		return platform::new_fiber(callback, stack_size);
	}
	LUNA_CORE_API IFiber* get_current_fiber()
	{
		return platform::get_current_fiber();
	}
	LUNA_CORE_API RV resume_fiber(IFiber* fiber)
	{
		return platform::resume_fiber(fiber);
	}
	LUNA_CORE_API RV yield_current_fiber()
	{
		return platform::yield_current_fiber();
	}
	LUNA_CORE_API RV tls_alloc(handle_t* handle)
	{
		return platform::tls_alloc(handle);
	}
	LUNA_CORE_API RV tls_free(handle_t handle)
	{
		return platform::tls_free(handle);
	}
	LUNA_CORE_API RV tls_set(handle_t handle, void* ptr)
	{
		return platform::tls_set(handle, ptr);
	}
	LUNA_CORE_API RV tls_get(handle_t handle, void** ptr)
	{
		return platform::tls_get(handle, ptr);
	}
	LUNA_CORE_API uint64 get_ticks()
	{
		return platform::get_ticks();
	}
	LUNA_CORE_API uint64 get_ticks_per_second()
	{
		return platform::get_ticks_per_second();
	}
	LUNA_CORE_API uint64 get_elapsed_ticks()
	{
		return platform::get_elapsed_ticks();
	}
	LUNA_CORE_API uint64 get_local_timestamp()
	{
		return platform::get_local_timestamp();
	}
	LUNA_CORE_API uint64 get_utc_timestamp()
	{
		return platform::get_utc_timestamp();
	}
	LUNA_CORE_API void timestamp_to_data_time(uint64 timestamp, DateTime& datetime)
	{
		platform::timestamp_to_data_time(timestamp, datetime);
	}
	LUNA_CORE_API uint64 date_time_to_timestamp(const DateTime& datetime)
	{
		return platform::date_time_to_timestamp(datetime);
	}
	LUNA_CORE_API P<IName> intern_name(const char* name, size_t len)
	{
		return strlib::intern_name(name, len);
	}
	LUNA_CORE_API RP<IName> check_name(const char* name, size_t len)
	{
		return strlib::check_name(name, len);
	}
	LUNA_CORE_API P<IStringBuffer> new_string_buffer()
	{
		return strlib::new_string_buffer();
	}
	LUNA_CORE_API P<IString> new_string(const char* data, size_t len)
	{
		return strlib::new_string(data, len);
	}
	LUNA_CORE_API P<IBlob> new_blob(const void* data, size_t len, IAllocator* alloc)
	{
		return strlib::new_blob(data, len, alloc);
	}
	LUNA_CORE_API P<IBuffer> new_buffer(size_t buffer_size, size_t alignment, IAllocator* alloc)
	{
		return strlib::new_buffer(buffer_size, alignment, alloc);
	}
	LUNA_CORE_API P<IBuffer> box_buffer(void* buffer, size_t buffer_size, IAllocator* alloc, size_t alignment)
	{
		return strlib::box_buffer(buffer, buffer_size, alloc, alignment);
	}
	LUNA_CORE_API P<IMemoryStream> new_memory_stream(size_t initial_buffer_size, size_t alignment, IAllocator* alloc)
	{
		return strlib::new_memory_stream(initial_buffer_size, alignment, alloc);
	}
	LUNA_CORE_API P<IMemoryStream> new_memory_stream_from_buffer(IBuffer* buffer)
	{
		return strlib::new_memory_stream_from_buffer(buffer);
	}
	LUNA_CORE_API P<IMutPath> new_path(const char* path_string, size_t len)
	{
		return strlib::new_path(path_string, len);
	}
	LUNA_CORE_API P<ILogger> new_logger(IName* channel)
	{
		return log_sys::new_logger(channel);
	}
	LUNA_CORE_API P<ILogHandler> new_log_handler(IName* name, ILogCallback* callback)
	{
		return log_sys::new_log_handler(name, callback);
	}
	LUNA_CORE_API RP<ILogHandler> get_log_handler(IName* name)
	{
		return log_sys::get_log_handler(name);
	}
	LUNA_CORE_API void new_frame()
	{
		update_loop::new_frame();
	}
	LUNA_CORE_API uint32 get_frame_index()
	{
		return update_loop::get_frame_index();
	}
	LUNA_CORE_API uint64 get_last_frame_ticks()
	{
		return update_loop::get_last_frame_ticks();
	}
	LUNA_CORE_API uint64 get_last_frame_ticks_raw()
	{
		return update_loop::get_last_frame_raw_ticks();
	}
	LUNA_CORE_API uint64 get_last_n_frame_ticks(uint32 frame_index)
	{
		return update_loop::get_frame_ticks(frame_index);
	}
	LUNA_CORE_API uint64 get_last_n_frame_ticks_raw(uint32 frame_index)
	{
		return update_loop::get_frame_ticks_raw(frame_index);
	}
	LUNA_CORE_API float32 get_fps()
	{
		return update_loop::get_fps();
	}
	LUNA_CORE_API float32 get_fps_raw()
	{
		return update_loop::get_fps_raw();
	}
	LUNA_CORE_API float32 get_fps_averaged()
	{
		return update_loop::get_fps_average();
	}
	LUNA_CORE_API float32 get_fps_averaged_raw()
	{
		return update_loop::get_fps_average_raw();
	}
	LUNA_CORE_API uint64 get_frame_elapsed_ticks()
	{
		return update_loop::get_frame_elapsed_ticks();
	}
	LUNA_CORE_API void set_frame_governing_enabled(bool enabled)
	{
		update_loop::set_frame_governing_enabled(enabled);
	}
	LUNA_CORE_API bool is_frame_governing_enabled()
	{
		return update_loop::is_frame_governing_enabled();
	}
	LUNA_CORE_API RV set_frame_governing_fps(float32 frames_per_second)
	{
		return update_loop::set_frame_governing_fps(frames_per_second);
	}
	LUNA_CORE_API float32 get_frame_governing_fps()
	{
		return update_loop::get_frame_governing_fps();
	}
	LUNA_CORE_API void set_min_frame_fps_clamping_enabled(bool enabled)
	{
		update_loop::set_min_frame_fps_clamping_enabled(enabled);
	}
	LUNA_CORE_API bool is_min_frame_fps_clamping_enabled()
	{
		return update_loop::is_min_frame_fps_clamping_enabled();
	}
	LUNA_CORE_API void set_min_frame_fps(float32 frames_per_second)
	{
		update_loop::set_min_frame_fps(frames_per_second);
	}
	LUNA_CORE_API float32 get_min_frame_fps()
	{
		return update_loop::get_min_frame_fps();
	}
	LUNA_CORE_API P<IVariant> new_var(EVariantType t)
	{
		return data::new_var(t);
	}
	LUNA_CORE_API P<IVariant> new_var1(EVariantType t, size_t d1)
	{
		return data::new_var1(t, d1);
	}
	LUNA_CORE_API P<IVariant> new_var2(EVariantType t, size_t d1, size_t d2)
	{
		return data::new_var2(t, d1, d2);
	}
	LUNA_CORE_API P<IVariant> new_var3(EVariantType t, size_t d1, size_t d2, size_t d3)
	{
		return data::new_var3(t, d1, d2, d3);
	}
	LUNA_CORE_API P<IVariant> new_var4(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4)
	{
		return data::new_var4(t, d1, d2, d3, d4);
	}
	LUNA_CORE_API P<IVariant> new_var5(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5)
	{
		return data::new_var5(t, d1, d2, d3, d4, d5);
	}
	LUNA_CORE_API P<IVariant> new_var6(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6)
	{
		return data::new_var6(t, d1, d2, d3, d4, d5, d6);
	}
	LUNA_CORE_API P<IVariant> new_var7(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7)
	{
		return data::new_var7(t, d1, d2, d3, d4, d5, d6, d7);
	}
	LUNA_CORE_API P<IVariant> new_var8(EVariantType t, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7, size_t d8)
	{
		return data::new_var8(t, d1, d2, d3, d4, d5, d6, d7, d8);
	}
	LUNA_CORE_API P<IEncoder> new_text_encoder()
	{
		return data::new_text_encoder();
	}
	LUNA_CORE_API P<IDecoder> new_text_decoder()
	{
		return data::new_text_decoder();
	}
	LUNA_CORE_API RP<IFile> open_file(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		return vfs::open_file(filename, flags, creation);
	}

	LUNA_CORE_API RP<IAsyncFile> open_file_async(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		return vfs::open_file_async(filename, flags, creation);
	}

	LUNA_CORE_API RV file_attribute(IPath* filename, FileAttribute& attribute)
	{
		return vfs::file_attribute(filename, attribute);
	}

	LUNA_CORE_API RV file_exists(IPath* filename)
	{
		return vfs::file_exists(filename);
	}

	LUNA_CORE_API RV dir_exists(IPath* dir_name)
	{
		return vfs::dir_exists(dir_name);
	}

	LUNA_CORE_API R<uint64> file_size(IPath* filename)
	{
		return vfs::file_size(filename);
	}

	LUNA_CORE_API RV copy_file(IPath* from_filename, IPath* to_filename, bool fail_if_exists)
	{
		return vfs::copy_file(from_filename, to_filename, fail_if_exists);
	}

	LUNA_CORE_API RV move_file(IPath* from_filename, IPath* to_filename, bool allow_copy, bool fail_if_exists)
	{
		return vfs::move_file(from_filename, to_filename, allow_copy, fail_if_exists);
	}

	LUNA_CORE_API RV delete_file(IPath* filename)
	{
		return vfs::delete_file(filename);
	}

	LUNA_CORE_API RP<IFileIterator> open_dir(IPath* dir_path)
	{
		return vfs::open_dir(dir_path);
	}

	LUNA_CORE_API RV create_dir(IPath* pathname)
	{
		return vfs::create_dir(pathname);
	}

	LUNA_CORE_API RV remove_dir(IPath* pathname, bool recursive)
	{
		return vfs::remove_dir(pathname, recursive);
	}

	LUNA_CORE_API RV mount_fs(IPath* mount_point, IFileSystem* fs)
	{
		return vfs::mount_fs(mount_point, fs);
	}

	LUNA_CORE_API RV mount_virtual_path(IPath* mount_point, IPath* vfs_path)
	{
		return vfs::mount_virtual_path(mount_point, vfs_path);
	}

	LUNA_CORE_API RV mount_platfrom_path(IPath* mount_point, IPath* platform_path)
	{
		return vfs::mount_platfrom_path(mount_point, platform_path);
	}

	LUNA_CORE_API RV unmount_fs(IPath* mount_point)
	{
		return vfs::unmount_fs(mount_point);
	}

	LUNA_CORE_API RP<IFileSystem> get_fs(IPath* mount_point)
	{
		return vfs::get_fs(mount_point);
	}

	LUNA_CORE_API Vector<P<IString>> mount_points()
	{
		return vfs::mount_points();
	}

	LUNA_CORE_API void clear_err()
	{
		platform::clear_err();
	}

	LUNA_CORE_API void set_err(result_t result_code, const char* msgfmt, ...)
	{
		va_list args;
		va_start(args, msgfmt);
		platform::set_verr(result_code, msgfmt, args);
		va_end(args);
	}
	LUNA_CORE_API void set_verr(result_t result_code, const char* msgfmt, va_list args)
	{
		platform::set_verr(result_code, msgfmt, args);
	}
	LUNA_CORE_API void set_err_user(const Guid& result_domain, result_t result_code, const char* msgfmt, ...)
	{
		va_list args;
		va_start(args, msgfmt);
		platform::set_verr_user(result_domain, result_code, msgfmt, args);
		va_end(args);
	}
	LUNA_CORE_API void set_verr_user(const Guid& result_domain, result_t result_code, const char* msgfmt, va_list args)
	{
		platform::set_verr_user(result_domain, result_code, msgfmt, args);
	}
	LUNA_CORE_API void set_err_var(result_t result_code, IVariant* variant, const char* msgfmt, ...)
	{
		va_list args;
		va_start(args, msgfmt);
		platform::set_verr_var(result_code, variant, msgfmt, args);
		va_end(args);
	}
	LUNA_CORE_API void set_verr_var(result_t result_code, IVariant* variant, const char* msgfmt, va_list args)
	{
		platform::set_verr_var(result_code, variant, msgfmt, args);
	}
	LUNA_CORE_API void set_err_var_user(const Guid& result_domain, result_t result_code, IVariant* variant, const char* msgfmt, ...)
	{
		va_list args;
		va_start(args, msgfmt);
		platform::set_verr_var_user(result_domain, result_code, variant, msgfmt, args);
		va_end(args);
	}
	LUNA_CORE_API void set_verr_var_user(const Guid& result_domain, result_t result_code, IVariant* variant, const char* msgfmt, va_list args)
	{
		platform::set_verr_var_user(result_domain, result_code, variant, msgfmt, args);
	}

	LUNA_CORE_API void set_err(IError* err_obj)
	{
		if (err_obj->err_var())
		{
			if (err_obj->err_domain() != err_domain_base)
			{
				set_err_var_user(err_obj->err_domain(), err_obj->err_result(), err_obj->err_var(), err_obj->err_msg()->c_str());
			}
			else
			{
				set_err_var(err_obj->err_result(), err_obj->err_var(), err_obj->err_msg()->c_str());
			}
		}
		else
		{
			if (err_obj->err_domain() != err_domain_base)
			{
				set_err_user(err_obj->err_domain(), err_obj->err_result(), err_obj->err_msg()->c_str());
			}
			else
			{
				set_err(err_obj->err_result(), err_obj->err_msg()->c_str());
			}
		}
	}

	LUNA_CORE_API const char* err_msg()
	{
		return platform::err_msg();
	}
	LUNA_CORE_API const Guid& err_domain()
	{
		return platform::err_domain();
	}
	LUNA_CORE_API result_t err_result()
	{
		return platform::err_result();
	}
	LUNA_CORE_API IVariant* err_var()
	{
		return platform::err_var();
	}
	LUNA_CORE_API P<IError> new_err()
	{
		return error::new_err();
	}
	LUNA_CORE_API P<IError> promote_err()
	{
		return error::promote_err();
	}
	LUNA_CORE_API const char* explain(result_t result_code)
	{
		return error::explain(result_code);
	}
	LUNA_CORE_API P<IError> err_from_code(result_t result_code)
	{
		return error::err_from_code(result_code);
	}
	LUNA_CORE_API P<IDispatchQueue> new_dispatch_queue(uint32 concurrency_limit)
	{
		return dispatch::new_dispatch_queue(concurrency_limit);
	}

	struct ModuleRecord
	{
		const char* m_name;
		module_deinit_func_t* m_deinit;
		uint32 m_ref_count = 1;
	};

	P<IMutex> g_module_mtx;
	Unconstructed<Vector<ModuleRecord>> g_modules;

	LUNA_CORE_API RV init()
	{
		if (failed(platform::init()))
		{
			return e_failure;
		}

		strlib::init(get_global_heap());

		log_sys::init(get_global_heap());
		g_system_logger = new_logger(intern_name("Core").get()).get();

		g_std_log_callback.construct();
		g_std_callback = box_ptr<ILogCallback>(&(g_std_log_callback.get()));
		g_std_handler = new_log_handler(intern_name("Std").get(), g_std_callback).get();
		g_std_handler->set_listening_all(true);

		core::g_ilogger = g_system_logger.get();

		update_loop::init();

		vfs::init(get_module_allocator());

		dispatch::init(get_module_allocator());

		g_module_mtx = new_mutex();
		g_modules.construct(get_global_heap());

		return s_ok;
	}

	LUNA_CORE_API void shutdown()
	{
		MutexGuard g(g_module_mtx);
		auto modules = g_modules.get();
		for (auto iter = modules.rbegin(); iter != modules.rend(); ++iter)
		{
			iter->m_deinit();
		}
		modules.clear();
		g.unlock();
		g_module_mtx = nullptr;
		g_modules.destruct();

		dispatch::deinit();
		vfs::deinit();
		g_system_logger = nullptr;
		g_std_handler = nullptr;
		g_std_callback = nullptr;
		log_sys::deinit();
		strlib::deinit();
		platform::close();
	}

	LUNA_CORE_API void add_module(const char* module_name, module_deinit_func_t* deinit_func)
	{
		MutexGuard g(g_module_mtx);
		ModuleRecord rec;
		rec.m_deinit = deinit_func;
		rec.m_name = module_name;
		g_modules.get().push_back(rec);
	}

	LUNA_CORE_API RV remove_module(const char* module_name)
	{
		MutexGuard g(g_module_mtx);
		auto modules = g_modules.get();
		for (auto iter = modules.rbegin(); iter != modules.rend(); ++iter)
		{
			if (!strcmp(iter->m_name, module_name))
			{
				--iter->m_ref_count;
				if (!iter->m_ref_count)
				{
					iter->m_deinit();
					modules.erase(iter.base());
				}
				return s_ok;
			}
		}
		return e_item_not_exist;
	}
}