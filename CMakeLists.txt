cmake_minimum_required( VERSION 3.16 )

set( CMAKE_BUILD_TYPE Release )

# use C++17 to get those nice features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# root level cmake
project( none )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

# ---------------------- THIRD PARTY ----------------------- #

# static libraries
set( GLAD_INCLUDE_DIR   "thirdparty/glad/include"   )
set( GLFW_INCLUDE_DIR   "thirdparty/glfw/include"   )
set( IMGUI_INCLUDE_DIR  "thirdparty/imgui"          )

# header only libraries
set( HEADER_ONLY_INCLUDE_DIR "thirdparty/header-only" )

# get absolute paths to avoid path issues on other systems

if ( NOT IS_ABSOLUTE ${GLAD_INCLUDE_DIR} )
  get_filename_component( GLAD_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/${GLAD_INCLUDE_DIR}" ABSOLUTE )
endif()

if ( NOT IS_ABSOLUTE ${GLFW_INCLUDE_DIR} )
  get_filename_component( GLFW_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/${GLFW_INCLUDE_DIR}" ABSOLUTE )
endif()

if ( NOT IS_ABSOLUTE ${IMGUI_INCLUDE_DIR} )
  get_filename_component( IMGUI_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/${IMGUI_INCLUDE_DIR}" ABSOLUTE )
endif()

if ( NOT IS_ABSOLUTE ${HEADER_ONLY_INCLUDE_DIR} )
  get_filename_component( HEADER_ONLY_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/${HEADER_ONLY_INCLUDE_DIR}" ABSOLUTE )
endif()

# include projects that need compilation
add_subdirectory( thirdparty/glad   )
add_subdirectory( thirdparty/glfw   )
add_subdirectory( thirdparty/imgui  )

set ( THRIDPARTY_INCLUDE_DIRS
  "${GLAD_INCLUDE_DIR}"
  "${GLFW_INCLUDE_DIR}"
  "${IMGUI_INCLUDE_DIR}"
  "${HEADER_ONLY_INCLUDE_DIR}"
)

set ( THRIDPARTY_LINK_LIBRARIES
  glfw
  imgui
  glad
)

# --------------------- EXECUTABLE ------------------------- #

add_subdirectory( map-viewer )

# ---------------------------------------------------------- #
