cmake_minimum_required (VERSION 3.3)

set(SRC_FILES
        IController.hpp
        IInputDevice.hpp
        IKeyboard.hpp
        IMouse.hpp
        Input.hpp
        KeyCode.hpp
        
        Source/InputHeader.hpp
        Source/InputSystem.hpp
        Source/InputSystem.cpp
        )

if(WIN32)
    set(SRC_FILES ${SRC_FILES} 
        Source/Windows/Keyboard.hpp
        Source/Windows/Mouse.hpp
        Source/Windows/Mouse.cpp
        Source/Windows/PlatformInit.cpp
        Source/Windows/XBOXController.hpp
        Source/Windows/XBOXController.cpp)
endif()

if(LIB)
    add_library(Input STATIC ${SRC_FILES})
else()
    add_library(Input SHARED ${SRC_FILES})
endif()

target_link_libraries(Input Runtime Core)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})