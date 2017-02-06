
cmake_minimum_required(VERSION 3.0)

project("simple_example")

if(NOT TARGET BiCOMC::BiCOMC)
	find_package(BiCOMC REQUIRED)
endif()

include_directories(".")

add_library("library-simple" SHARED library.cpp)
target_link_libraries("library-simple" BiCOMC::BiCOMC)

add_executable(${PROJECT_NAME} main.cpp dl_helper.cpp)
target_link_libraries(${PROJECT_NAME} BiCOMC::BiCOMC)
add_dependencies(${PROJECT_NAME} "library-simple")

if(NOT WIN32)
	target_link_libraries(${PROJECT_NAME} dl)
endif()
