project(qt-toast)

set(QT_TOAST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qt-toast)

set(QT_TOAST_SRC ${QT_TOAST_DIR}/src/Toast.cpp)
set(QT_TOAST_INCLUDE_DIR ${QT_TOAST_DIR}/src/)

set(CMAKE_AUTOMOC ON)

add_library(${PROJECT_NAME} STATIC ${QT_TOAST_SRC})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt6::Widgets)

target_include_directories(${PROJECT_NAME} PUBLIC ${QT_TOAST_INCLUDE_DIR})
