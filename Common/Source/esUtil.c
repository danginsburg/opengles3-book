//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// ESUtil.c
//
//    A utility library for OpenGL ES.  This library provides a
//    basic common framework for the example applications in the
//    OpenGL ES 3.0 Programming Guide.
//

///
//  Includes
//
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "esUtil.h"
#include "esUtil_win.h"

#ifdef ANDROID
#include <android/log.h>
#include <android_native_app_glue.h>
#endif // ANDROID

#ifdef __APPLE__
#include "FileWrapper.h"
#endif

///
//  Macros
//
#define INVERTED_BIT            (1 << 5)

///
//  Types
//
#ifndef __APPLE__
#pragma pack(push,x1)                            // Byte alignment (8-bit)
#pragma pack(1)
#endif

typedef struct
#ifdef __APPLE__
__attribute__((packed))
#endif
{
   unsigned char  IdSize,
                  MapType,
                  ImageType;
   unsigned short PaletteStart,
                  PaletteSize;
   unsigned char  PaletteEntryDepth;
   unsigned short X,
                  Y,
                  Width,
                  Height;
   unsigned char  ColorDepth,
                  Descriptor;
         
} TGA_HEADER;

#ifndef __APPLE__
#pragma pack(pop,x1)
#endif

#ifndef __APPLE__
///
// CreateEGLContext()
//
//    Creates an EGL rendering context and all associated elements
//
EGLBoolean CreateEGLContext ( EGLNativeWindowType eglNativeWindow, EGLNativeDisplayType eglNativeDisplay,
                              EGLDisplay* eglDisplay, EGLContext* eglContext, EGLSurface* eglSurface,
                              EGLint attribList[])
{
   EGLint numConfigs = 0;
   EGLint majorVersion;
   EGLint minorVersion;
   EGLDisplay display;
   EGLContext context;
   EGLSurface surface;
   EGLConfig config;
   EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };

   // Get Display
   display = eglGetDisplay(eglNativeDisplay);
   if ( display == EGL_NO_DISPLAY )
   {
      return EGL_FALSE;
   }

   // Initialize EGL
   if ( !eglInitialize(display, &majorVersion, &minorVersion) )
   {
      return EGL_FALSE;
   }

   // Choose config
   if ( !eglChooseConfig(display, attribList, &config, 1, &numConfigs) )
   {
      return EGL_FALSE;
   }

   if ( numConfigs < 1 )
   {
      return EGL_FALSE;
   }

#ifdef ANDROID
   // For Android, need to get the EGL_NATIVE_VISUAL_ID and set it using ANativeWindow_setBuffersGeometry
   {
      EGLint format = 0;
      eglGetConfigAttrib( display, config, EGL_NATIVE_VISUAL_ID, &format );
      ANativeWindow_setBuffersGeometry ( eglNativeWindow, 0, 0, format );
   }
#endif // ANDROID

   // Create a surface
   surface = eglCreateWindowSurface(display, config, eglNativeWindow, NULL);
   if ( surface == EGL_NO_SURFACE )
   {
      return EGL_FALSE;
   }

   // Create a GL context
   context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
   if ( context == EGL_NO_CONTEXT )
   {
      return EGL_FALSE;
   }   
   
   // Make the context current
   if ( !eglMakeCurrent(display, surface, surface, context) )
   {
      return EGL_FALSE;
   }
   
   *eglDisplay = display;
   *eglSurface = surface;
   *eglContext = context;
   return EGL_TRUE;
}
#endif

//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

///
//  esInitContext()
//
//      Initialize ES utility context.  This must be called before calling any other
//      functions.
//
void ESUTIL_API esInitContext ( ESContext *esContext )
{
   if ( esContext != NULL )
   {
      memset( esContext, 0, sizeof( ESContext ) );
   }
}

