project(external)

include(args.cmake)

set(BUILD_DOC OFF)
set(BUILD_SANDBOX OFF)
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
