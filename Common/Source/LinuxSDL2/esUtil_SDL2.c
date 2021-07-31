
//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
//

// esUtil_SDL2.c
//
//    This file contains the LinuxX11 implementation of the windowing functions. 

///
// Includes
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include "esUtil.h"

#include <SDL2/SDL.h> // For Events
#include <SDL2/SDL_syswm.h>
SDL_Window *win;
SDL_GLContext glc;
SDL_Renderer* rdr;

//////////////////////////////////////////////////////////////////
//
//  Private Functions
//
//


//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

///
//  WinCreate()
//
//      This function initialized the native X11 display and window for EGL
//
EGLBoolean WinCreate(ESContext *esContext, const char *title)
{
    win = SDL_CreateWindow(
            title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            esContext->width, esContext->height, 
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
        );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    glc = SDL_GL_CreateContext(win);

    SDL_SysWMinfo sysInfo;
    SDL_VERSION(&sysInfo.version);
    SDL_GetWindowWMInfo(win, &sysInfo);

    esContext->eglNativeWindow = (EGLNativeWindowType)sysInfo.info.x11.window;
    esContext->eglNativeDisplay = (EGLNativeDisplayType)sysInfo.info.x11.display;

    return EGL_TRUE;
}

///
//  userInterrupt()
//
//      Reads from X11 event loop and interrupt program if there is a keypress, or
//      window close action.
//
GLboolean userInterrupt(ESContext *esContext)
{
    GLboolean userinterrupt = GL_FALSE;
    char text;

    SDL_Event e;
    int mouse_evt = 0;
    ESMouseHandle handle = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_QUIT: 
                userinterrupt = GL_TRUE;
                break;
            case SDL_KEYDOWN:
                text = e.key.keysym.sym;
                printf("key %c pressed\n", text);
                if (esContext->keyFunc != NULL)
                    esContext->keyFunc(esContext, text, 0, 0);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                mouse_evt = 1;
                handle.event = e.type == SDL_MOUSEBUTTONDOWN ? ES_MOUSE_BUTTON_DOWN : ES_MOUSE_BUTTON_UP;
                handle.x = e.button.x;
                handle.y = e.button.y;
                handle.clicks = e.button.clicks;
                handle.state = e.button.state == SDL_PRESSED ? ES_MOUSE_BUTTON_PRESSED : ES_MOUSE_BUTTON_RELEASED;
                switch(e.button.button){
                    case SDL_BUTTON_LEFT:
                        handle.button = ES_MOUSE_BUTTON_LEFT;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        handle.button = ES_MOUSE_BUTTON_MIDDLE;
                        break;
                    case SDL_BUTTON_RIGHT:
                        handle.button = ES_MOUSE_BUTTON_RIGHT;
                        break;
                }
                break;
            case SDL_MOUSEWHEEL:
                mouse_evt = 1;
                handle.event = ES_MOUSE_WHEEL;
                handle.x = e.wheel.x;
                handle.y = e.wheel.y;
                if(e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED){
                    handle.x *= -1;
                    handle.y *= -1;
                }
                break;
            case SDL_MOUSEMOTION:
                mouse_evt = 1;
                handle.event = ES_MOUSE_MOTION;
                handle.x = e.motion.x;
                handle.y = e.motion.y;
                handle.mv_x = e.motion.xrel;
                handle.mv_y = e.motion.yrel;
                if(e.motion.state & SDL_BUTTON_LMASK)
                    handle.state |= ES_MOUSE_BUTTON_LMASK;
                if(e.motion.state & SDL_BUTTON_MMASK)
                    handle.state |= ES_MOUSE_BUTTON_MMASK;
                if(e.motion.state & SDL_BUTTON_RMASK)
                    handle.state |= ES_MOUSE_BUTTON_RMASK;
                break;
        }
        if(mouse_evt && esContext->mouseFunc != NULL)
            esContext->mouseFunc(esContext, &handle);
    }

    return userinterrupt;
}

///
//  WinLoop()
//
//      Start main windows loop
//
void WinLoop ( ESContext *esContext )
{
    struct timeval t1, t2;
    struct timezone tz;
    float deltatime;

    gettimeofday ( &t1 , &tz );

    while(userInterrupt(esContext) == GL_FALSE)
    {
        gettimeofday(&t2, &tz);
        deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
        t1 = t2;

        if (esContext->updateFunc != NULL)
            esContext->updateFunc(esContext, deltatime);
        if (esContext->drawFunc != NULL)
            esContext->drawFunc(esContext);

        SDL_GL_SwapWindow(win);
    }
}

///
//  Global extern.  The application must declsare this function
//  that runs the application.
//
extern int esMain( ESContext *esContext );

///
//  main()
//
//      Main entrypoint for application
//
int main ( int argc, char *argv[] )
{
    ESContext esContext;

    memset ( &esContext, 0, sizeof( esContext ) );


    if ( esMain ( &esContext ) != GL_TRUE )
        return 1;   
 
    WinLoop ( &esContext );

    if ( esContext.shutdownFunc != NULL )
        esContext.shutdownFunc ( &esContext );

    if ( esContext.userData != NULL )
        free ( esContext.userData );

    return 0;
}
