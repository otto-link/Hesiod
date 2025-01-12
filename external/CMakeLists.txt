project(external)

include(args.cmake)

# GNode
set(GNODE_ENABLE_DOCS OFF)
set(GNODE_ENABLE_EXAMPLES OFF)
set(GNODE_ENABLE_TESTS OFF)
add_subdirectory(GNode)

# GNode GUI
set(GNODEGUI_ENABLE_TESTS OFF)
add_subdirectory(GNodeGUI)

# HighMap
set(HIGHMAP_ENABLE_DOCS OFF)
set(HIGHMAP_ENABLE_EXAMPLES OFF)
set(HIGHMAP_ENABLE_TESTS OFF)
add_subdirectory(HighMap)

# Attributes
set(ATTRIBUTES_ENABLE_TESTS OFF)
add_subdirectory(Attributes)

# add it ony if it has not being added before by any other dependency
if(NOT TARGET nlohmann_json)
    add_subdirectory(json)
endif()