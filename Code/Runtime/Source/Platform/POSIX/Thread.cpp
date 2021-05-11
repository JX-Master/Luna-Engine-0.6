// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Thread.cpp
* @author JXMaster
* @date 2020/9/28
*/
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#include "../../OS.hpp"

#include <unistd.h>
#include <ucontext.h>
#include <pthread.h>

namespace Luna
{
    namespace OS
    {
        struct Fiber
        {
			ucontext_t m_context;
			void* m_stack;
			thread_callback_func_t* m_func;
			void* m_params;
        };

		struct Thread
		{
			pthread_t m_handle;        // Thread handle.
			int m_sched_policy;
			sched_param m_sched_param;

			thread_callback_func_t* m_func;
			void* m_params;
			handle_t m_finish_signal;

            Fiber* m_native_fiber = nullptr;
            Fiber* m_current_fiber = nullptr;

			bool m_detached = false;
		};

        thread_local Thread* tls_current_thread;

		static void* posix_thread_main(void* cookie)
		{
			Thread* t = (Thread*)cookie;
            tls_current_thread = t;
			t->m_func(t->m_params);
            trigger_signal(t->m_finish_signal);
            while (!t->m_detached)
            {
                yield_current_thread();
            }
            memdelete(t);
			return 0;
		}

		static void posix_uctx_main(u32 low, u32 high)
		{
			Fiber* f = (Fiber*)((usize)low | ((usize)high << 32));
			f->m_func(f->m_params);
		}

