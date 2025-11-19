add_library(hesiod_platform INTERFACE)

# Linux
if(UNIX AND NOT APPLE)
  message(STATUS "Platform: Linux")
  target_compile_definitions(hesiod_platform INTERFACE HSD_OS_LINUX)

  # Windows
elseif(WIN32)
  message(STATUS "Platform: Windows")

  # Unsupported platforms (macOS already blocked in root CMakeLists)
else()
  message(
    FATAL_ERROR "Unsupported platform. Only Linux and Windows are supported.")
endif()
