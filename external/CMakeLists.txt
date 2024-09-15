project(external)

include(args.cmake)

# GNode
set(GNODE_ENABLE_DOCS OFF)
set(GNODE_ENABLE_EXAMPLES OFF)
set(GNODE_ENABLE_TESTS OFF)
add_subdirectory(GNode)

# GNode GUI
add_subdirectory(GNodeGUI)

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

# add it ony if it has not being added before by any other dependency
if(NOT TARGET nlohmann_json)
    add_subdirectory(json)
endif()