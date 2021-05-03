add_executable( ParticleSystem ParticleSystem.c )
target_link_libraries( ParticleSystem Common )

if( EMSCRIPTEN )
    target_link_options( ParticleSystem
        PRIVATE --preload-file=${CMAKE_CURRENT_LIST_DIR}/smoke.tga@smoke.tga )
else()
    configure_file(smoke.tga ${CMAKE_CURRENT_BINARY_DIR}/smoke.tga COPYONLY)
endif()

