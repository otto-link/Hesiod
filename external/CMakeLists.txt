include(ImGui.cmake)
include(ImGuiNodeEditor.cmake)
include(ImGuiFileDialog.cmake)
include(ImCandy.cmake)

set(BUILD_DOC OFF)
set(BUILD_SANDBOX OFF)

add_subdirectory(HighMap)
add_subdirectory(GNode)
add_subdirectory(json)