cmake_minimum_required(VERSION 3.10)
project(MyCrowApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
endif()
