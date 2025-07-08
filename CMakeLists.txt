cmake_minimum_required(VERSION 3.10)
project(MyCrowApp)

set(CMAKE_CXX_STANDARD 17)

find_package(Crow CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES
    main.cpp
    handlers/*.cpp
    utils/*.cpp
)

include_directories(include)

add_executable(MyCrowApp ${SOURCES})

target_link_libraries(MyCrowApp PRIVATE Crow::Crow)
