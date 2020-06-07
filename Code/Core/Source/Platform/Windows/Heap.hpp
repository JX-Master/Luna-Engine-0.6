// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Heap.hpp
* @author JXMaster
* @date 2019/8/7
*/
#pragma once
#include "../../CoreHeader.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include <Base/TSAssert.hpp>

namespace luna
{
	namespace win
	{
		//! Custom heap.
		class ThreadSafeHeap : public IHeap
		{
		public:
			lucid("{32b48476-6708-42d4-b334-71e6e50a3827}");
			luqself(ThreadSafeHeap, IHeap, IAllocator, IObject);
			

			HANDLE m_handle;
			size_t m_allocated;
			ref_count_t m_ref_count;

			char m_name[64];

			ThreadSafeHeap(HANDLE handle) :
				m_handle(handle),
				m_allocated(0),
				m_ref_count(1)
			{
				m_name[0] = 0;
			}

			void delete_self();

			virtual ref_count_t get_ref() override
			{
				return m_ref_count;
			}
			virtual ref_count_t add_ref() override
			{
				return atom_inc_i32(&m_ref_count);
			}
			virtual RV add_ref_if_not_expired() override
			{
				add_ref();
				return s_ok;
			}
			virtual ref_count_t release() override
			{
				ref_count_t r = atom_dec_i32(&m_ref_count);
				if (!r)
				{
					delete_self();
				}
				return r;
			}
			virtual ref_count_t get_weak_ref() override
			{
				return m_ref_count;
			}
			virtual ref_count_t add_weak_ref() override
			{
				return atom_inc_i32(&m_ref_count);
			}
			virtual ref_count_t release_weak() override
			{
				ref_count_t r = atom_dec_i32(&m_ref_count);
				if (!r)
				{
					delete_self();
				}
				return r;
			}
			virtual bool expired() override
			{
				return false;
			}
			virtual void* allocate(size_t s) override;
			virtual void free(void* ptr) override;
			virtual void* reallocate(void* old_ptr, size_t new_size) override;
			virtual size_t size(void* ptr) override;
			virtual void* allocate_aligned(size_t size, size_t alignment) override;
			virtual void free_aligned(void* ptr, size_t alignment) override;
			virtual void* reallocate_aligned(void* old_ptr, size_t old_alignment, size_t new_size, size_t new_alignment) override;
			virtual size_t size_aligned(void* ptr, size_t alignment) override;
			virtual const char* name() override;
			virtual void set_name(const char* name) override;
			virtual size_t allocated() override
			{
				return m_allocated;
			}
		};
		ThreadSafeHeap* init_heap(HANDLE h);

		class NonThreadSafeHeap : public IHeap
		{
		public:
			lucid("{09714273-c95f-4c15-a2fc-72d7bd3e9ea4}");
			luqself(NonThreadSafeHeap, IHeap, IAllocator, IObject);
			lutsassert_lock();

			HANDLE m_handle;
			size_t m_allocated;
			ref_count_t m_ref_count;

			char m_name[64];

			NonThreadSafeHeap(HANDLE handle) :
				m_handle(handle),
				m_allocated(0),
				m_ref_count(1)
			{
				m_name[0] = 0;
			}

			void delete_self();

			virtual ref_count_t get_ref() override
			{
				return m_ref_count;
			}
			virtual ref_count_t add_ref() override
			{
				return atom_inc_i32(&m_ref_count);
			}
			virtual RV add_ref_if_not_expired() override
			{
				add_ref();
				return s_ok;
			}
			virtual ref_count_t release() override
			{
				ref_count_t r = atom_dec_i32(&m_ref_count);
				if (!r)
				{
					delete_self();
				}
				return r;
			}
			virtual ref_count_t get_weak_ref() override
			{
				return m_ref_count;
			}
			virtual ref_count_t add_weak_ref() override
			{
				return atom_inc_i32(&m_ref_count);
			}
			virtual ref_count_t release_weak() override
			{
				ref_count_t r = atom_dec_i32(&m_ref_count);
				if (!r)
				{
					delete_self();
				}
				return r;
			}
			virtual bool expired() override
			{
				return false;
			}
			virtual void* allocate(size_t s) override;
			virtual void free(void* ptr) override;
			virtual void* reallocate(void* old_ptr, size_t new_size) override;
			virtual size_t size(void* ptr) override;
			virtual void* allocate_aligned(size_t size, size_t alignment) override;
			virtual void free_aligned(void* ptr, size_t alignment) override;
			virtual void* reallocate_aligned(void* old_ptr, size_t old_alignment, size_t new_size, size_t new_alignment) override;
			virtual size_t size_aligned(void* ptr, size_t alignment) override;
			virtual const char* name() override;
			virtual void set_name(const char* name) override;
			virtual size_t allocated() override
			{
				return m_allocated;
			}
		};
		NonThreadSafeHeap* init_non_thread_safe_heap(HANDLE h);
	}
}

#endif