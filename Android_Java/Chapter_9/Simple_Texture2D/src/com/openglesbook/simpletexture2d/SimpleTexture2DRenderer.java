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

// Simple_Texture2D
//
//    This is a simple example that draws a quad with a 2D
//    texture image. The purpose of this example is to demonstrate
//    the basics of 2D texturing
//

package com.openglesbook.simpletexture2d;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.openglesbook.common.ESShader;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;

public class SimpleTexture2DRenderer implements GLSurfaceView.Renderer
{

   ///
   // Constructor
   //
   public SimpleTexture2DRenderer ( Context context )
   {

      mVertices = ByteBuffer.allocateDirect ( mVerticesData.length * 4 )
                  .order ( ByteOrder.nativeOrder() ).asFloatBuffer();
      mVertices.put ( mVerticesData ).position ( 0 );
      mIndices = ByteBuffer.allocateDirect ( mIndicesData.length * 2 )
                 .order ( ByteOrder.nativeOrder() ).asShortBuffer();
      mIndices.put ( mIndicesData ).position ( 0 );
   }

   //
   // Create a simple 2x2 texture image with four different colors
   //
   private int createSimpleTexture2D( )
   {
      // Texture object handle
      int[] textureId = new int[1];

      // 2x2 Image, 3 bytes per pixel (R, G, B)
      byte[] pixels =
      {
         ( byte ) 0xff,   0,   0, // Red
         0, ( byte ) 0xff,   0, // Green
         0,   0, ( byte ) 0xff, // Blue
         ( byte ) 0xff, ( byte ) 0xff,   0 // Yellow
      };
      ByteBuffer pixelBuffer = ByteBuffer.allocateDirect ( 4 * 3 );
      pixelBuffer.put ( pixels ).position ( 0 );

      // Use tightly packed data
      GLES30.glPixelStorei ( GLES30.GL_UNPACK_ALIGNMENT, 1 );

      //  Generate a texture object
      GLES30.glGenTextures ( 1, textureId, 0 );

      // Bind the texture object
      GLES30.glBindTexture ( GLES30.GL_TEXTURE_2D, textureId[0] );

      //  Load the texture
      GLES30.glTexImage2D ( GLES30.GL_TEXTURE_2D, 0, GLES30.GL_RGB, 2, 2, 0, GLES30.GL_RGB, GLES30.GL_UNSIGNED_BYTE, pixelBuffer );

      // Set the filtering mode
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_NEAREST );
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_NEAREST );

      return textureId[0];
   }

   ///
   // Initialize the shader and program object
   //
   public void onSurfaceCreated ( GL10 glUnused, EGLConfig config )
   {
      String vShaderStr =
         "#version 300 es              				\n" +
         "layout(location = 0) in vec4 a_position;   \n" +
         "layout(location = 1) in vec2 a_texCoord;   \n" +
         "out vec2 v_texCoord;     	  				\n" +
         "void main()                  				\n" +
         "{                            				\n" +
         "   gl_Position = a_position; 				\n" +
         "   v_texCoord = a_texCoord;  				\n" +
         "}                            				\n";

      String fShaderStr =
         "#version 300 es                                     \n" +
         "precision mediump float;                            \n" +
         "in vec2 v_texCoord;                            	 \n" +
         "layout(location = 0) out vec4 outColor;             \n" +
         "uniform sampler2D s_texture;                        \n" +
         "void main()                                         \n" +
         "{                                                   \n" +
         "  outColor = texture( s_texture, v_texCoord );      \n" +
         "}                                                   \n";

      // Load the shaders and get a linked program object
      mProgramObject = ESShader.loadProgram ( vShaderStr, fShaderStr );

      // Get the sampler location
      mSamplerLoc = GLES30.glGetUniformLocation ( mProgramObject, "s_texture" );

      // Load the texture
      mTextureId = createSimpleTexture2D ();

      GLES30.glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   }

   // /
   // Draw a triangle using the shader pair created in onSurfaceCreated()
   //
   public void onDrawFrame ( GL10 glUnused )
   {
      // Set the viewport
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

      // Bind the texture
      GLES30.glActiveTexture ( GLES30.GL_TEXTURE0 );
      GLES30.glBindTexture ( GLES30.GL_TEXTURE_2D, mTextureId );

      // Set the sampler texture unit to 0
      GLES30.glUniform1i ( mSamplerLoc, 0 );

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
   private int mSamplerLoc;

   // Texture handle
   private int mTextureId;

   // Additional member variables
   private int mWidth;
   private int mHeight;
   private FloatBuffer mVertices;
   private ShortBuffer mIndices;

   private final float[] mVerticesData =
   {
      -0.5f, 0.5f, 0.0f, // Position 0
      0.0f, 0.0f, // TexCoord 0
      -0.5f, -0.5f, 0.0f, // Position 1
      0.0f, 1.0f, // TexCoord 1
      0.5f, -0.5f, 0.0f, // Position 2
      1.0f, 1.0f, // TexCoord 2
      0.5f, 0.5f, 0.0f, // Position 3
      1.0f, 0.0f // TexCoord 3
   };

   private final short[] mIndicesData =
   {
      0, 1, 2, 0, 2, 3
   };

}
