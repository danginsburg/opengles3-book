add_executable( ParticleSystemTransformFeedback ParticleSystemTransformFeedback.c )
target_link_libraries( ParticleSystemTransformFeedback Common )

configure_file(smoke.tga ${CMAKE_CURRENT_BINARY_DIR}/smoke.tga COPYONLY)

