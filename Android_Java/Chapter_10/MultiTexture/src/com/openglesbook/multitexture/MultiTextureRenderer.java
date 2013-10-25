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

// MultiTexture
//
//    This is an example that draws a quad with a basemap and
//    lightmap to demonstrate multitexturing.
//

package com.openglesbook.multitexture;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.openglesbook.common.ESShader;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;


public class MultiTextureRenderer implements GLSurfaceView.Renderer
{

   ///
   // Constructor
   //
   public MultiTextureRenderer ( Context context )
   {
      mContext = context;
      mVertices = ByteBuffer.allocateDirect ( mVerticesData.length * 4 )
                  .order ( ByteOrder.nativeOrder() ).asFloatBuffer();
      mVertices.put ( mVerticesData ).position ( 0 );
      mIndices = ByteBuffer.allocateDirect ( mIndicesData.length * 2 )
                 .order ( ByteOrder.nativeOrder() ).asShortBuffer();
      mIndices.put ( mIndicesData ).position ( 0 );
   }

   ///
   //  Load texture from asset
   //
   private int loadTextureFromAsset ( String fileName )
   {
      int[] textureId = new int[1];
      Bitmap bitmap = null;
      InputStream is = null;

      try
      {
         is = mContext.getAssets().open ( fileName );
      }
      catch ( IOException ioe )
      {
         is = null;
      }

      if ( is == null )
      {
         return 0;
      }

      bitmap = BitmapFactory.decodeStream ( is );

      GLES30.glGenTextures ( 1, textureId, 0 );
      GLES30.glBindTexture ( GLES30.GL_TEXTURE_2D, textureId[0] );

      GLUtils.texImage2D ( GLES30.GL_TEXTURE_2D, 0, bitmap, 0 );

      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_LINEAR );
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_LINEAR );
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_S, GLES30.GL_CLAMP_TO_EDGE );
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_T, GLES30.GL_CLAMP_TO_EDGE );

      return textureId[0];
   }

   ///
   // Initialize the shader and program object
   //
   public void onSurfaceCreated ( GL10 glUnused, EGLConfig config )
   {
      // Load shaders from 'assets' and get a linked program object
      mProgramObject = ESShader.loadProgramFromAsset ( mContext,
                                                       "shaders/vertexShader.vert",
                                                       "shaders/fragmentShader.frag" );

      // Get the sampler locations
      mBaseMapLoc = GLES30.glGetUniformLocation ( mProgramObject, "s_baseMap" );
      mLightMapLoc = GLES30.glGetUniformLocation ( mProgramObject, "s_lightMap" );

      // Load the texture images from 'assets'
      mBaseMapTexId = loadTextureFromAsset ( "textures/basemap.png" );
      mLightMapTexId = loadTextureFromAsset ( "textures/lightmap.png" );

      GLES30.glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   }

   // /
   // Draw a triangle using the shader pair created in onSurfaceCreated()
   //
   public void onDrawFrame ( GL10 glUnused )
   {
      // Set the view-port
      GLES30.glViewport ( 0, 0, mWidth, mHeight );

      // Clear the color buffer
      GLES30.glClear ( GLES30.GL_COLOR_BUFFER_BIT );

      // Use the program object
      GLES30.glUseProgram ( mProgramObject );

      // Load the vertex position
      mVertices.position ( 0 );
      GLES30.glVertexAttribPointer ( 0, 3, GLES30.GL_FLOAT,
                                     false,
                                     5 * 4, mVertices );
      // Load the texture coordinate
      mVertices.position ( 3 );
      GLES30.glVertexAttribPointer ( 1, 2, GLES30.GL_FLOAT,
                                     false,
                                     5 * 4,
                                     mVertices );

      GLES30.glEnableVertexAttribArray ( 0 );
      GLES30.glEnableVertexAttribArray ( 1 );


      // Bind the base map
      GLES30.glActiveTexture ( GLES30.GL_TEXTURE0 );
      GLES30.glBindTexture ( GLES30.GL_TEXTURE_2D, mBaseMapTexId );

      // Set the base map sampler to texture unit to 0
      GLES30.glUniform1i ( mBaseMapLoc, 0 );

      // Bind the light map
      GLES30.glActiveTexture ( GLES30.GL_TEXTURE1 );
      GLES30.glBindTexture ( GLES30.GL_TEXTURE_2D, mLightMapTexId );

      // Set the light map sampler to texture unit 1
      GLES30.glUniform1i ( mLightMapLoc, 1 );

      GLES30.glDrawElements ( GLES30.GL_TRIANGLES, 6, GLES30.GL_UNSIGNED_SHORT, mIndices );
   }

   ///
   // Handle surface changes
   //
   public void onSurfaceChanged ( GL10 glUnused, int width, int height )
   {
      mWidth = width;
      mHeight = height;
   }


   // Handle to a program object
   private int mProgramObject;

   // Sampler location
   private int mBaseMapLoc;
   private int mLightMapLoc;

   // Texture handle
   private int mBaseMapTexId;
   private int mLightMapTexId;

   // Additional member variables
   private int mWidth;
   private int mHeight;
   private FloatBuffer mVertices;
   private ShortBuffer mIndices;
   private Context mContext;

   private final float[] mVerticesData =
   {
      -0.5f,  0.5f, 0.0f, // Position 0
       0.0f,  0.0f,       // TexCoord 0
      -0.5f, -0.5f, 0.0f, // Position 1
       0.0f,  1.0f,       // TexCoord 1
       0.5f, -0.5f, 0.0f, // Position 2
       1.0f,  1.0f,       // TexCoord 2
       0.5f,  0.5f, 0.0f, // Position 3
       1.0f,  0.0f        // TexCoord 3
   };

   private final short[] mIndicesData =
   {
      0, 1, 2, 0, 2, 3
   };

}
