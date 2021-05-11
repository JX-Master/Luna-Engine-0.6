cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
            Base.hpp 
            PlatformDefines.hpp
            Assert.hpp
            Base64.hpp
            Unicode.hpp
            Iterator.hpp
            MemoryUtils.hpp
            Hash.hpp
            Functional.hpp
            Algorithm.hpp
            Atomic.hpp
            Runtime.hpp
            Vector.hpp
            HashSet.hpp
            HashMap.hpp
            String.hpp
            RingDeque.hpp
            Name.hpp
            Math.hpp
            Memory.hpp
            Path.hpp
            Time.hpp
            Blob.hpp
            TSAssert.hpp
            Module.hpp
            Result.hpp
            Thread.hpp
            HashMultiMap.hpp
            HashMultiSet.hpp
            Debug.hpp
            Error.hpp
            LexicalAnalyzer.hpp
            TypeBuilder.hpp
            Platform.hpp
            List.hpp

            Source/OS.hpp
            Source/HashTable.hpp
            Source/HashMultiTable.hpp
            Source/Base64.cpp
            Source/Unicode.cpp
            Source/Name.hpp
            Source/Name.cpp
            Source/Math/MathBase.hpp
            Source/Math/Simd.hpp
            Source/Math/SimdConvert.hpp
            Source/Math/SimdMatrix.hpp
            Source/Math/SimdMisc.hpp
            Source/Math/SimdVector.hpp
            Source/Math/Vector.hpp
            Source/Math/Matrix.hpp
            Source/Math/Transform.hpp
            Source/Math/Quaternion.hpp
            Source/Math/Color.hpp
            Source/Module.hpp
            Source/Module.cpp
            Source/Error.cpp
            Source/Memory.hpp
            Source/Memory.cpp
            Source/Time.cpp
            Source/Thread.cpp
            Source/Assert.cpp
            Source/Runtime.cpp
            Source/Debug.cpp
            Source/Assembly.hpp
            Source/Assembly.cpp
            Source/TypeBuilder.hpp
            Source/TypeBuilder.cpp
            Source/FunctionBuilder.hpp
            Source/TypeInfo.hpp
            Source/LexicalAnalyzer.cpp
            Source/Platform.cpp
            )

if (WIN32)
    set(SRC_FILES ${SRC_FILES} 
        Source/Platform/Windows/AtomicImpl.hpp
        Source/Platform/Windows/MiniWin.hpp
        Source/Platform/Windows/Assert.cpp
        Source/Platform/Windows/Memory.cpp
        Source/Platform/Windows/Thread.cpp
        Source/Platform/Windows/OS.cpp
        Source/Platform/Windows/Sync.cpp
        Source/Platform/Windows/Debug.cpp
        Source/Platform/Windows/Time.cpp
        Source/Platform/Windows/File.cpp
        Source/Platform/Windows/DLL.cpp)
        
endif()

if(UNIX)
    set(SRC_FILES ${SRC_FILES}
        Source/Platform/POSIX/AtomicImpl.hpp
        Source/Platform/POSIX/Errno.hpp
        Source/Platform/POSIX/Assert.cpp
        Source/Platform/POSIX/Memory.cpp
        Source/Platform/POSIX/Thread.cpp
        Source/Platform/POSIX/OS.cpp
        Source/Platform/POSIX/Sync.cpp
        Source/Platform/POSIX/Debug.cpp
        Source/Platform/POSIX/Time.cpp
        Source/Platform/POSIX/File.cpp
        Source/Platform/POSIX/DLL.cpp)
endif()

option(CHECK_MEMORY_LEAK "Check memory leak when the Engine shut down." OFF)

if(CHECK_MEMORY_LEAK)
    add_definitions("-DLUNA_RUNTIME_CHECK_MEMORY_LEAK")
endif()

if(LIB)
    add_library(Runtime STATIC ${SRC_FILES})
else()
    add_library(Runtime SHARED ${SRC_FILES})
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})
