cmake_minimum_required(VERSION 3.10)
project(MyCrowApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory to project root for all configurations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR})

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(MyCrowApp
    main.cpp
    handlers/login_handler.cpp
    handlers/signup_handler.cpp
    utils/db_utils.cpp
    utils/hash_utils.cpp
)

# Link all libraries together
target_link_libraries(MyCrowApp PRIVATE 
    sqlite3
    Threads::Threads
)

# For Windows, you might need to link additional libraries
if(WIN32)
    target_link_libraries(MyCrowApp PRIVATE ws2_32 wsock32)
    
    # Copy SQLite3 DLL to project root after build (Windows only)
    # Try to find sqlite3.dll from vcpkg or build directory
    find_file(SQLITE3_DLL 
        NAMES sqlite3.dll
        PATHS 
            "${CMAKE_CURRENT_SOURCE_DIR}/build/Debug"
            "$ENV{VCPKG_ROOT}/installed/x64-windows/bin"
            "$ENV{VCPKG_ROOT}/installed/x86-windows/bin"
        NO_DEFAULT_PATH
    )
    
    if(SQLITE3_DLL)
        add_custom_command(TARGET MyCrowApp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SQLITE3_DLL}"
            ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Copying SQLite3 DLL to project root: ${SQLITE3_DLL}"
            VERBATIM
        )
    else()
        message(WARNING "SQLite3 DLL not found. You may need to copy it manually.")
    endif()
endif()
