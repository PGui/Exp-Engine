# Copyright (C) 2020 Guillaume Pastor
# https://shot511.github.io/2018-05-29-how-to-setup-opengl-project-with-cmake/
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Exp-Engine)

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# expected behaviour
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++lastest")

# set any variables you might need to set for your app and assimp
# set variables that are needed
set(ZLIB_LIBRARIES zlibstatic)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
set(ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "Build a static (.a) version of the library")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "If the supplementary tools for Assimp are built in addition to the library.")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build tests.")


set(3RDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# Add source files
file(GLOB_RECURSE SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.cpp
	${3RDPARTY_DIR}/imgui/imgui.h
    ${3RDPARTY_DIR}/imgui/imgui.cpp
    ${3RDPARTY_DIR}/imgui/imgui_internal.h
    ${3RDPARTY_DIR}/imgui/imgui_demo.cpp
    ${3RDPARTY_DIR}/imgui/imgui_draw.cpp
    ${3RDPARTY_DIR}/imgui/imgui_widgets.cpp
    ${3RDPARTY_DIR}/imgui/imconfig.h
    ${3RDPARTY_DIR}/imgui/examples/imgui_impl_glfw.h
    ${3RDPARTY_DIR}/imgui/examples/imgui_impl_glfw.cpp
    ${3RDPARTY_DIR}/imgui/examples/imgui_impl_opengl3.h
    ${3RDPARTY_DIR}/imgui/examples/imgui_impl_opengl3.cpp
    ${3RDPARTY_DIR}/stb/stb_image.h
    ${3RDPARTY_DIR}/Remotery/lib/Remotery.h
    ${3RDPARTY_DIR}/Remotery/lib/Remotery.c
    ${3RDPARTY_DIR}/entt/src/*.h
    ${3RDPARTY_DIR}/entt/src/*.hpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp)

# Configure assets header file
configure_file(src/helpers/RootDir.h.in src/helpers/RootDir.h)
include_directories(${CMAKE_BINARY_DIR}/src)

include_directories("${3RDPARTY_DIR}/imgui")
include_directories("${3RDPARTY_DIR}/imgui/examples")

include_directories("${3RDPARTY_DIR}/stb")

include_directories("${3RDPARTY_DIR}/Remotery/lib")

# add assimp source dir as a subdirectory, effectively making
add_subdirectory(${3RDPARTY_DIR}/assimp ${CMAKE_BINARY_DIR}/assimp)

# Add glm
add_subdirectory(${3RDPARTY_DIR}/glm)
add_definitions(-DGLM_FORCE_RADIANS) # use only modern API in radians
include_directories("${3RDPARTY_DIR}/glm")

#spdlog
add_subdirectory(${3RDPARTY_DIR}/spdlog)

#entt
include_directories("${3RDPARTY_DIR}/entt/src")

include_directories( ${OPENGL_INCLUDE_DIRS} )

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

#######################################
# LOOK for the packages that we need! #
#######################################

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
set(GLFW_DIR "${3RDPARTY_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${3RDPARTY_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

# ASSIMP
# be sure to link in assimp, use platform-agnostic syntax for the linker
target_link_libraries(${PROJECT_NAME} assimp)

#spdlog
target_link_libraries(${PROJECT_NAME} spdlog)

# Define the include DIRs
include_directories(
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/include"
)

set(CMAKE_DEBUG_POSTFIX "d")

IF (MSVC)
  TARGET_COMPILE_OPTIONS(assimp PRIVATE /WX-)
ELSE()
  TARGET_COMPILE_OPTIONS(assimp PRIVATE -Werror)
ENDIF()

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

# Copy dlls
if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/dlls"
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()
