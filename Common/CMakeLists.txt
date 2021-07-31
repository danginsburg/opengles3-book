set ( common_src Source/esShader.c 
                 Source/esShapes.c
                 Source/esTransform.c
                 Source/esUtil.c )


# Win32 Platform files
if(WIN32)
    set( common_platform_src Source/Win32/esUtil_win32.c )
    add_library( Common STATIC ${common_src} ${common_platform_src} )
    target_link_libraries( Common ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} )
else()
    find_library(M_LIB m)
	find_package(SDL2)
	if(SDL2_FOUND)
		set( common_platform_src Source/LinuxSDL2/esUtil_SDL2.c )
		add_library( Common STATIC ${common_src} ${common_platform_src} )
		target_link_libraries( Common ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} ${SDL2_LIBRARIES} ${M_LIB} )
	else()
		find_package(X11)
		set( common_platform_src Source/LinuxX11/esUtil_X11.c )
		add_library( Common STATIC ${common_src} ${common_platform_src} )
		target_link_libraries( Common ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} ${X11_LIBRARIES} ${M_LIB} )
	endif()
endif()

