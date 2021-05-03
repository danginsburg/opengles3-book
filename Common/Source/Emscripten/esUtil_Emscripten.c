// The MIT License (MIT)
//
// Copyright (c) 2021 Konstantin Podsvirov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// esUtil_Emscripten.c
//
//    This file contains the Emscripten implementation of the windowing functions.


///
// Includes
//
#include <emscripten.h>
#include <emscripten/html5.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "esUtil.h"


//////////////////////////////////////////////////////////////////
//
//  Private Functions
//
//

///
// GetCurrentTime()
//
static float GetCurrentTime()
{
   struct timespec clockRealTime;
   clock_gettime ( CLOCK_MONOTONIC, &clockRealTime );
   double curTimeInSeconds = clockRealTime.tv_sec + ( double ) clockRealTime.tv_nsec / 1e9;
   return ( float ) curTimeInSeconds;
}

EM_JS(int, js_get_window_width, (), {
    return window.innerWidth;
});

EM_JS(int, js_get_window_height, (), {
    return window.innerHeight;
});


///
//  Global extern.  The application must declare this function
//  that runs the application.
//
extern int esMain ( ESContext *esContext );

///
//  Static Variables
//

static struct ESContext esContext;
static float lastTime;


//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

void EMSCRIPTEN_KEEPALIVE c_reshape_window() {
   // Get actual window size
   esContext.width = js_get_window_width();
   esContext.height = js_get_window_height();
   // Set canvas size
   emscripten_set_canvas_element_size("canvas", esContext.width, esContext.height);
};

///
//  main_loop()
//
void main_loop ( )
{
   // Call app update function
   if ( esContext.updateFunc != NULL )
   {
      float curTime = GetCurrentTime();
      float deltaTime =  ( curTime - lastTime );
      lastTime = curTime;
      esContext.updateFunc ( &esContext, deltaTime );
   }

   if ( esContext.drawFunc != NULL )
   {
      esContext.drawFunc ( &esContext );
      eglSwapBuffers ( esContext.eglDisplay, esContext.eglSurface );
   }
}

///
//  main()
//
//

int main()
{
   // Initialize the context
   memset ( &esContext, 0, sizeof ( ESContext ) );

   // Call the main entrypoint for the app
   if ( esMain ( &esContext ) != GL_TRUE )
   {
      return EXIT_FAILURE;
   }

   lastTime = GetCurrentTime();

   // This function call won't return, and will engage in an infinite loop, processing events from the browser, and dispatching them.
   emscripten_set_main_loop_arg(main_loop, NULL, 0, 1);

   return EXIT_SUCCESS;
}

///
//  WinCreate()
//
//      Create window
//
GLboolean WinCreate ( ESContext *esContext, const char *title )
{
   // On Emscriptn, this does not need to do anything to create window (canvas).

   // Set window title
   emscripten_set_window_title(title);

   // For Emscripten, get the width/height from the window rather than what the
   // application requested.
   c_reshape_window();

   return GL_TRUE;
}
