// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
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
// esUtil_win32.c
//
//    This file contains the Win32 implementation of the windowing functions.


///
// Includes
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include "esUtil.h"

#ifdef _WIN64
#define GWL_USERDATA GWLP_USERDATA
#endif

//////////////////////////////////////////////////////////////////
//
//  Private Functions
//
//

///
//  ESWindowProc()
//
//      Main window procedure
//
LRESULT WINAPI ESWindowProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT  lRet = 1;

   switch ( uMsg )
   {
      case WM_CREATE:
         break;

      case WM_PAINT:
      {
         ESContext *esContext = ( ESContext * ) ( LONG_PTR ) GetWindowLongPtr ( hWnd, GWL_USERDATA );

         if ( esContext && esContext->drawFunc )
         {
            esContext->drawFunc ( esContext );
            eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
         }


         ValidateRect ( esContext->eglNativeWindow, NULL );
      }
      break;

      case WM_DESTROY:
         PostQuitMessage ( 0 );
         break;

      case WM_CHAR:
      {
         POINT      point;
         ESContext *esContext = ( ESContext * ) ( LONG_PTR ) GetWindowLongPtr ( hWnd, GWL_USERDATA );

         GetCursorPos ( &point );

         if ( esContext && esContext->keyFunc )
            esContext->keyFunc ( esContext, ( unsigned char ) wParam,
                                 ( int ) point.x, ( int ) point.y );
      }
      break;

      default:
         lRet = DefWindowProc ( hWnd, uMsg, wParam, lParam );
         break;
   }

   return lRet;
}

//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

///
//  WinCreate()
//
//      Create Win32 instance and window
//
GLboolean WinCreate ( ESContext *esContext, const char *title )
{
   WNDCLASS wndclass = {0};
   DWORD    wStyle   = 0;
   RECT     windowRect;
   HINSTANCE hInstance = GetModuleHandle ( NULL );


   wndclass.style         = CS_OWNDC;
   wndclass.lpfnWndProc   = ( WNDPROC ) ESWindowProc;
   wndclass.hInstance     = hInstance;
   wndclass.hbrBackground = ( HBRUSH ) GetStockObject ( BLACK_BRUSH );
   wndclass.lpszClassName = "opengles3.0";

   if ( !RegisterClass ( &wndclass ) )
   {
      return FALSE;
   }

   wStyle = WS_VISIBLE | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION;

   // Adjust the window rectangle so that the client area has
   // the correct number of pixels
   windowRect.left = 0;
   windowRect.top = 0;
   windowRect.right = esContext->width;
   windowRect.bottom = esContext->height;

   AdjustWindowRect ( &windowRect, wStyle, FALSE );



   esContext->eglNativeWindow = CreateWindow (
                                   "opengles3.0",
                                   title,
                                   wStyle,
                                   0,
                                   0,
                                   windowRect.right - windowRect.left,
                                   windowRect.bottom - windowRect.top,
                                   NULL,
                                   NULL,
                                   hInstance,
                                   NULL );

   // Set the ESContext* to the GWL_USERDATA so that it is available to the
   // ESWindowProc
#ifdef _WIN64
   //In LLP64 LONG is stll 32bit.
   SetWindowLongPtr( esContext->eglNativeWindow, GWL_USERDATA, ( LONGLONG ) ( LONG_PTR )esContext);
#else
   SetWindowLongPtr ( esContext->eglNativeWindow, GWL_USERDATA, ( LONG ) ( LONG_PTR ) esContext );
#endif


   if ( esContext->eglNativeWindow == NULL )
   {
      return GL_FALSE;
   }

   ShowWindow ( esContext->eglNativeWindow, TRUE );

   return GL_TRUE;
}

///
//  WinLoop()
//
//      Start main windows loop
//
void WinLoop ( ESContext *esContext )
{
   MSG msg = { 0 };
   int done = 0;
   DWORD lastTime = GetTickCount();

   while ( !done )
   {
      int gotMsg = ( PeekMessage ( &msg, NULL, 0, 0, PM_REMOVE ) != 0 );
      DWORD curTime = GetTickCount();
      float deltaTime = ( float ) ( curTime - lastTime ) / 1000.0f;
      lastTime = curTime;

      if ( gotMsg )
      {
         if ( msg.message == WM_QUIT )
         {
            done = 1;
         }
         else
         {
            TranslateMessage ( &msg );
            DispatchMessage ( &msg );
         }
      }
      else
      {
         SendMessage ( esContext->eglNativeWindow, WM_PAINT, 0, 0 );
      }

      // Call update function if registered
      if ( esContext->updateFunc != NULL )
      {
         esContext->updateFunc ( esContext, deltaTime );
      }
   }
}

///
//  Global extern.  The application must declare this function
//  that runs the application.
//
extern int esMain ( ESContext *esContext );

///
//  main()
//
//      Main entrypoint for application
//
int main ( int argc, char *argv[] )
{
   ESContext esContext;

   memset ( &esContext, 0, sizeof ( ESContext ) );

   if ( esMain ( &esContext ) != GL_TRUE )
   {
      return 1;
   }

   WinLoop ( &esContext );

   if ( esContext.shutdownFunc != NULL )
   {
      esContext.shutdownFunc ( &esContext );
   }

   if ( esContext.userData != NULL )
   {
      free ( esContext.userData );
   }

   return 0;
}
