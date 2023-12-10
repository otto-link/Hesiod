cmake_minimum_required(VERSION 3.10)
project(hesiod)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC compiler options
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Od")
else()
    # GNU compiler options
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-dangling-reference -O3 -pthread -DLOG_LEVEL=3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_CEREAL")
endif()

set(CMAKE_CXX_STANDARD 20)

# --- Dear ImGui
find_package(glfw3 REQUIRED)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/external/imgui)

set(IMGUI_SRC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

set(IMGUI_INCLUDE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends)

set(IMGUI_NODE_EDITOR_SRC
  ${PROJECT_SOURCE_DIR}/external/imgui-node-editor/crude_json.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui-node-editor/imgui_canvas.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui-node-editor/imgui_node_editor_api.cpp
  ${PROJECT_SOURCE_DIR}/external/imgui-node-editor/imgui_node_editor.cpp)

set(IMGUI_NODE_EDITOR_INCLUDE
  ${PROJECT_SOURCE_DIR}/external/imgui-node-editor
)

# --- sources
file(GLOB_RECURSE HESIOD_SOURCES
     ${PROJECT_SOURCE_DIR}/src/*.cpp
     ${PROJECT_SOURCE_DIR}/external/ImGuiFileDialog/ImGuiFileDialog.cpp)
     
add_executable(${PROJECT_NAME}
    ${HESIOD_SOURCES}
    ${IMGUI_SRC}
    ${IMGUI_NODE_EDITOR_SRC}
)

set(HESIOD_INCLUDE
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    ${PROJECT_SOURCE_DIR}/external/cereal/include
    ${PROJECT_SOURCE_DIR}/external/ImguiCandy
    ${PROJECT_SOURCE_DIR}/external/ImGuiFileDialog
    ${PROJECT_SOURCE_DIR}/external/macro-logger
    ${IMGUI_INCLUDE}
    ${IMGUI_NODE_EDITOR_INCLUDE}
)

target_include_directories(${PROJECT_NAME} ${HESIOD_INCLUDE})
			     
# Link libraries
target_link_libraries(${PROJECT_NAME}
  highmap
  gnode
  glfw
  OpenGL::GL
  GLEW::GLEW
	GLUT::GLUT)

# documentatio
add_subdirectory(${PROJECT_SOURCE_DIR}/external/HighMap)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/GNode)
add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
