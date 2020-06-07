// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformMemory.cpp
* @author JXMaster
* @date 2019/12/26
*/
#include "../../Platform.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
#include "Heap.hpp"
namespace luna
{
	namespace platform
	{
		using namespace win;

		RP<IHeap> new_heap(EHeapCreationFlag heap_creation_flags, size_t initial_size, size_t maximum_size, const char* name)
		{
			DWORD heap_flags = 0;
			if ((heap_creation_flags & EHeapCreationFlag::enable_execute) != EHeapCreationFlag::none)
			{
				heap_flags |= HEAP_CREATE_ENABLE_EXECUTE;
			}
			if ((heap_creation_flags & EHeapCreationFlag::no_thread_safe) != EHeapCreationFlag::none)
			{
				heap_flags |= HEAP_NO_SERIALIZE;
			}
			HANDLE handle = ::HeapCreate(heap_flags, initial_size, maximum_size);
			if (handle)
			{
				if ((heap_creation_flags & EHeapCreationFlag::no_thread_safe) != EHeapCreationFlag::none)
				{
					P<IHeap> heap;
					heap.attach(init_non_thread_safe_heap(handle));
					return heap;
				}
				else
				{
					P<IHeap> heap;
					heap.attach(init_heap(handle));
					return heap;
				}
			}
			return { nullptr, e_bad_system_call };
		}
		void* virtual_reserve(void* address, size_t size)
		{
			return ::VirtualAlloc(address, size, MEM_RESERVE, PAGE_NOACCESS);
		}
		void* virtual_commit(void* address, size_t size, EPageProtection protection)
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
			return ::VirtualAlloc(address, size, MEM_COMMIT, prot);
		}
		RV virtual_decommit(void* address, size_t size)
		{
			if (::VirtualFree(address, size, MEM_DECOMMIT))
			{
				return s_ok;
			}
			return e_failure;
		}
		RV virtual_release(void* address)
		{
			if (::VirtualFree(address, 0, MEM_RELEASE))
			{
				return s_ok;
			}
			return e_failure;
		}
		RV virtual_protect(void* address, size_t size, EPageProtection new_protection, EPageProtection* old_protection)
		{
			DWORD prot;
			switch (new_protection)
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
				if (old_protection)
				{
					switch (old_prot)
					{
					case PAGE_NOACCESS:
						*old_protection = EPageProtection::no_access;
						break;
					case PAGE_READONLY:
						*old_protection = EPageProtection::read;
						break;
					case PAGE_READWRITE:
						*old_protection = EPageProtection::read_write;
						break;
					case PAGE_EXECUTE:
						*old_protection = EPageProtection::execute;
						break;
					case PAGE_EXECUTE_READ:
						*old_protection = EPageProtection::execute_read;
						break;
					case PAGE_EXECUTE_READWRITE:
						*old_protection = EPageProtection::execute_read_write;
						break;
					default:
						lupanic();
						break;
					}
				}
				return s_ok;
			}
			return e_failure;
		}
	}
}

#endif