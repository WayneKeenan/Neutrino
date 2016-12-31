cmake_minimum_required(VERSION 3.2)
project(Neutrino)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/external_dependencies/cmake-modules/)
set(BIN_DIR ${PROJECT_SOURCE_DIR}/build/bin/)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1 )

# Default build type will be release (No build type provided to CMAKE)
if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

# Set our build type compiler flags
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wcast-qual -Wwrite-strings -std=c++11 -Wno-c++14-compat")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O3")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
	string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
endif()


# Setup the package requirements
find_package(OpenGL REQUIRED)
if ( OPENGL_FOUND )
    include_directories(${OPENGL_INCLUDE_DIRS})
    link_libraries(${OPENGL_LIBRARIES})
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2IMAGE REQUIRED)
include_directories(
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/external_dependencies/glm/
    ${PROJECT_SOURCE_DIR}/external_dependencies/imgui/
    ${PROJECT_SOURCE_DIR}/external_dependencies/inih/
    ${PROJECT_SOURCE_DIR}/external_dependencies/inih/cpp/
)


if(SDL2_IMAGE_FOUND)
    MESSAGE("SDL2 Image found...")
    link_libraries(${SDL2_IMAGE_LIBRARY})
endif()

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    MESSAGE("Glew found...")
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()


find_package(LIBCONFIG REQUIRED)
if(LIBCONFIG_FOUND)
    MESSAGE("LibConfig found...")
    include_directories(${LIBCONFIG_INCLUDE_DIR})
    link_libraries(${LIBCONFIG_LIBRARY})
endif()

find_package(PhysFS REQUIRED)
if(PHYSFS_FOUND)
    MESSAGE("Phyfs found...")
    include_directories(${PHYSFS_INCLUDE_DIR})
    link_libraries(${PHYSFS_LIBRARY})
endif()

# Add source files (this means regenerating CMake files every time we add new source files)
FILE( GLOB_RECURSE NEUTRINO_HEADERS source/*.h )
FILE( GLOB_RECURSE NEUTRINO_SOURCES source/*.cpp )


add_executable(Neutrino ${NEUTRINO_SOURCES})
target_link_libraries(
    Neutrino ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${GLEW_LIBRARIES}
 )

# Technically don't need this as we run cmake from the build folder. Could use it to copy
# builds to the steam content folder though
install(TARGETS Neutrino RUNTIME DESTINATION ${BIN_DIR})

