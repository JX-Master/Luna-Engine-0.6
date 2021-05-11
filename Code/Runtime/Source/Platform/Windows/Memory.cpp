// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Memory.cpp
* @author JXMaster
* @date 2020/7/28
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_WINDOWS

#include <Runtime/Assert.hpp>
#include "MiniWin.hpp"
#include "../../OS.hpp"
#include <Runtime/Result.hpp>

namespace Luna
{
	namespace OS
	{
		R<void*> virtual_reserve(void* address, usize size)
		{
			void* r = ::VirtualAlloc(address, size, MEM_RESERVE, PAGE_NOACCESS);
			if (!r)
			{
				return BasicError::bad_system_call();
			}
			return r;
		}
		RV virtual_commit(void* address, usize size, EPageProtection protection)
		{
			DWORD prot;
			switch (protection)
			{
			case EPageProtection::no_access:
				prot = PAGE_NOACCESS;
				break;
			case EPageProtection::read:
				prot = PAGE_READONLY;
				break;
			case EPageProtection::read_write:
				prot = PAGE_READWRITE;
				break;
			case EPageProtection::execute:
				prot = PAGE_EXECUTE;
				break;
			case EPageProtection::execute_read:
				prot = PAGE_EXECUTE_READ;
				break;
			case EPageProtection::execute_read_write:
				prot = PAGE_READWRITE;
				break;
			default:
				lupanic();
				break;
			}
			void* r = ::VirtualAlloc(address, size, MEM_COMMIT, prot);
			if (!r)
			{
				return BasicError::bad_system_call();
			}
			return RV();
		}
		RV virtual_decommit(void* address, usize size)
		{
			if (::VirtualFree(address, size, MEM_DECOMMIT))
			{
				return RV();
			}
			return BasicError::bad_system_call();
		}
		RV virtual_release(void* address, usize size)
		{
			if (::VirtualFree(address, 0, MEM_RELEASE))
			{
				return RV();
			}
			return BasicError::bad_system_call();
		}
		RV virtual_protect(void* address, usize size, EPageProtection protection)
		{
			DWORD prot;
			switch (protection)
			{
			case EPageProtection::no_access:
				prot = PAGE_NOACCESS;
				break;
			case EPageProtection::read:
				prot = PAGE_READONLY;
				break;
			case EPageProtection::read_write:
				prot = PAGE_READWRITE;
				break;
			case EPageProtection::execute:
				prot = PAGE_EXECUTE;
				break;
			case EPageProtection::execute_read:
				prot = PAGE_EXECUTE_READ;
				break;
			case EPageProtection::execute_read_write:
				prot = PAGE_READWRITE;
				break;
			default:
				lupanic();
				break;
			}
			DWORD old_prot;
			if (::VirtualProtect(address, size, prot, &old_prot))
			{
				return RV();
			}
			return BasicError::bad_system_call();
		}
		void* memalloc(usize size, usize alignment /* = 0 */)
		{
			return (alignment > max_align) ? _aligned_malloc(size, alignment) : malloc(size);
		}
		void memfree(void* ptr, usize alignment /* = 0 */)
		{
			if (alignment > max_align)
			{
				_aligned_free(ptr);
			}
			else
			{
				free(ptr);
			}
		}
		void* memrealloc(void* ptr, usize size, usize alignment /* = 0 */)
		{
			return (alignment > max_align) ? _aligned_realloc(ptr, size, alignment) : realloc(ptr, size);
		}
		usize memsize(void* ptr, usize alignment /* = 0 */)
		{
			if (!ptr) return 0;
			return (alignment > max_align) ? _aligned_msize(ptr, alignment, 0) : _msize(ptr);
		}
	}
}

#endif