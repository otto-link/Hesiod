project(hesiod)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS}
                    ${GLUT_INCLUDE_DIRS})

# Qt5
set(CMAKE_AUTOMOC ON)

# --- sources GUI headers need to be added in add_executable, otherwise the moc
# won't parse them
file(GLOB_RECURSE HESIOD_GUI_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

file(GLOB_RECURSE HESIOD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/app/main.cpp)

add_executable(${PROJECT_NAME} ${HESIOD_SOURCES} ${HESIOD_GUI_INCLUDES})

set(HESIOD_INCLUDE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME} PRIVATE ${HESIOD_INCLUDE})

target_precompile_headers(
  ${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hesiod/model/attributes.hpp)

# Link libraries
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE args
          spdlog::spdlog
          highmap
          Qt5::Core
          Qt5::OpenGL
          Qt5::Widgets
          QtNodes::QtNodes)
