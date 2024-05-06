cmake_minimum_required(VERSION 3.10)
project(hesiod-root)

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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Od")
else()
  # GNU compiler options
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-dangling-reference -O3 -pthread -DLOG_LEVEL=3"
  )
endif()

add_definitions("-DBOOST_ALLOW_DEPRECATED_HEADERS")
add_definitions("-DBOOST_BIND_GLOBAL_PLACEHOLDERS")

set(CMAKE_CXX_STANDARD 20)

# --- Dear ImGui
find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL)

add_subdirectory(external)

add_subdirectory(Hesiod)

if(HESIOD_ENABLE_GENERATE_APP_IMAGE)
  add_subdirectory(scripts/generate_appimage)
endif(HESIOD_ENABLE_GENERATE_APP_IMAGE)
