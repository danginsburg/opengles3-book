add_executable( Sponza Sponza.cpp)
target_link_libraries( Sponza Common )
target_link_libraries( Sponza assimp )
file(COPY "Sponza_Zombie" DESTINATION . )
