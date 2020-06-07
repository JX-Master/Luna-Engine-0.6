// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Heap.cpp
* @author JXMaster
* @date 2019/8/7
*/
#include "Heap.hpp"
#include "../../Platform.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
namespace luna
{
	namespace platform
	{
		class PlatformHeap final : public IHeap
		{
		public:
			lucid("{769f2914-b29e-4b99-8222-dd466d8ad8bd}");
			luiimpl_static(PlatformHeap, IHeap, IAllocator, IObject);
			

			size_t m_allocated;
			char m_name[64];

			PlatformHeap() :
				m_allocated(0)
			{
				strcpy_s(m_name, "Global");
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
			virtual size_t allocated() override;
		};

		void* PlatformHeap::allocate(size_t s)
		{
			void* v = malloc(s);
			if (v)
			{
				atom_add_sizet(&m_allocated, _msize(v));
			}
			return v;
		}
		void PlatformHeap::free(void* ptr)
		{
			if (ptr)
			{
				atom_add_sizet(&m_allocated, -(ptrdiff_t)(_msize(ptr)));
			}
			::free(ptr);
		}
		void* PlatformHeap::reallocate(void* old_ptr, size_t new_size)
		{
			ptrdiff_t old_size = old_ptr ? (ptrdiff_t)_msize(old_ptr) : 0;
			void* new_ptr = realloc(old_ptr, new_size);
			ptrdiff_t new_sz = new_ptr ? (ptrdiff_t)_msize(new_ptr) : 0;
			atom_add_sizet(&m_allocated, new_sz - old_size);
			return new_ptr;
		}
		size_t PlatformHeap::size(void* ptr)
		{
			return _msize(ptr);
		}
		void* PlatformHeap::allocate_aligned(size_t sz, size_t alignment)
		{
			void* v = _aligned_malloc(sz, alignment);
			if (v)
			{
				atom_add_sizet(&m_allocated, _aligned_msize(v, alignment, 0));
			}
			return v;
		}
		void PlatformHeap::free_aligned(void* ptr, size_t alignment)
		{
			if (ptr)
			{
				atom_add_sizet(&m_allocated, -(ptrdiff_t)(_aligned_msize(ptr, alignment, 0)));
			}
			_aligned_free(ptr);
		}
		void* PlatformHeap::reallocate_aligned(void* old_ptr, size_t old_alignment, size_t new_size, size_t new_alignment)
		{
			ptrdiff_t old_size = old_ptr ? (ptrdiff_t)_aligned_msize(old_ptr, old_alignment, 0) : 0;
			void* new_ptr = _aligned_realloc(old_ptr, new_size, new_alignment);
			ptrdiff_t new_sz = new_ptr ? (ptrdiff_t)_aligned_msize(new_ptr, new_alignment, 0) : 0;
			atom_add_sizet(&m_allocated, new_sz - old_size);
			return new_ptr;
		}
		size_t PlatformHeap::size_aligned(void* ptr, size_t alignment)
		{
			return _aligned_msize(ptr, alignment, 0);
		}
		const char* PlatformHeap::name()
		{
			return m_name;
		}
		void PlatformHeap::set_name(const char* name)
		{
			strcpy_s(m_name, name);
		}
		size_t PlatformHeap::allocated()
		{
			return m_allocated;
		}

		Unconstructed<PlatformHeap> g_platform_heap;

		void platform_heap_init()
		{
			g_platform_heap.construct();
		}

		IHeap* get_global_heap()
		{
			return &(g_platform_heap.get());
		}
	}

	namespace win
	{
		void ThreadSafeHeap::delete_self()
		{
			HANDLE h = m_handle;
			ThreadSafeHeap* heap = this;
			heap->~ThreadSafeHeap();
			::HeapFree(h, 0, heap);
			::HeapDestroy(h);
		}
		void* ThreadSafeHeap::allocate(size_t s)
		{
			void* r = ::HeapAlloc(m_handle, 0, s);
			if (!r)
			{
				return nullptr;
			}
			atom_add_sizet(&m_allocated, ::HeapSize(m_handle, 0, r));
			return r;
		}
		void ThreadSafeHeap::free(void* ptr)
		{
			if (!ptr)
			{
				return;
			}
			atom_add_sizet(&m_allocated, -(ptrdiff_t)(::HeapSize(m_handle, 0, ptr)));
			::HeapFree(m_handle, 0, ptr);
		}
		void* ThreadSafeHeap::reallocate(void* old_ptr, size_t new_size)
		{
			size_t old_allodated_size = old_ptr ? ::HeapSize(m_handle, 0, old_ptr) : 0;
			void* new_ptr = ::HeapReAlloc(m_handle, 0, old_ptr, new_size);
			size_t new_allodated_size = new_ptr ? ::HeapSize(m_handle, 0, new_ptr) : 0;
			atom_add_sizet(&m_allocated, (ptrdiff_t)new_allodated_size - (ptrdiff_t)old_allodated_size);
			return new_ptr;
		}
		size_t ThreadSafeHeap::size(void* ptr)
		{
			return ::HeapSize(m_handle, 0, ptr);
		}

