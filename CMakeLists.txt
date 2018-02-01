
cmake_minimum_required(VERSION 2.8.8)

set(PROJECT_VERSION 0.1.0)
if(NOT (CMAKE_VERSION VERSION_LESS 3.0))
	cmake_policy(SET CMP0048 NEW)
	project(BiCOMC VERSION ${PROJECT_VERSION} LANGUAGES CXX)
else()
	project(BiCOMC CXX)
endif()

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

# options & caches
option(${PROJECT_NAME}_BUILD_EXAMPLE "Build ${PROJECT_NAME} examples." ON)

set(TARGET_EXPORT_NAME ${PROJECT_NAME_LOWER}-target)
set(INCLUDE_INSTALL_DIR "include")
set(CONFIG_INSTALL_DIR "lib/cmake/${PROJECT_NAME}")


# projects
include(CTest)

if(NOT (CMAKE_VERSION VERSION_LESS 3.0))
	add_library(${PROJECT_NAME} INTERFACE)
	target_include_directories(${PROJECT_NAME}
		INTERFACE
			$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
			$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
	)
elseif(NOT (CMAKE_VERSION VERSION_LESS 2.8.11))
	add_library(${PROJECT_NAME} STATIC "include/bicomc/object.h")
	target_include_directories(${PROJECT_NAME}
		INTERFACE
			$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
			$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
	)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		LINKER_LANGUAGE CXX
	)
else()
	include_directories("${CMAKE_CURRENT_LIST_DIR}/include")
	add_library(${PROJECT_NAME} STATIC "include/bicomc/object.h")
	set_target_properties(${PROJECT_NAME} PROPERTIES
		LINKER_LANGUAGE CXX
	)
endif()

if(${PROJECT_NAME}_BUILD_EXAMPLE)
	add_subdirectory(example)
endif()

if(NOT TARGET uninstall)
	add_custom_target(uninstall
		"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_LIST_DIR}/cmake/uninstall.cmake"
	)
endif()


# install
set(configFile "${CMAKE_CURRENT_BINARY_DIR}/generated/${PROJECT_NAME_LOWER}-config.cmake")
set(versionFile "${CMAKE_CURRENT_BINARY_DIR}/generated/${PROJECT_NAME_LOWER}-config-version.cmake")
set(licenseFiles "LICENSE" "CONTRIBUTORS" "NOTICE.md")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${versionFile}"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
	"cmake/config.cmake.in"
	"${configFile}"
	INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}"
)

install(FILES "${versionFile}" "${configFile}" ${licenseFiles}
	DESTINATION "${CONFIG_INSTALL_DIR}"
)
install(DIRECTORY "include/bicomc"
	DESTINATION "${INCLUDE_INSTALL_DIR}"
)
install(FILES ${licenseFiles}
	DESTINATION "${INCLUDE_INSTALL_DIR}/bicomc"
)

install(TARGETS ${PROJECT_NAME}
	EXPORT ${TARGET_EXPORT_NAME}
	ARCHIVE DESTINATION "lib/static"
)
install(EXPORT ${TARGET_EXPORT_NAME}
	DESTINATION "${CONFIG_INSTALL_DIR}"
)

export(TARGETS ${PROJECT_NAME} FILE "${TARGET_EXPORT_NAME}.cmake")
