cmake_minimum_required(VERSION 3.9)
project(raylibtest)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++17 -O1 -g -Wall -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++17 -O3 -Wall -s -Wno-missing-braces")

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(ENABLE_ASAN ON)
endif()
add_subdirectory(raylib)

add_executable(raylibtest src/main.cpp src/map.cpp src/drawfunctions.cpp loguru/loguru.cpp)
target_link_libraries(raylibtest raylib)