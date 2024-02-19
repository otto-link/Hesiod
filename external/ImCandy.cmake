project(ImCandy)

set(IMCANDY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ImguiCandy)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE ${IMCANDY_DIR})