cmake_minimum_required (VERSION 3.1)
project (AsyncFileReader)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(AsyncFileReader main.cpp AsyncFileReader.cpp)
target_link_libraries(AsyncFileReader)

