project(hesiod)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS}
                    ${GLUT_INCLUDE_DIRS})

# prepare the runtime doc header
set(JSON_FILE_PATH ${CMAKE_SOURCE_DIR}/docs/runtime_doc.json)
file(READ ${JSON_FILE_PATH} JSON_CONTENT)

string(REPLACE "\(" "\[" JSON_CONTENT "${JSON_CONTENT}")
string(REPLACE "\)" "\]" JSON_CONTENT "${JSON_CONTENT}")

# Generate the header file with the embedded JSON content
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hesiod/model/nodes/runtime_doc.hpp.in
  ${CMAKE_BINARY_DIR}/include/hesiod/model/nodes/runtime_doc.hpp @ONLY)

# Qt6
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC_VERBOSE ON)

# --- sources GUI headers need to be added in add_executable, otherwise the moc
# won't parse them
file(GLOB_RECURSE HESIOD_GUI_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

file(GLOB_RECURSE HESIOD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/app/main.cpp)

add_executable(${PROJECT_NAME} ${HESIOD_SOURCES} ${HESIOD_GUI_INCLUDES})

set(HESIOD_INCLUDE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME} PRIVATE ${HESIOD_INCLUDE}
                                                   ${CMAKE_BINARY_DIR}/include)

# Link libraries
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE args
          spdlog::spdlog
          nlohmann_json::nlohmann_json
          highmap
          gnode
          gnodegui
          attributes
          Qt6::Core
          Qt6::OpenGL
          Qt6::Widgets
          Qt6::OpenGLWidgets)
