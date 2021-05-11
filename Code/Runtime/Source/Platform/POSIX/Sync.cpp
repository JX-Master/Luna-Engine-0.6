// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Sync.cpp
* @author JXMaster
* @date 2020/9/22
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#include "../../OS.hpp"
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <Runtime/Assert.hpp>

namespace Luna
{
	namespace OS
	{
		struct Signal
		{
			pthread_cond_t m_cond;
			pthread_mutex_t m_mutex;
			volatile bool m_signaled;
			volatile bool m_manual_reset;

			Signal()
			{
				m_signaled = false;
				luna_eval_and_assert_equal(pthread_mutex_init(&m_mutex, NULL), 0);
				luna_eval_and_assert_equal(pthread_cond_init(&m_cond, NULL), 0);
			}
			~Signal()
			{
				pthread_cond_destroy(&m_cond);
				pthread_mutex_destroy(&m_mutex);
			}
		};
		handle_t new_signal(bool manual_reset)
		{
			Signal* sig = memnew<Signal>();
			sig->m_manual_reset = manual_reset;
			return sig;
		}
		void delete_signal(handle_t sig)
		{
			Signal* o = (Signal*)sig;
			memdelete(o);
		}
		void wait_signal(handle_t sig)
		{
			Signal* o = (Signal*)sig;
			luna_eval_and_assert_equal(pthread_mutex_lock(&o->m_mutex), 0);
			while (!o->m_signaled)
			{
				luna_eval_and_assert_equal(pthread_cond_wait(&o->m_cond, &o->m_mutex), 0);
			}
			if (!o->m_manual_reset)
			{
				o->m_signaled = false;
			}
			luna_eval_and_assert_equal(pthread_mutex_unlock(&o->m_mutex), 0);
		}
		RV try_wait_signal(handle_t sig)
		{
			Signal* o = (Signal*)sig;
			int rc = 0;
			struct timespec abstime;
			struct timeval tv;
			gettimeofday(&tv, NULL);
			abstime.tv_sec = tv.tv_sec;
			abstime.tv_nsec = tv.tv_usec * 1000;
			if (abstime.tv_nsec >= 1000000000)
			{
				abstime.tv_nsec -= 1000000000;
				abstime.tv_sec++;
			}
			luna_eval_and_assert_equal(pthread_mutex_lock(&o->m_mutex), 0);
			while (!o->m_signaled)
			{
				rc = pthread_cond_timedwait(&o->m_cond, &o->m_mutex, &abstime);
				if (rc != 0)
				{
					break;
				}
			}
			if (rc == 0 && !o->m_manual_reset)
			{
				o->m_signaled = false;
			}
			luna_eval_and_assert_equal(pthread_mutex_unlock(&o->m_mutex), 0);
			return rc == 0 ? RV() : BasicError::timeout();
		}
		void trigger_signal(handle_t sig)
		{
			Signal* o = (Signal*)sig;
			luna_eval_and_assert_equal(pthread_mutex_lock(&o->m_mutex), 0);
			o->m_signaled = true;
			if (o->m_manual_reset)
			{
				luna_eval_and_assert_equal(pthread_cond_broadcast(&o->m_cond), 0);
			}
			else
			{
				luna_eval_and_assert_equal(pthread_cond_signal(&o->m_cond), 0);
			}
			luna_eval_and_assert_equal(pthread_mutex_unlock(&o->m_mutex), 0);
		}
		void reset_signal(handle_t sig)
		{
			Signal* o = (Signal*)sig;
			luna_eval_and_assert_equal(pthread_mutex_lock(&o->m_mutex), 0);
			o->m_signaled = false;
			luna_eval_and_assert_equal(pthread_mutex_unlock(&o->m_mutex), 0);
		}
		handle_t new_mutex()
		{
			pthread_mutex_t* mtx = memnew<pthread_mutex_t>();
			pthread_mutexattr_t attr;
			luna_eval_and_assert_equal(pthread_mutexattr_init(&attr), 0);
			luna_eval_and_assert_equal(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE), 0);
			luna_eval_and_assert_equal(pthread_mutex_init(mtx, &attr), 0);
			pthread_mutexattr_destroy(&attr);
			return mtx;
		}
		void delete_mutex(handle_t mtx)
		{
			pthread_mutex_t* o = (pthread_mutex_t*)mtx;
			pthread_mutex_destroy(o);
			memdelete(o);
		}
		void lock_mutex(handle_t mtx)
		{
			pthread_mutex_t* o = (pthread_mutex_t*)mtx;
			luna_eval_and_assert_equal(pthread_mutex_lock(o), 0);
		}
		RV try_lock_mutex(handle_t mtx)
		{
			pthread_mutex_t* o = (pthread_mutex_t*)mtx;
			int rv = pthread_mutex_trylock(o);
			return (rv == 0) ? RV() : BasicError::timeout();
		}
		void unlock_mutex(handle_t mtx)
		{
			pthread_mutex_t* o = (pthread_mutex_t*)mtx;
			luna_eval_and_assert_equal(pthread_mutex_unlock(o), 0);
		}
		struct Semaphore
		{
			pthread_mutex_t m_mutex;
			pthread_cond_t m_cond;
			i32 m_counter;
			i32 m_max_count;