        R<handle_t> new_thread(thread_callback_func_t* callback, void* params, usize stack_size)
        {
            Thread* t = memnew<Thread>();
			t->m_func = callback;
            t->m_params = params;
            t->m_finish_signal = new_signal(true);
			if (stack_size == 0)
			{
				stack_size = 2_mb;
			}
			pthread_attr_t attr;
			pthread_attr_init(&attr);

			pthread_attr_setstacksize(&attr, stack_size);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
			pthread_attr_getschedpolicy(&attr, &(t->m_sched_policy));
			pthread_attr_getschedparam(&attr, &(t->m_sched_param));

			int r = pthread_create(&(t->m_handle), &attr, &posix_thread_main, t);
			pthread_attr_destroy(&attr);
			if (r != 0)
			{
                memdelete(t);
				return BasicError::bad_system_call();
			}
            return t;
        }
        RV set_thread_priority(handle_t thread, ThreadPriority priority)
        {
            Thread* t = (Thread*)thread;
            sched_param param = t->m_sched_param;
            switch (priority)
            {
            case ThreadPriority::low:
                param.sched_priority = (param.sched_priority + sched_get_priority_min(t->m_sched_policy)) >> 1;
                pthread_setschedparam(t->m_handle, t->m_sched_policy, &param);
                break;
            case ThreadPriority::normal:
                pthread_setschedparam(t->m_handle, t->m_sched_policy, &param);
                break;
            case ThreadPriority::high:
                param.sched_priority = (param.sched_priority + sched_get_priority_max(t->m_sched_policy)) >> 1;
                pthread_setschedparam(t->m_handle, t->m_sched_policy, &param);
                break;
            case ThreadPriority::critical:
                param.sched_priority = sched_get_priority_max(t->m_sched_policy);
                pthread_setschedparam(t->m_handle, t->m_sched_policy, &param);
                break;
            }
        }
        void wait_thread(handle_t thread)
        {
            Thread* t = (Thread*)thread;
            wait_signal(t->m_finish_signal);
        }
        RV try_wait_thread(handle_t thread)
        {
            Thread* t = (Thread*)thread;
            try_wait_signal(t->m_finish_signal);
        }
        void detach_thread(handle_t thread)
        {
            Thread* t = (Thread*)thread;
            pthread_detach(t->m_handle);
            t->m_detached = true;
        }
        handle_t get_current_thread()
        {
            return tls_current_thread;
        }
        u32 get_current_thread_id()
        {
            return (u32)pthread_getthreadid_np();
        }
        void sleep(u32 time_milliseconds)
        {
            ::usleep(time_milliseconds * 1000);
        }
        void fast_sleep(u32 time_microseconds)
        {
			u64 t = get_ticks();
			f64 tps = get_ticks_per_second();

			u64 endTime = t + (u64)(tps * time_microseconds / 1000000);
			if (time_microseconds > 4000) //4ms
			{
				::usleep(time_microseconds);
			}
			t = get_ticks();
			while ((u64)(t) < endTime)
			{
				yield_current_thread();
				yield_current_thread();
				yield_current_thread();
				yield_current_thread();
			}
        }
        void yield_current_thread()
        {
            ::sched_yield();
        }
        R<handle_t> convert_thread_to_fiber()
        {
			Thread* t = tls_current_thread;
			if (t->m_native_fiber)
			{
                return R<handle_t>::success(t->m_native_fiber);
			}
			Fiber* f = memnew<Fiber>();
			int r = getcontext(&f->m_context);
			if (r != 0)
			{
                memdelete(f);
				return BasicError::bad_system_call();
			}
			t->m_native_fiber = f;
			t->m_current_fiber = f;
            return R<handle_t>::success(f);
        }
        RV convert_fiber_to_thread()
        {
            Thread* t = tls_current_thread;
			if (!t->m_native_fiber)
			{
				return BasicError::bad_calling_time();
			}
            memdelete(t->m_native_fiber);
            t->m_native_fiber = nullptr;
            return RV();
        }
        R<handle_t> new_fiber(thread_callback_func_t* callback, void* params, usize stack_size)
        {
			luassert(callback);
			if (!stack_size)
			{
				stack_size = (u32)1_mb;
			}
			// Allocate stack.
			void* stack = memalloc(stack_size);
			Fiber* f = memnew<Fiber>();
            f->m_stack = stack;
            getcontext(&(f->m_context));
            f->m_context.uc_stack.ss_sp = (char*)stack;
            f->m_context.uc_stack.ss_size = stack_size;
            f->m_context.uc_stack.ss_flags = 0;
			makecontext(&(f->m_context), (void(*)(void))(&posix_uctx_main), 2, (u32)(usize)f, (u32)((usize)(f) >> 32));
			f->m_func = callback;
            f->m_params = params;
            return R<handle_t>::success(f);
        }
        void delete_fiber(handle_t fiber)
        {
            Fiber* f = (Fiber*)fiber;
            memdelete(f->m_stack);
            memdelete(f);
        }
        void switch_to_fiber(handle_t fiber)
        {
            Thread* t = tls_current_thread;
            lucheck(t->m_native_fiber);
            Fiber* new_f = (Fiber*)fiber;
            Fiber* old_f = t->m_current_fiber;
            t->m_current_fiber = new_f;
            swapcontext(&(old_f->m_context), &(new_f->m_context));
        }
        R<handle_t> tls_alloc(tls_destructor* destructor)
		{
            pthread_key_t key;
			int r = pthread_key_create(&key, destructor);
			if (r)
			{
                if (r == EAGAIN)
                {
                    return BasicError::overflow();
                }
                if (r == ENOMEM)
                {
                    return BasicError::bad_memory_alloc();
                }
                return BasicError::bad_memory_alloc();
			}
			return R<handle_t>::success((handle_t)key);
        }
        void tls_free(handle_t handle)
        {
            pthread_key_t key = (pthread_key_t)handle;
            pthread_key_delete(key);
        }
        RV tls_set(handle_t handle, void* ptr)
        {
            pthread_key_t key = (pthread_key_t)handle;
            int r = pthread_setspecific(key, ptr);
            if (r)
            {
                if (r == EINVAL)
                {
                    return BasicError::bad_arguments();
                }
                if (r == ENOMEM)
                {
                    return BasicError::bad_memory_alloc();
                }
                return BasicError::bad_system_call();
            }
            return RV();
        }
        void* tls_get(handle_t handle)
        {
            pthread_key_t key = (pthread_key_t)handle;
            void* k = pthread_getspecific(key);
            return k;
        }
    }
}

#endif
