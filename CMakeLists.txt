cmake_minimum_required(VERSION 3.10)
project(MyCrowApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory to project root for all configurations
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR})

# Find required packages
find_package(Threads REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/handlers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/services)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)

# Add all relevant source files to the executable
set(SOURCES
    main.cpp
    utils/hash_utils.cpp
    handlers/login_handler.cpp
    handlers/signup_handler.cpp
    handlers/post_handler.cpp
    handlers/timeline_handler.cpp
    handlers/profile_handler.cpp
    handlers/comment_handler.cpp
    handlers/friend_handler.cpp
    handlers/like_handler.cpp
    handlers/search_handler.cpp
    services/Post.cpp
    services/Timeline.cpp
    services/post_service.cpp
    services/Timeline_service.cpp
    services/comment_service.cpp
    services/comment.cpp
    services/User.cpp
    services/profile.cpp
    services/profile_services.cpp
    services/friend_search_service.cpp
    services/friend_suggestion_service.cpp
    services/FriendManager.cpp
    services/AVLtree.cpp
    services/BST.cpp
    services/notification_service.cpp
    services/like_service.cpp
    services/dynamic_timeline_service.cpp
    database/db_utils.cpp

    # Add other .cpp files as needed
)

add_executable(MyCrowApp ${SOURCES})


target_link_libraries(MyCrowApp PRIVATE 
    sqlite3
    Threads::Threads
    nlohmann_json::nlohmann_json
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
