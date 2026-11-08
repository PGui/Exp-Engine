# Copyright (C) 2020 Guillaume Pastor
# https://shot511.github.io/2018-05-29-how-to-setup-opengl-project-with-cmake/
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# CMake 4.x (shipped with Visual Studio 2026) rejects projects declaring a minimum
# version < 3.5. Kept as a safety net for third-party code (ignored by CMake < 4).
if(NOT DEFINED CMAKE_POLICY_VERSION_MINIMUM)
	set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()

project(Exp-Engine LANGUAGES C CXX)

# Default standard for third-party code; the engine itself uses C++20 (required by EnTT 4).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Third-party options (must be set before their add_subdirectory)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
set(ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "Build a static (.a) version of the library")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "If the supplementary tools for Assimp are built in addition to the library.")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build tests.")
set(ASSIMP_NO_EXPORT ON CACHE BOOL "The engine only imports models.")
set(ASSIMP_INSTALL OFF CACHE BOOL "Do not generate assimp install targets.")
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "Do not install assimp PDBs.")


set(3RDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# Add source files (engine only; third-party code is built as separate targets below)
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp)

# Configure assets header file
configure_file(src/helpers/RootDir.h.in src/helpers/RootDir.h)
include_directories(${CMAKE_BINARY_DIR}/src)

#######################################
# Third-party libraries               #
#######################################

# Assimp
# zlib: use the system one when available, otherwise let assimp build its bundled copy.
# Forced so that a stale cache (e.g. from assimp 5, where the default was OFF) cannot
# leave ASSIMP_BUILD_ZLIB=OFF on a machine without zlib (typical on Windows).
find_package(ZLIB QUIET)
if(NOT ZLIB_FOUND)
	set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Build assimp's bundled zlib (no system zlib found)" FORCE)
endif()
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Do not fail on assimp warnings.")
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "No assimp samples.")
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "No assimp docs.")
add_subdirectory(${3RDPARTY_DIR}/assimp ${CMAKE_BINARY_DIR}/assimp)

# glm (header only)
add_subdirectory(${3RDPARTY_DIR}/glm)

# spdlog
add_subdirectory(${3RDPARTY_DIR}/spdlog)

# EnTT (header only)
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE "${3RDPARTY_DIR}/entt/src")
target_compile_features(entt INTERFACE cxx_std_20)

# GLFW
set(GLFW_DIR "${3RDPARTY_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")

# glad (generated, vendored)
set(GLAD_DIR "${3RDPARTY_DIR}/glad")
add_library(glad STATIC "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PUBLIC "${GLAD_DIR}/include")
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})

# Dear ImGui + GLFW/OpenGL3 backends
set(IMGUI_DIR "${3RDPARTY_DIR}/imgui")
add_library(imgui STATIC
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
	${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
	${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC glfw)

# stb (header only, implementation compiled in src/stb/stb_image.cpp)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${3RDPARTY_DIR}/stb")

# Remotery profiler
add_library(remotery STATIC "${3RDPARTY_DIR}/Remotery/lib/Remotery.c")
target_include_directories(remotery PUBLIC "${3RDPARTY_DIR}/Remotery/lib")
if(UNIX)
	find_package(Threads REQUIRED)
	target_link_libraries(remotery PUBLIC Threads::Threads m)
endif()

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw imgui assimp spdlog::spdlog glm::glm entt stb remotery)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE GLM_FORCE_RADIANS GLM_ENABLE_EXPERIMENTAL)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")

set(CMAKE_DEBUG_POSTFIX "d")

IF (MSVC)
  TARGET_COMPILE_OPTIONS(assimp PRIVATE /WX- /MP)
ELSE()
  TARGET_COMPILE_OPTIONS(assimp PRIVATE -Wno-error)
ENDIF()

if(MSVC)
	# /MP: parallel compilation, /utf-8: required by fmt/spdlog, /permissive-: standard conformance
	target_compile_options(${PROJECT_NAME} PRIVATE /MP /utf-8 /permissive- /W3)
	target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
	# Visual Studio: F5 launches the engine directly from the solution.
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
	# Keep third-party targets out of the way in the Solution Explorer.
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	foreach(dep assimp zlibstatic glfw glad imgui remotery spdlog uninstall UpdateAssimpLibsDebugSymbolsAndDLLs update_mappings)
		if(TARGET ${dep})
			set_property(TARGET ${dep} PROPERTY FOLDER "thirdparty")
		endif()
	endforeach()
endif()

# Create virtual folders to make it look nicer in VS
if(MSVC_IDE)
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	# Run macro
	GroupSources(src)
endif()

# Copy dlls (optional folder, not versioned)
if(WIN32 AND EXISTS "${PROJECT_SOURCE_DIR}/dlls")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/dlls"
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()
