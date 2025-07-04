cmake_minimum_required(VERSION 3.13)  
project(CSIIProject)               
set(CMAKE_CXX_STANDARD 14)            

set(SOURCE_FILES 
main.cpp
user.cpp
)

add_executable(trial ${SOURCE_FILES})

find_package(Crow)
target_link_libraries(your_project PUBLIC Crow::Crow)