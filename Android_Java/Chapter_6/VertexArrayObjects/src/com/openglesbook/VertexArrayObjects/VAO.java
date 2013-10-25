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

package com.openglesbook.VertexArrayObjects;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;

/**
 * Activity class for example program that detects OpenGL ES 3.0.
 **/
public class VAO extends Activity
{

   private final int CONTEXT_CLIENT_VERSION = 3;

   @Override
   protected void onCreate ( Bundle savedInstanceState )
   {
      super.onCreate ( savedInstanceState );
      mGLSurfaceView = new GLSurfaceView ( this );

      if ( detectOpenGLES30() )
      {
         // Tell the surface view we want to create an OpenGL ES 3.0-compatible
         // context, and set an OpenGL ES 3.0-compatible renderer.
         mGLSurfaceView.setEGLContextClientVersion ( CONTEXT_CLIENT_VERSION );
         mGLSurfaceView.setRenderer ( new VAORenderer ( this ) );
      }
      else
      {
         Log.e ( "SimpleTexture2D", "OpenGL ES 3.0 not supported on device.  Exiting..." );
         finish();

      }

      setContentView ( mGLSurfaceView );
   }

   private boolean detectOpenGLES30()
   {
      ActivityManager am =
         ( ActivityManager ) getSystemService ( Context.ACTIVITY_SERVICE );
      ConfigurationInfo info = am.getDeviceConfigurationInfo();
      return ( info.reqGlEsVersion >= 0x30000 );
   }

   @Override
   protected void onResume()
   {
      // Ideally a game should implement onResume() and onPause()
      // to take appropriate action when the activity looses focus
      super.onResume();
      mGLSurfaceView.onResume();
   }

   @Override
   protected void onPause()
   {
      // Ideally a game should implement onResume() and onPause()
      // to take appropriate action when the activity looses focus
      super.onPause();
      mGLSurfaceView.onPause();
   }

   private GLSurfaceView mGLSurfaceView;
}
