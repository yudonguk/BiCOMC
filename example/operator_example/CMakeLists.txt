
cmake_minimum_required(VERSION 2.8)

project(operator_example)

if(NOT TARGET BiCOMC)
	find_package(BiCOMC CONFIG REQUIRED)
endif()

include_directories("${CMAKE_CURRENT_LIST_DIR}")

add_library(library-callable SHARED library.cpp)
target_link_libraries(library-callable BiCOMC)

add_executable(${PROJECT_NAME} main.cpp dl_helper.cpp)
target_link_libraries(${PROJECT_NAME} BiCOMC)
add_dependencies(${PROJECT_NAME} library-callable)

if(NOT WIN32)
	target_link_libraries(${PROJECT_NAME} dl)
endif()
