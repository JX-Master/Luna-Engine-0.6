// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Memory.cpp
* @author JXMaster
* @date 2020/9/22
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#include "../../OS.hpp"
#include <sys/mman.h>
#include "Errno.hpp"
#include <Runtime/Error.hpp>
#include <pthread.h>
#include <stdlib.h>
#include <Runtime/Algorithm.hpp>

namespace Luna
{
    namespace OS
    {
        R<void*> virtual_reserve(void* address, usize size)
        {
			void* r = mmap(address, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
			if (r == MAP_FAILED)
			{
				errno_t err = errno;
				//get_error().set(e_bad_system_call, u8"virtual_reserve failed with error code %d %s", err, display_errno(err));
				return BasicError::bad_system_call();
			}
			return r;
        }
        RV virtual_commit(void* address, usize size, EPageProtection protection)
        {
			int prot;
			switch (protection) {
			case EPageProtection::no_access:
				prot = PROT_NONE;
				break;
			case EPageProtection::read:
				prot = PROT_READ;
				break;
			case EPageProtection::read_write:
				prot = PROT_READ | PROT_WRITE;
				break;
			case EPageProtection::execute:
				prot = PROT_EXEC;
				break;
			case EPageProtection::execute_read:
				prot = PROT_READ | PROT_EXEC;
				break;
			case EPageProtection::execute_read_write:
				prot = PROT_READ | PROT_WRITE | PROT_EXEC;
				break;
			default:
				lupanic();
				break;
			}
			int res = mprotect(address, size, prot);
			if (res != 0)
			{
				errno_t err = errno;
				//get_error().set(e_bad_system_call, u8"virtual_commit failed with error code %d %s", err, display_errno(err));
				return BasicError::bad_system_call();
			}
			return RV();
        }
        RV virtual_decommit(void* address, usize size)
        {
			int res = mprotect(address, size, PROT_NONE);
			if (res != 0)
			{
				errno_t err = errno;
				//get_error().set(e_bad_system_call, u8"virtual_commit failed with error code %d %s", err, display_errno(err));
				return BasicError::bad_system_call();
			}
			return RV();
        }
        RV virtual_release(void* address, usize size)
        {
			int res = munmap(address, size);
			if (res != 0)
			{
				errno_t err = errno;
				//get_error().set(e_bad_system_call, u8"virtual_release failed with error code %d %s", err, display_errno(err));
				return BasicError::bad_system_call();
			}
			return RV();
        }
        RV virtual_protect(void* address, usize size, EPageProtection protection)
        {
			int prot;
			switch (protection) {
			case EPageProtection::no_access:
				prot = PROT_NONE;
				break;
			case EPageProtection::read:
				prot = PROT_READ;
				break;
			case EPageProtection::read_write:
				prot = PROT_READ | PROT_WRITE;
				break;
			case EPageProtection::execute:
				prot = PROT_EXEC;
				break;
			case EPageProtection::execute_read:
				prot = PROT_READ | PROT_EXEC;
				break;
			case EPageProtection::execute_read_write:
				prot = PROT_READ | PROT_WRITE | PROT_EXEC;
				break;
			default:
				lupanic();
				break;
			}
			int res = mprotect(address, size, prot);
			if (res != 0)
			{
				errno_t err = errno;
				//get_error().set(e_bad_system_call, u8"virtual_protect failed with error code %d %s", err, display_errno(err));
				return BasicError::bad_system_call();
			}
        }
        void* memalloc(usize size, usize alignment /* = 0 */)
        {
            if (!size) return nullptr;
            if (alignment <= max_align) return malloc(size);
            usize ptr = (usize)malloc(size + alignment);
            usize aligned_ptr = align_upper(ptr + 1, alignment);
            isize offset = aligned_ptr - ptr;
            *((isize*)(aligned_ptr)-1) = offset;
            return (void*)aligned_ptr;
        }
        void memfree(void* ptr, usize alignment /* = 0 */)
        {
            if (!ptr) return;
            if (alignment <= max_align)
            {
                free(ptr);
            }
            isize offset = *(((isize*)ptr) - 1);
            void* origin_ptr = (void*)(((usize)ptr) - offset);
            free(origin_ptr);
        }
        void* memrealloc(void* ptr, usize size, usize alignment /* = 0 */)
        {
            if (!size)
            {
                memfree(ptr, alignment);
                return nullptr;
            }
            void* new_ptr = memalloc(size, alignment);
            if (ptr && new_ptr)
            {
                usize old_sz = memsize(ptr, alignment);
                memcpy(new_ptr, ptr, min(old_sz, size));
                memfree(ptr, alignment);
            }
            return new_ptr;
        }
        usize memsize(void* ptr, usize alignment /* = 0 */)
        {
            if (!ptr) return 0;
			if (alignment <= max_align)
			{
				return malloc_usable_size(ptr);
			}
			isize offset = *(((isize*)ptr) - 1);
			void* origin_ptr = (void*)(((usize)ptr) - offset);
			return malloc_usable_size(origin_ptr) - offset;
        }
    }
}

#endif
