add_executable( MultiTexture MultiTexture.c )
target_link_libraries( MultiTexture Common )

if( EMSCRIPTEN )
    target_link_options( MultiTexture PRIVATE
        "SHELL:--preload-file \"${CMAKE_CURRENT_LIST_DIR}/basemap.tga@basemap.tga\""
        "SHELL:--preload-file \"${CMAKE_CURRENT_LIST_DIR}/lightmap.tga@lightmap.tga\"" )
else()
    configure_file(basemap.tga ${CMAKE_CURRENT_BINARY_DIR}/basemap.tga COPYONLY)
    configure_file(lightmap.tga ${CMAKE_CURRENT_BINARY_DIR}/lightmap.tga COPYONLY)
endif()