///
//  esCreateWindow()
//
//      title - name for title bar of window
//      width - width of window to create
//      height - height of window to create
//      flags  - bitwise or of window creation flags 
//          ES_WINDOW_ALPHA       - specifies that the framebuffer should have alpha
//          ES_WINDOW_DEPTH       - specifies that a depth buffer should be created
//          ES_WINDOW_STENCIL     - specifies that a stencil buffer should be created
//          ES_WINDOW_MULTISAMPLE - specifies that a multi-sample buffer should be created
//
GLboolean ESUTIL_API esCreateWindow ( ESContext *esContext, const char* title, GLint width, GLint height, GLuint flags )
{
#ifndef __APPLE__
   EGLint attribList[] =
   {
       EGL_RED_SIZE,       5,
       EGL_GREEN_SIZE,     6,
       EGL_BLUE_SIZE,      5,
       EGL_ALPHA_SIZE,     (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
       EGL_DEPTH_SIZE,     (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
       EGL_STENCIL_SIZE,   (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
       EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
       EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
       EGL_NONE
   };
   
   if ( esContext == NULL )
   {
      return GL_FALSE;
   }

#ifdef ANDROID
   // For Android, get the width/height from the window rather than what the 
   // application requested.
   esContext->width = ANativeWindow_getWidth ( esContext->eglNativeWindow );
   esContext->height = ANativeWindow_getHeight ( esContext->eglNativeWindow );
#else
   esContext->width = width;
   esContext->height = height;
#endif

   if ( !WinCreate ( esContext, title ) )
   {
      return GL_FALSE;
   }

  
   if ( !CreateEGLContext ( esContext->eglNativeWindow,
                            esContext->eglNativeDisplay,
                            &esContext->eglDisplay,
                            &esContext->eglContext,
                            &esContext->eglSurface,
                            attribList) )
   {
      return GL_FALSE;
   }
#endif // #ifndef __APPLE__
   

   return GL_TRUE;
}

///
//  esRegisterDrawFunc()
//
void ESUTIL_API esRegisterDrawFunc ( ESContext *esContext, void (ESCALLBACK *drawFunc) ( ESContext* ) )
{
   esContext->drawFunc = drawFunc;
}

///
//  esRegisterShutdownFunc()
//
void ESUTIL_API esRegisterShutdownFunc ( ESContext *esContext, void (ESCALLBACK *shutdownFunc) ( ESContext* ) )
{
   esContext->shutdownFunc = shutdownFunc;
}

///
//  esRegisterUpdateFunc()
//
void ESUTIL_API esRegisterUpdateFunc ( ESContext *esContext, void (ESCALLBACK *updateFunc) ( ESContext*, float ) )
{
   esContext->updateFunc = updateFunc;
}


///
//  esRegisterKeyFunc()
//
void ESUTIL_API esRegisterKeyFunc ( ESContext *esContext,
                                    void (ESCALLBACK *keyFunc) ( ESContext*, unsigned char, int, int ) )
{
   esContext->keyFunc = keyFunc;
}


///
// esLogMessage()
//
//    Log an error message to the debug output for the platform
//
void ESUTIL_API esLogMessage ( const char *formatStr, ... )
{
    va_list params;
    char buf[BUFSIZ];

    va_start ( params, formatStr );
    vsprintf ( buf, formatStr, params );
    
#ifdef ANDROID
    __android_log_print( ANDROID_LOG_INFO, "esUtil" , "%s", buf );
#else
    printf ( "%s", buf );
#endif
    
    va_end ( params );
}


///
// esLoadTGA()
//
//    Loads a 24-bit TGA image from a file
//
char* ESUTIL_API esLoadTGA ( const char *fileName, int *width, int *height )
{
   char        *buffer;
   FILE        *fp;
   TGA_HEADER   Header;
    
#ifdef __APPLE__
    // iOS: Remap the filename to a path that can be opened from the bundle.
    fileName = GetBundleFileName( fileName );
#endif

   if ( ( fp = fopen ( fileName, "rb" ) ) == 0 )
   {
      return NULL;
   }

   fread ( &Header, sizeof(TGA_HEADER), 1, fp );

   *width = Header.Width;
   *height = Header.Height;
   
   if ( Header.ColorDepth == 24 )
   {
      buffer = (char*)malloc(sizeof(char) * 3 * (*width) * (*height));

      if (buffer)
      {
         fread(buffer, sizeof(char) * 3, (*width) * (*height), fp);

         fclose(fp);
         return(buffer);
      }		
   }

   return(NULL);
}
