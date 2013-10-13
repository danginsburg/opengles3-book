add_executable( TerrainRendering TerrainRendering.c )
target_link_libraries( TerrainRendering Common )

configure_file(heightmap.tga ${CMAKE_CURRENT_BINARY_DIR}/heightmap.tga COPYONLY)
