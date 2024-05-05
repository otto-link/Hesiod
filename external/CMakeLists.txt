project(external)

include(args.cmake)

set(HIGHMAP_ENABLE_DOCS
    OFF
    CACHE BOOL "")
set(HIGHMAP_ENABLE_EXAMPLES
    OFF
    CACHE BOOL "")
set(HIGHMAP_ENABLE_TESTS
    OFF
    CACHE BOOL "")

add_subdirectory(HighMap)

set(BUILD_EXAMPLES
    OFF
    CACHE BOOL "")
set(USE_QT6
    OFF
    CACHE BOOL "")

if(UNIX AND NOT APPLE)
  # a bit hacky, but an access to these directories is required to tweak the
  # rendering of the nodes
  target_include_directories(
    ${PROJECT_NAME} INTERFACE nodeeditor/src
                              nodeeditor/include/QtNodes/internal)
endif()

add_subdirectory(nodeeditor)
