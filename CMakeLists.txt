cmake_minimum_required(VERSION 3.10)
project(hesiod)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Ofast -ffast-math -funroll-all-loops -funsafe-loop-optimizations -funsafe-math-optimizations -frounding-math")

set(CMAKE_CXX_STANDARD 17)

# sources
file(GLOB_RECURSE SOURCES
     "${PROJECT_SOURCE_DIR}/src/*.cpp")
     
add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
                           PUBLIC
                             ${PROJECT_SOURCE_DIR}/include
			   PRIVATE
			    )
			     
# Link libraries
target_link_libraries(${PROJECT_NAME}
  highmap
  )

# documentation
add_subdirectory(${PROJECT_SOURCE_DIR}/external/HighMap)
add_subdirectory(${PROJECT_SOURCE_DIR}/doc)
