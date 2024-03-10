project(args)

set(ARGS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/args)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE ${ARGS_DIR})