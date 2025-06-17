cmake_minimum_required(VERSION 3.10)
project(hesiod-root VERSION 0.4.0)

# dont forget to remove the CMakeCache.txt in the build dir if options are
# changed
option(HESIOD_ENABLE_GENERATE_APP_IMAGE "" ON)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if(UNIX AND NOT APPLE)
  # see issue #152, for now the custom node face and node menu are only
  # available on linux
  message(STATUS "Configuring for LINUX system...")
  add_definitions("-DHSD_OS_LINUX")
else()
  message(STATUS "NOT configuring for LINUX system...")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # MSVC compiler options
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} /W4 /Od /D M_PI=3.14159265358979323846")
else()
  # GNU compiler options
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-dangling-reference -Ofast -ffast-math -pthread -DLOG_LEVEL=3 -Wno-deprecated-declarations"
  )
endif()

# * compile defitions

# Get latest Git tag
execute_process(
  COMMAND git describe --tags --abbrev=0
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE HESIOD_GIT_TAG
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get current commit short SHA
execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE HESIOD_GIT_SHA
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Optionally print for debug
message(STATUS "Git tag: ${HESIOD_GIT_TAG}")
message(STATUS "Git SHA: ${HESIOD_GIT_SHA}")

add_compile_definitions(
  HESIOD_VERSION_MAJOR=${hesiod-root_VERSION_MAJOR}
  HESIOD_VERSION_MINOR=${hesiod-root_VERSION_MINOR}
  HESIOD_VERSION_PATCH=${hesiod-root_VERSION_PATCH}
  HESIOD_GIT_TAG="${HESIOD_GIT_TAG}"
  HESIOD_GIT_SHA="${HESIOD_GIT_SHA}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets)

add_compile_definitions(
  CL_HPP_MINIMUM_OPENCL_VERSION=120 CL_HPP_TARGET_OPENCL_VERSION=120
  HSD_DEFAULT_GPU_MODE=true)

add_subdirectory(external)

add_subdirectory(Hesiod)

if(HESIOD_ENABLE_GENERATE_APP_IMAGE)
  add_subdirectory(scripts/generate_appimage)
endif(HESIOD_ENABLE_GENERATE_APP_IMAGE)
