// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AsyncFile.cpp
* @author JXMaster
* @date 2019/9/29
*/
#include "AsyncFile.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
#include "Heap.hpp"
namespace luna
{
	namespace win
	{
		RP<IAsyncFileRequest> AsyncFile::read(void* buffer, uint32 size, uint64 file_offset)
		{
			luassert(m_file != INVALID_HANDLE_VALUE);
			IAllocator* alloc = m_rc.m_alloc;
			P<AsyncFileRequest> re;
			re.attach(alloc->new_obj<AsyncFileRequest>(this, alloc));
			re->m_event = ::CreateEventA(NULL, TRUE, FALSE, NULL);
			if (!re->m_event)
			{
				re->release();
				re = nullptr;
				return { nullptr, e_bad_system_call };
			}
			re->m_ol.hEvent = re->m_event;
			re->m_ol.Offset = (DWORD)(file_offset & 0xffffffff);
			re->m_ol.OffsetHigh = (DWORD)((file_offset >> 32) & 0xffffffff);
			if (!::ReadFile(m_file, buffer, size, NULL, &(re->m_ol)))
			{
				DWORD err = ::GetLastError();
				if (err == ERROR_IO_PENDING)
				{
					return { re, s_ok };
				}
				else
				{
					re = nullptr;
					return { nullptr, e_bad_system_call };
				}
			}
			return { re, s_ok };
		}
		RP<IAsyncFileRequest> AsyncFile::write(const void* buffer, uint32 size, uint64 file_offset)
		{
			luassert(m_file != INVALID_HANDLE_VALUE);
			IAllocator* alloc = m_rc.m_alloc;
			P<AsyncFileRequest> re;
			re.attach(alloc->new_obj<AsyncFileRequest>(this, alloc));
			re->m_event = ::CreateEventA(NULL, TRUE, FALSE, NULL);
			if (!re->m_event)
			{
				re->release();
				re = nullptr;
				return { nullptr, e_bad_system_call };
			}
			re->m_ol.hEvent = re->m_event;
			re->m_ol.Offset = (DWORD)(file_offset & 0xffffffff);
			re->m_ol.OffsetHigh = (DWORD)((file_offset >> 32) & 0xffffffff);
			if (!::WriteFile(m_file, buffer, size, NULL, &(re->m_ol)))
			{
				DWORD err = ::GetLastError();
				if (err == ERROR_IO_PENDING)
				{
					return { re, s_ok };
				}
				else
				{
					re = nullptr;
					return { nullptr, e_bad_system_call };
				}
			}
			return { re, s_ok };
		}
		R<uint64> AsyncFile::size()
		{
			luassert(m_file != INVALID_HANDLE_VALUE);
			LARGE_INTEGER size;
			if (::GetFileSizeEx(m_file, &size))
			{
				return R<uint64>::success(size.QuadPart, s_ok);
			}
			return R<uint64>::failure(e_bad_system_call);
		}

		R<uint32> AsyncFileRequest::transferred_size()
		{
			DWORD sz;
			if (::GetOverlappedResult(m_file->m_file, &m_ol, &sz, FALSE))
			{
				return R<uint32>::success(sz, s_ok);
			}
			DWORD err = ::GetLastError();
			if (err == ERROR_IO_PENDING)
			{
				return R<uint32>::failure(e_pending);
			}
			return R<uint32>::failure(e_bad_system_call);
		}

		void AsyncFileRequest::wait()
		{
			DWORD dw = ::WaitForSingleObject(m_event, INFINITE);
			if (dw != WAIT_OBJECT_0)
			{
				lupanic_msg_always("WaitForSingleObject failed.");
			}
		}

		RV AsyncFileRequest::try_wait()
		{
			DWORD dw = ::WaitForSingleObject(m_event, 0);
			if (dw == WAIT_OBJECT_0)
			{
				return s_ok;
			}
			if (dw == WAIT_TIMEOUT)
			{
				return e_pending;
			}
			return e_failure;
		}
	}
}

#endif