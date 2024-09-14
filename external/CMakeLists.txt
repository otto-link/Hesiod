project(external)

include(args.cmake)
add_subdirectory(json)

# GNode
set(GNODE_ENABLE_DOCS OFF)
set(GNODE_ENABLE_EXAMPLES OFF)
set(GNODE_ENABLE_TESTS OFF)
add_subdirectory(GNode)

# HighMap
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