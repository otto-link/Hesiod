project(appimage)

add_custom_target(appimage ALL
    COMMAND ./generate_appimage.sh ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating AppImage"
    DEPENDS hesiod
)
