
cmake_minimum_required(VERSION 2.8)

project(simple_example)
set(library library-simple)

if(NOT TARGET BiCOMC)
	find_package(BiCOMC CONFIG REQUIRED)
endif()

include_directories("${CMAKE_CURRENT_LIST_DIR}")

add_library(${library} SHARED library.cpp)
target_link_libraries(${library} BiCOMC)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} BiCOMC)
add_dependencies(${PROJECT_NAME} ${library})

if(NOT WIN32)
	target_link_libraries(${PROJECT_NAME} dl)
endif()

# tests
include(CTest)

add_test(NAME ${PROJECT_NAME}-TC01 COMMAND ${PROJECT_NAME})
add_test(NAME ${PROJECT_NAME}-TC02 COMMAND ${PROJECT_NAME} $<TARGET_FILE:${library}>)
