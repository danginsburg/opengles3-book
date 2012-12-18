set ( common_src Source/esShader.c 
                 Source/esShapes.c
                 Source/esTransform.c
                 Source/esUtil.c )

# Win32 Platform files
if(WIN32)
	set( common_platform_src Source/Win32/esUtil_TGA.c 
							 Source/Win32/esUtil_win32.c )
else()
	message( FATAL_ERROR "Unsupported platform" )
endif()

             

add_library( Common STATIC ${common_src} ${common_platform_src} )                                 
target_link_libraries( Common ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} )