		void* heap_allocate_aligned(HANDLE heap, size_t size, size_t alignment)
		{
			if (alignment <= max_align_v)
			{
				return ::HeapAlloc(heap, 0, size);
			}
			void* r = HeapAlloc(heap, 0, size + alignment);
			size_t offset = alignment - ((size_t)r % alignment);
			offset = offset ? offset : alignment;
			*((uint8*)((size_t)r + offset - 1)) = (uint8)offset;
			return (void*)((size_t)r + offset);
		}

		void heap_free_aligned(HANDLE heap, void* ptr, size_t alignment)
		{
			if (alignment <= max_align_v)
			{
				::HeapFree(heap, 0, ptr);
				return;
			}
			uint8 offset = *((uint8*)((size_t)ptr - 1));
			void* ori = (void*)((size_t)ptr - offset);
			::HeapFree(heap, 0, ori);
		}

		size_t heap_size_aligned(HANDLE heap, const void* ptr, size_t alignment)
		{
			if (alignment <= max_align_v)
			{
				return ::HeapSize(heap, 0, ptr);
			}
			uint8 offset = *((uint8*)((size_t)ptr - 1));
			void* ori = (void*)((size_t)ptr - offset);
			return ::HeapSize(heap, 0, ori);
		}

		void* heap_reallocate_aligned(HANDLE heap, void* old_ptr, size_t old_alignment, size_t new_size, size_t new_alignment)
		{
			void* r = heap_allocate_aligned(heap, new_size, new_alignment);
			if (!r)
			{
				return nullptr;
			}
			if (old_ptr)
			{
				if (old_alignment <= max_align_v)
				{
					memcpy(r, old_ptr, min((size_t)::HeapSize(heap, 0, old_ptr), new_size));
					::HeapFree(heap, 0, old_ptr);
				}
				else
				{
					memcpy(r, old_ptr, min(heap_size_aligned(heap, old_ptr, old_alignment), new_size));
					heap_free_aligned(heap, old_ptr, old_alignment);
				}
			}
			return r;
		}

		void* ThreadSafeHeap::allocate_aligned(size_t s, size_t alignment)
		{
			void* r = heap_allocate_aligned(m_handle, s, alignment);
			if (r)
			{
				atom_add_sizet(&m_allocated, heap_size_aligned(m_handle, r, alignment));
			}
			return r;
		}
		void ThreadSafeHeap::free_aligned(void* addr, size_t alignment)
		{
			if (addr)
			{
				atom_add_sizet(&m_allocated, -(ptrdiff_t)heap_size_aligned(m_handle, addr, alignment));
			}
			heap_free_aligned(m_handle, addr, alignment);
		}
		void* ThreadSafeHeap::reallocate_aligned(void* old_ptr, size_t old_alignment, size_t new_size, size_t new_alignment)
		{
			size_t old_allocated_size = old_ptr ? heap_size_aligned(m_handle, old_ptr, old_alignment) : 0;
			void* r =  heap_reallocate_aligned(m_handle, old_ptr, old_alignment, new_size, new_alignment);
			size_t new_allocated_size = r ? heap_size_aligned(m_handle, r, new_alignment) : 0;
			atom_add_sizet(&m_allocated, (ptrdiff_t)new_allocated_size - (ptrdiff_t)old_allocated_size);
			return r;
		}
		size_t ThreadSafeHeap::size_aligned(void* ptr, size_t alignment)
		{
			return heap_size_aligned(m_handle, ptr, alignment);
		}

		const char* ThreadSafeHeap::name()
		{
			return m_name;
		}

