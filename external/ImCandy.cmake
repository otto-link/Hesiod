project(ImCandy)

set(IMCANDY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ImguiCandy)

set(IMCANDY_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/stub/stub.cpp
)

add_library(${PROJECT_NAME} STATIC ${IMCANDY_SOURCE_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE ImGui::ImGui)
target_include_directories(${PROJECT_NAME} PUBLIC ${IMCANDY_DIR}/)