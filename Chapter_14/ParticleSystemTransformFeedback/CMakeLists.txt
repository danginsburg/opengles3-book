add_executable( ParticleSystemTransformFeedback ParticleSystemTransformFeedback.c Noise3D.c Noise3D.h )
target_link_libraries( ParticleSystemTransformFeedback Common )

configure_file(smoke.tga ${CMAKE_CURRENT_BINARY_DIR}/smoke.tga COPYONLY)

