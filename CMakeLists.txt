cmake_minimum_required(VERSION 3.10)
project(MyCrowApp)

set(CMAKE_CXX_STANDARD 17)

find_package(Crow CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(include)

add_executable(MyCrowApp
    main.cpp
    handlers/login_handler.cpp
    handlers/signup_handler.cpp
    utils/db_utils.cpp
    utils/hash_utils.cpp
)

target_link_libraries(MyCrowApp PRIVATE sqlite3 ssl crypto z sodium)
