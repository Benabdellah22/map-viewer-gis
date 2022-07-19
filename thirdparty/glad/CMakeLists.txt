
add_library( glad STATIC "src/glad.c" )

target_include_directories( glad PRIVATE "include" )

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  add_compile_definitions( DEBUG )
elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
  add_compile_definitions( NDEBUG )
endif()
