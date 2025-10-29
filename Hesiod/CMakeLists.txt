project(hesiod)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS}
                    ${GLUT_INCLUDE_DIRS})

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

# data (two locations for convenience aka lazyness)
file(COPY ${CMAKE_SOURCE_DIR}/Hesiod/data
     DESTINATION ${EXECUTABLE_OUTPUT_PATH}/.)
file(COPY ${CMAKE_SOURCE_DIR}/Hesiod/data
     DESTINATION ${EXECUTABLE_OUTPUT_PATH}/../.)

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
          Qt6::OpenGLWidgets
          qterrain-renderer
          qtexture_downloader)

# --- some checking

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Global flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")
