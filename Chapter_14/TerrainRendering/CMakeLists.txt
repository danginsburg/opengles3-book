add_executable( TerrainRendering TerrainRendering.c )
target_link_libraries( TerrainRendering Common )

if( EMSCRIPTEN )
    target_link_options( TerrainRendering
        PRIVATE --preload-file=${CMAKE_CURRENT_LIST_DIR}/heightmap.tga@heightmap.tga )
else()
    configure_file(heightmap.tga ${CMAKE_CURRENT_BINARY_DIR}/heightmap.tga COPYONLY)
endif()
