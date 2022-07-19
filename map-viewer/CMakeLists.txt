
# project name
project( cartographics-map-viewer )

# use regular expression to include all files in `include` and `src` folder
file ( GLOB_RECURSE project_source_files src/*.*  )
file ( GLOB_RECURSE project_header_files include/*.* )

# add the source/header files to the project
add_executable( cartographics-map-viewer ${project_source_files} ${project_header_files} )

# add include directory to the includes path
target_include_directories ( cartographics-map-viewer PRIVATE "include/map-viewer" )

# add include paths for all the thridparty libraries
target_include_directories ( cartographics-map-viewer PRIVATE ${THRIDPARTY_INCLUDE_DIRS} )

# link all thirdparty libraries
target_link_libraries( cartographics-map-viewer PRIVATE ${THRIDPARTY_LINK_LIBRARIES} )

# add definitions with respective build mode
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  add_compile_definitions( DEBUG )
elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
  add_compile_definitions( NDEBUG )
endif()

# disable `_s` usage warnings in Windows
if ( WIN32 )
  add_compile_definitions( _CRT_SECURE_NO_WARNINGS )
endif( WIN32 )

