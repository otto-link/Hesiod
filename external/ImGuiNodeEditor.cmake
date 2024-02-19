project(ImGuiNodeEditor)

set(IMGUI_NODE_EDITOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui-node-editor)

set(IMGUI_NODE_EDITOR_SOURCE_FILES
  ${IMGUI_NODE_EDITOR_DIR}/crude_json.cpp
  ${IMGUI_NODE_EDITOR_DIR}/imgui_canvas.cpp
  ${IMGUI_NODE_EDITOR_DIR}/imgui_node_editor_api.cpp
  ${IMGUI_NODE_EDITOR_DIR}/imgui_node_editor.cpp)


add_library(${PROJECT_NAME} STATIC ${IMGUI_NODE_EDITOR_SOURCE_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE ImGui::ImGui)
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_NODE_EDITOR_DIR})