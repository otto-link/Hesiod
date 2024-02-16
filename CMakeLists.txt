cmake_minimum_required(VERSION 3.10)
project(hesiod-root)

option(HESIOD_ENABLE_GENERATE_APP_IMAGE "" OFF)
option(HESIOD_ENABLE_GENERATE_NODE_SNAPSHOT "" OFF)
option(HESIOD_ENABLE_CEREAL "" ON)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC compiler options
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Od")
else()
    # GNU compiler options
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-dangling-reference -O3 -pthread -DLOG_LEVEL=3")
endif()

set(CMAKE_CXX_STANDARD 20)

# --- Dear ImGui
find_package(glfw3 REQUIRED)
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

add_subdirectory(external)

add_subdirectory(Hesiod)

if(HESIOD_ENABLE_GENERATE_APP_IMAGE)
    add_subdirectory(scripts/generate_appimage)
endif(HESIOD_ENABLE_GENERATE_APP_IMAGE)

# documentation
# add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
# add_subdirectory(${PROJECT_SOURCE_DIR}/scripts/generate_appimage)