		void ThreadSafeHeap::set_name(const char* name)
		{
			if (name)
			{
				strcpy_s(m_name, 63, name);
				m_name[63] = 0;
			}
		}

		ThreadSafeHeap* init_heap(HANDLE h)
		{
			ThreadSafeHeap* heap = (ThreadSafeHeap*)::HeapAlloc(h, 0, sizeof(ThreadSafeHeap));
			new (heap) ThreadSafeHeap(h);
			return heap;
		}

		void NonThreadSafeHeap::delete_self()
		{
			HANDLE h = m_handle;
			NonThreadSafeHeap* heap = this;
			heap->~NonThreadSafeHeap();
			::HeapFree(h, 0, heap);
			::HeapDestroy(h);
		}
		void* NonThreadSafeHeap::allocate(size_t s)
		{
			lutsassert();
			void* r = ::HeapAlloc(m_handle, 0, s);
			if (!r)
			{
				return nullptr;
			}
			atom_add_sizet(&m_allocated, ::HeapSize(m_handle, 0, r));
			return r;
		}
		void NonThreadSafeHeap::free(void* ptr)
		{
			lutsassert();
			if (!ptr)
			{
				return;
			}
			atom_add_sizet(&m_allocated, -(ptrdiff_t)(::HeapSize(m_handle, 0, ptr)));
			::HeapFree(m_handle, 0, ptr);
		}
		void* NonThreadSafeHeap::reallocate(void* old_ptr, size_t new_size)
		{
			lutsassert();
			size_t old_allodated_size = old_ptr ? ::HeapSize(m_handle, 0, old_ptr) : 0;
			void* new_ptr = ::HeapReAlloc(m_handle, 0, old_ptr, new_size);
			size_t new_allodated_size = new_ptr ? ::HeapSize(m_handle, 0, new_ptr) : 0;
			atom_add_sizet(&m_allocated, (ptrdiff_t)new_allodated_size - (ptrdiff_t)old_allodated_size);
			return new_ptr;
		}
		size_t NonThreadSafeHeap::size(void* ptr)
		{
			lutsassert();
			return ::HeapSize(m_handle, 0, ptr);
		}

		void* NonThreadSafeHeap::allocate_aligned(size_t s, size_t alignment)
		{
			lutsassert();
			void* r = heap_allocate_aligned(m_handle, s, alignment);
			if (r)
			{
				atom_add_sizet(&m_allocated, heap_size_aligned(m_handle, r, alignment));
			}
			return r;
		}
		void NonThreadSafeHeap::free_aligned(void* addr, size_t alignment)
		{
			lutsassert();
			if (addr)
			{
				atom_add_sizet(&m_allocated, -(ptrdiff_t)heap_size_aligned(m_handle, addr, alignment));
			}
			heap_free_aligned(m_handle, addr, alignment);
		}
		void* NonThreadSafeHeap::reallocate_aligned(void* old_ptr, size_t old_alignment, size_t new_size, size_t new_alignment)
		{
			lutsassert();
			size_t old_allocated_size = old_ptr ? heap_size_aligned(m_handle, old_ptr, old_alignment) : 0;
			void* r = heap_reallocate_aligned(m_handle, old_ptr, old_alignment, new_size, new_alignment);
			size_t new_allocated_size = r ? heap_size_aligned(m_handle, r, new_alignment) : 0;
			atom_add_sizet(&m_allocated, (ptrdiff_t)new_allocated_size - (ptrdiff_t)old_allocated_size);
			return r;
		}
		size_t NonThreadSafeHeap::size_aligned(void* ptr, size_t alignment)
		{
			lutsassert();
			return heap_size_aligned(m_handle, ptr, alignment);
		}

		const char* NonThreadSafeHeap::name()
		{
			lutsassert();
			return m_name;
		}

		void NonThreadSafeHeap::set_name(const char* name)
		{
			lutsassert();
			if (name)
			{
				strcpy_s(m_name, 63, name);
				m_name[63] = 0;
			}
		}

		NonThreadSafeHeap* init_non_thread_safe_heap(HANDLE h)
		{
			NonThreadSafeHeap* heap = (NonThreadSafeHeap*)::HeapAlloc(h, 0, sizeof(NonThreadSafeHeap));
			new (heap) NonThreadSafeHeap(h);
			return heap;
		}
	}
}

#endif