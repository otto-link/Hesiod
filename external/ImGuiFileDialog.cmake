project(ImGuiFileDialog)

set(IMGUI_FILE_DIALOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ImGuiFileDialog)

set(IMGUI_FILE_DIALOG_SOURCE_FILES
    ${IMGUI_FILE_DIALOG_DIR}/ImGuiFileDialog.cpp
    ${IMGUI_FILE_DIALOG_DIR}/ImGuiFileDialog.h
    ${IMGUI_FILE_DIALOG_DIR}/ImGuiFileDialogConfig.h
)

add_library(${PROJECT_NAME} STATIC ${IMGUI_FILE_DIALOG_SOURCE_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE ImGui::ImGui)
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_FILE_DIALOG_DIR})