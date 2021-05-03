add_executable( ParticleSystemTransformFeedback ParticleSystemTransformFeedback.c Noise3D.c Noise3D.h )
target_link_libraries( ParticleSystemTransformFeedback Common )

if( EMSCRIPTEN )
    target_link_options( ParticleSystemTransformFeedback
        PRIVATE --preload-file=${CMAKE_CURRENT_LIST_DIR}/smoke.tga@smoke.tga )
else()
    configure_file(smoke.tga ${CMAKE_CURRENT_BINARY_DIR}/smoke.tga COPYONLY)
endif()

