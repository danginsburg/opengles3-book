add_executable( Bunny Bunny.cpp Wavefront.cpp)
target_link_libraries( Bunny Common )

configure_file(bunny.obj ${CMAKE_CURRENT_BINARY_DIR}/bunny.obj COPYONLY)
