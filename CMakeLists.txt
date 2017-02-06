
cmake_minimum_required(VERSION 3.0)

project(BiCOMC VERSION 0.1.0 LANGUAGES CXX)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

# options & caches
option(${PROJECT_NAME}_BUILD_EXAMPLE "Build ${PROJECT_NAME} examples." ON)


# projects
set(TARGET_EXPORT_NAME ${PROJECT_NAME_LOWER}-target)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS ${PROJECT_NAME}
	EXPORT ${TARGET_EXPORT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION include
)
install(DIRECTORY include/bicomc DESTINATION include)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if(${PROJECT_NAME}_BUILD_EXAMPLE)
	add_subdirectory(example)
endif()

add_custom_target(uninstall
	"${CMAKE_COMMAND}" -P "cmake/uninstall.cmake"
)


# install config file
set(confiInstallDir lib/cmake/${PROJECT_NAME})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	${PROJECT_NAME_LOWER}-config-version.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)
configure_package_config_file(cmake/config.cmake.in
	${PROJECT_NAME_LOWER}-config.cmake
	INSTALL_DESTINATION ${confiInstallDir}
)
install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config-version.cmake
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config.cmake
	DESTINATION ${confiInstallDir}
)

install(EXPORT ${TARGET_EXPORT_NAME}
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION ${confiInstallDir}
)

export(EXPORT ${TARGET_EXPORT_NAME}
	NAMESPACE ${PROJECT_NAME}::
)
