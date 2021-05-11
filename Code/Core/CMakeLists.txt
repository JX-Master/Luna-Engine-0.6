cmake_minimum_required (VERSION 3.3)

set(SRC_FILES
        Core.hpp
        Error.hpp
        IMemoryStream.hpp
        IMutex.hpp
        Interface.hpp
        IObject.hpp
        ISemaphore.hpp
        ISignal.hpp
        IStream.hpp
        IWaitable.hpp
        IThread.hpp
        IFiber.hpp
        IDispatchQueue.hpp
        IRunnable.hpp
        IRandom.hpp
        Variant.hpp
        IEncoder.hpp
        IDecoder.hpp
        IFile.hpp
        IFileIterator.hpp
        IFileSystem.hpp
        ISerializable.hpp
        
        Source/Core.cpp
        Source/MemoryStream.cpp
        Source/MemoryStream.hpp
        Source/Mutex.hpp
        Source/Semaphore.hpp
        Source/Signal.hpp
        Source/UpdateLoop.cpp
        Source/Thread.hpp
        Source/Thread.cpp
        Source/Fiber.hpp
        Source/ThreadPool.hpp
        Source/ThreadPool.cpp
        Source/DispatchQueue.hpp
        Source/DispatchQueue.cpp
        Source/Random.hpp
        Source/Random.cpp
        Source/TextEncoder.hpp
        Source/TextEncoder.cpp
        Source/TextDecoder.hpp
        Source/TextDecoder.cpp
        Source/File.hpp
        Source/File.cpp
        Source/Variant.cpp
        Source/PlatformFileSystem.hpp
        Source/PlatformFileSystem.cpp
        Source/VirtualFileSystem.hpp
        Source/VirtualFileSystem.cpp
        Source/Vfs.hpp
        Source/Vfs.cpp
        )

if(LIB)
    add_library(Core STATIC ${SRC_FILES})
else()
    add_library(Core SHARED ${SRC_FILES})
endif()

target_link_libraries(Core Runtime)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})