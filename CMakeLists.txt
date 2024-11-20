cmake_minimum_required(VERSION 3.12...3.18)

project(TechArena LANGUAGES CXX)

if(UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++2a -fopenmp")
endif()

#set(SOURCE_EXE src/main.cpp)
#add_compile_definitions(FILE_INPUT)
#add_compile_definitions(RUN_TESTS)
add_compile_definitions(PARAM_SEARCH)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -fsanitize=address)
    link_libraries(-g -O0 -fsanitize=address)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3)
    link_libraries(-O3)
endif()

file(GLOB SOURCES "src/*.cpp")
add_executable(techarena ${SOURCES})
target_include_directories(techarena PRIVATE include)
#target_link_libraries(techarena lib)

message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
