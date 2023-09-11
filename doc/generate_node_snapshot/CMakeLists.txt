file(GLOB_RECURSE SOURCES
     ${PROJECT_SOURCE_DIR}/src/gui/*.cpp
     ${PROJECT_SOURCE_DIR}/src/nodes/*.cpp)

add_executable(snapshot
               main.cpp
	       ${SOURCES}
	       ${IMGUI_SRC}
               ${IMGUI_NODE_EDITOR_SRC})

target_link_libraries(snapshot
  highmap
  gnode
  glfw
  OpenGL::GL)

message(${PROJECT_SOURCE_DIR})
message(${IMGUI_INCLUDE})
	
target_include_directories(snapshot
                           PUBLIC
                             ${PROJECT_SOURCE_DIR}/include
			   PRIVATE
           		     ${PROJECT_SOURCE_DIR}/external/cereal/include
			     ${PROJECT_SOURCE_DIR}/external/macro-logger
			     ${IMGUI_INCLUDE}
                             ${IMGUI_NODE_EDITOR_INCLUDE}
			    )
