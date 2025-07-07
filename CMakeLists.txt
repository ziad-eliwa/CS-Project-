cmake_minimum_required(VERSION 3.14)
project(CSIIProject)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES
    main.cpp
    handlers/*.cpp
    utils/*.cpp
)

add_executable(csii ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR} include handlers utils)

find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)

target_link_libraries(csii PRIVATE SQLite::SQLite3 OpenSSL::SSL)