			Semaphore(i32 initial_count, i32 max_count)
			{
				m_max_count = max_count;
				m_counter = initial_count;
				luna_eval_and_assert_equal(pthread_mutex_init(&m_mutex, NULL), 0);
				luna_eval_and_assert_equal(pthread_cond_init(&m_cond, NULL), 0);
			}
			~Semaphore()
			{
				pthread_cond_destroy(&m_cond);
				pthread_mutex_destroy(&m_mutex);
			}
		};
		handle_t new_semaphore(i32 initial_count, i32 max_count)
		{
			Semaphore* sema = memnew<Semaphore>(initial_count, max_count);
			return h;
		}
		void delete_semaphore(handle_t sema)
		{
			Semaphore* o = (Semaphore*)sema;
			memdelete(o);
		}
		void acquire_semaphore(handle_t sema)
		{
			Semaphore* o = (Semaphore*)sema;
			luna_eval_and_assert_equal(pthread_mutex_lock(&o->m_mutex), 0);
			if (o->m_counter > 0)
			{
				atom_dec_i32(&o->m_counter);
				luna_eval_and_assert_equal(pthread_mutex_unlock(&o->m_mutex), 0);
			}
			atom_dec_i32(&o->m_counter);
			luna_eval_and_assert_equal(pthread_cond_wait(&o->m_cond, &o->m_mutex), 0);
			atom_inc_i32(&o->m_counter);
			luna_eval_and_assert_equal(pthread_mutex_unlock(&o->m_mutex), 0);
		}
		RV try_acquire_semaphore(handle_t sema)
		{
			Semaphore* o = (Semaphore*)sema;
			luna_eval_and_assert_equal(pthread_mutex_lock(&o->m_mutex), 0);
			if (o->m_counter > 0)
			{
				atom_dec_i32(&o->m_counter);
				luna_eval_and_assert_equal(pthread_mutex_unlock(&o->m_mutex), 0);
				return RV();
			}
			luna_eval_and_assert_equal(pthread_mutex_unlock(&o->m_mutex), 0);
			return BasicError::timeout();
		}
		void release_semaphore(handle_t sema)
		{
			Semaphore* o = (Semaphore*)sema;
			luna_eval_and_assert_equal(pthread_mutex_lock(&o->m_mutex), 0);
			if (o->m_counter < o->m_max_count)
			{
				if (o->m_counter < 0)
				{
					luna_eval_and_assert_equal(pthread_cond_signal(&o->m_cond), 0);
				}
				else
				{
					atom_inc_i32(&o->m_counter);
				}
			}
			luna_eval_and_assert_equal(pthread_mutex_unlock(&o->m_mutex), 0);
		}
	}
}

#endif
