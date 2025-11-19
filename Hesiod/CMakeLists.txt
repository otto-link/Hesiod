project(hesiod LANGUAGES CXX)

# ------------------------------
# Qt6 setup
# ------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC_VERBOSE ON)

# ------------------------------
# Source files
# ------------------------------
file(GLOB_RECURSE HESIOD_GUI_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE HESIOD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/app/main.cpp)

# ------------------------------
# Executable
# ------------------------------
add_executable(${PROJECT_NAME})
target_sources(
  ${PROJECT_NAME} PRIVATE ${HESIOD_SOURCES} ${HESIOD_GUI_INCLUDES} # needed for
                                                                   # moc
)

# ------------------------------
# Include directories
# ------------------------------
target_include_directories(
  ${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include
          ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

# ------------------------------
# Compiler features
# ------------------------------
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# ------------------------------
# Link libraries
# ------------------------------
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE hesiod_options
          hesiod_platform
          hesiod_qt_logging
          args
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

# ------------------------------
# Copy data folder for dev
# ------------------------------
set(HESIOD_DATA_DIR ${CMAKE_SOURCE_DIR}/Hesiod/data)
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${HESIOD_DATA_DIR}
          $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)

# Optional second location (if you need parent folder for some legacy reason)
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${HESIOD_DATA_DIR}
          $<TARGET_FILE_DIR:${PROJECT_NAME}>/../data)

# ------------------------------
# Build information messages
# ------------------------------
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
