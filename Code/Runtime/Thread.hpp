// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Thread.hpp
* @author JXMaster
* @date 2020/12/10
*/
#pragma once
#include "Base.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	//! Suspends current thread for a specific period of time. The actual suspended time may be longer than required.
	//! @param[in] timeMillisecounds The time, in milliseconds, that this thread needs to suspend.
	LUNA_RUNTIME_API void sleep(u32 time_milliseconds);
	//! Delay the execution of this thread for a very shout time by yielding this thread several times.
	//! This is more accurate to `sleep` method and will not suspend current thread unless the specified time is larger than several milliseconds.
	//! @param[in] time_microseconds The time, in microseconds, that this thread needs to delay.
	LUNA_RUNTIME_API void fast_sleep(u32 time_microseconds);
	//! Yields the remain time slice of the current thread and let OS to schedule other threads.
	//! There is no way to resume a thread from user mode, since threads are scheduled by OS automatically.
	LUNA_RUNTIME_API void yield_current_thread();
	//! Gets the thread ID of the current thread.
	//! @return Returns the thread ID of the current thread.
	LUNA_RUNTIME_API u32 get_current_thread_id();

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
	LUNA_RUNTIME_API R<handle_t> tls_alloc(tls_destructor* destructor = nullptr);

	//! Free the TLS slot allocated by `tls_alloc`. The handle will be invalid after this call and the pointer stored for every 
	//! thread will be discarded.
	//! 
	//! Make sure to free all resources bound to the specified slot manually before calling this, or they will never be freed.
	//! @param[in] handle The handle returned by `tls_alloc`.
	//! @remark Note that calling `tls_free` will not call the destructor registered for this slot on any thread. After `tls_free` is called, the 
	//! destructor will be cleared and will not be called any more.
	LUNA_RUNTIME_API void tls_free(handle_t handle);

	//! Set the data bound to the current thread's TLS slot specified by `handle`.
	//! @param[in] handle The handle of the slot specified.
	//! @param[in] ptr The pointer value to set to this slot.
	LUNA_RUNTIME_API RV tls_set(handle_t handle, void* ptr);

	//! Get the value bound to the TLS slot of current thread.
	//! @param[in] handle The handle of the slot to query.
	//! @return Returns the pointer set, or `nullptr` if no pointer is set to this slot.
	LUNA_RUNTIME_API void* tls_get(handle_t handle);
}