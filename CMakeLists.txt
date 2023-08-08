cmake_minimum_required(VERSION 3.10)
project(hesiod)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Ofast -ffast-math -funroll-all-loops -funsafe-loop-optimizations -funsafe-math-optimizations -frounding-math")

set(CMAKE_CXX_STANDARD 20)

# --- Dear ImGui
find_package(glfw3 REQUIRED)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

set(IMGUI_DIR external/imgui)

set(IMGUI_SRC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

set(IMGUI_INCLUDE
	external/imgui
	external/imgui/backends)

set(IMNODES_DIR external/imnodes_fork)

set(IMNODES_SRC
    ${IMNODES_DIR}/imnodes.cpp)
    
set(IMNODES_INCLUDE
	${IMNODES_DIR})

# --- sources
file(GLOB_RECURSE SOURCES
     ${PROJECT_SOURCE_DIR}/src/*.cpp)
     
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${IMGUI_SRC}
    ${IMNODES_SRC}
)

target_include_directories(${PROJECT_NAME}
                           PUBLIC
                             ${PROJECT_SOURCE_DIR}/include
			   PRIVATE
			     ${PROJECT_SOURCE_DIR}/external/macro-logger
			     ${IMGUI_INCLUDE}
			     ${IMNODES_INCLUDE}
			    )
			     
# Link libraries
target_link_libraries(${PROJECT_NAME}
  highmap
  gnode
  glfw
  OpenGL::GL)

# documentatio
add_subdirectory(${PROJECT_SOURCE_DIR}/external/HighMap)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/GNode)
add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
