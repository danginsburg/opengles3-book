set ( common_src Source/esShader.c 
                 Source/esShapes.c
                 Source/esTransform.c
                 Source/esUtil.c )


if( EMSCRIPTEN ) # Emscripten Platform files
    set( common_platform_src Source/Emscripten/esUtil_Emscripten.c )
    add_library( Common STATIC ${common_src} ${common_platform_src} )
    target_compile_options( Common PUBLIC
        "SHELL:-s MAX_WEBGL_VERSION=2" "SHELL:-s FULL_ES3=1" )
    target_link_options( Common PUBLIC
        "SHELL:-s MAX_WEBGL_VERSION=2" "SHELL:-s FULL_ES3=1"
        "SHELL:-s EXPORTED_RUNTIME_METHODS='[\"ccall\"]'"
        "SHELL:--shell-file \"${CMAKE_CURRENT_LIST_DIR}/Source/Emscripten/esUtil_Emscripten.html\"" )
elseif(WIN32) # Win32 Platform files
    set( common_platform_src Source/Win32/esUtil_win32.c )
    add_library( Common STATIC ${common_src} ${common_platform_src} )
    target_link_libraries( Common ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} )
else()
    find_package(X11)
    find_library(M_LIB m)
    set( common_platform_src Source/LinuxX11/esUtil_X11.c )
    add_library( Common STATIC ${common_src} ${common_platform_src} )
    target_link_libraries( Common ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} ${X11_LIBRARIES} ${M_LIB} )
endif()

             


