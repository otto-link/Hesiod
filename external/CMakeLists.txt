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

add_subdirectory(nodeeditor)
