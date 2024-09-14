project(external)

include(args.cmake)
add_subdirectory(json)

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