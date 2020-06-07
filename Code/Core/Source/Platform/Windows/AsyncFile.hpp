// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AsyncFile.hpp
* @author JXMaster
* @date 2019/9/29
*/
#pragma once
#include "../../CoreHeader.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
namespace luna
{
	namespace win
	{
		class AsyncFileRequest;
		class AsyncFile : public IAsyncFile
		{
		public:
			lucid("{5046db58-0f35-463f-b2c9-815702c312d9}");
			luiimpl(AsyncFile, IAsyncFile, IObject);
			

			HANDLE m_file;

			AsyncFile(IAllocator* alloc) :
				luibind(alloc),
				m_file(INVALID_HANDLE_VALUE) {}
			~AsyncFile()
			{
				if (m_file != INVALID_HANDLE_VALUE)
				{
					::CloseHandle(m_file);
					m_file = INVALID_HANDLE_VALUE;
				}
			}

			virtual RP<IAsyncFileRequest> read(void* buffer, uint32 size, uint64 file_offset) override;
			virtual RP<IAsyncFileRequest> write(const void* buffer, uint32 size, uint64 file_offset) override;
			virtual R<uint64> size() override;
		};

		class AsyncFileRequest : public IAsyncFileRequest
		{
		public:
			lucid("{24f44c7b-88bd-4bbe-9e20-63e7a9cd3775}");

			OVERLAPPED m_ol;
			HANDLE m_event;
			P<AsyncFile> m_file;

			luiimpl(AsyncFileRequest, IAsyncFileRequest, IWaitable, IObject);
			

			AsyncFileRequest(AsyncFile* f, IAllocator* alloc) :
				luibind(alloc),
				m_file(f),
				m_event(NULL) {}
			~AsyncFileRequest()
			{
				::WaitForSingleObject(m_event, INFINITE);
				::CloseHandle(m_event);
				m_event = NULL;
			}
			virtual R<uint32> transferred_size() override;
			virtual void wait() override;
			virtual RV try_wait() override;
		};
	}
}

#endif