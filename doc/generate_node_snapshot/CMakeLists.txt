file(GLOB_RECURSE SOURCES
     ${PROJECT_SOURCE_DIR}/src/gui/*.cpp
     ${PROJECT_SOURCE_DIR}/src/nodes/*.cpp
     ${PROJECT_SOURCE_DIR}/external/ImGuiFileDialog/ImGuiFileDialog.cpp)

add_executable(snapshot
               main.cpp
	             ${SOURCES}
	             ${IMGUI_SRC}
               ${IMGUI_NODE_EDITOR_SRC})

target_link_libraries(snapshot
  highmap
  gnode
  glfw
  OpenGL::GL
  ${GLEW_LIBRARIES}
	${GLUT_LIBRARY})
	
target_include_directories(snapshot ${HESIOD_INCLUDE})
