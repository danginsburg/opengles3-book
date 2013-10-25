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

// TextureWrap
//
//    This is an example that demonstrates the three texture
//    wrap modes available on 2D textures
//

package com.openglesbook.texturewrap;

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

public class TextureWrapRenderer implements GLSurfaceView.Renderer
{

   ///
   // Constructor
   //
   public TextureWrapRenderer ( Context context )
   {

      mVertices = ByteBuffer.allocateDirect ( mVerticesData.length * 4 )
                  .order ( ByteOrder.nativeOrder() ).asFloatBuffer();
      mVertices.put ( mVerticesData ).position ( 0 );
      mIndices = ByteBuffer.allocateDirect ( mIndicesData.length * 2 )
                 .order ( ByteOrder.nativeOrder() ).asShortBuffer();
      mIndices.put ( mIndicesData ).position ( 0 );
   }

   ///
   //  Generate an RGB8 checkerboard image
   //
   private ByteBuffer genCheckImage ( int width, int height, int checkSize )
   {
      int x,
          y;
      byte[] pixels = new byte[width * height * 3];


      for ( y = 0; y < height; y++ )
         for ( x = 0; x < width; x++ )
         {
            byte rColor = 0;
            byte bColor = 0;

            if ( ( x / checkSize ) % 2 == 0 )
            {
               rColor = ( byte ) ( 127 * ( ( y / checkSize ) % 2 ) );
               bColor = ( byte ) ( 127 * ( 1 - ( ( y / checkSize ) % 2 ) ) );
            }
            else
            {
               bColor = ( byte ) ( 127 * ( ( y / checkSize ) % 2 ) );
               rColor = ( byte ) ( 127 * ( 1 - ( ( y / checkSize ) % 2 ) ) );
            }

            pixels[ ( y * width + x ) * 3] = rColor;
            pixels[ ( y * width + x ) * 3 + 1] = 0;
            pixels[ ( y * width + x ) * 3 + 2] = bColor;
         }

      ByteBuffer result = ByteBuffer.allocateDirect ( width * height * 3 );
      result.put ( pixels ).position ( 0 );
      return result;
   }

   ///
   // Create a 2D texture image
   //
   private int createTexture2D( )
   {
      // Texture object handle
      int[] textureId = new int[1];
      int    width = 256,
             height = 256;
      ByteBuffer pixels;

      pixels = genCheckImage ( width, height, 64 );

      // Generate a texture object
      GLES30.glGenTextures ( 1, textureId, 0 );

      // Bind the texture object
      GLES30.glBindTexture ( GLES30.GL_TEXTURE_2D, textureId[0] );

      // Load mipmap level 0
      GLES30.glTexImage2D ( GLES30.GL_TEXTURE_2D, 0, GLES30.GL_RGB, width, height,
                            0, GLES30.GL_RGB, GLES30.GL_UNSIGNED_BYTE, pixels );

      // Set the filtering mode
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_LINEAR );
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_LINEAR );

      return textureId[0];
   }



   ///
   // Initialize the shader and program object
   //
   public void onSurfaceCreated ( GL10 glUnused, EGLConfig config )
   {
      String vShaderStr =
         "#version 300 es                            \n" +
         "uniform float u_offset;      				\n" +
         "layout(location = 0) in vec4 a_position;   \n" +
         "layout(location = 1) in vec2 a_texCoord;   \n" +
         "out vec2 v_texCoord;     					\n" +
         "void main()                  				\n" +
         "{                            				\n" +
         "   gl_Position = a_position; 				\n" +
         "   gl_Position.x += u_offset;				\n" +
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
         "   outColor = texture( s_texture, v_texCoord );  	 \n" +
         "}                                                   \n";

      // Load the shaders and get a linked program object
      mProgramObject = ESShader.loadProgram ( vShaderStr, fShaderStr );

      // Get the sampler location
      mSamplerLoc = GLES30.glGetUniformLocation ( mProgramObject, "s_texture" );

      // Get the offset location
      mOffsetLoc = GLES30.glGetUniformLocation ( mProgramObject, "u_offset" );

      // Load the texture
      mTextureId = createTexture2D ();

      GLES30.glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   }

   ///
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
      GLES30.glVertexAttribPointer ( 0, 4, GLES30.GL_FLOAT,
                                     false,
                                     6 * 4, mVertices );
      // Load the texture coordinate
      mVertices.position ( 4 );
      GLES30.glVertexAttribPointer ( 1, 2, GLES30.GL_FLOAT,
                                     false,
                                     6 * 4,
                                     mVertices );

      GLES30.glEnableVertexAttribArray ( 0 );
      GLES30.glEnableVertexAttribArray ( 1 );

      // Bind the texture
      GLES30.glActiveTexture ( GLES30.GL_TEXTURE0 );
      GLES30.glBindTexture ( GLES30.GL_TEXTURE_2D, mTextureId );

      // Set the sampler texture unit to 0
      GLES30.glUniform1i ( mSamplerLoc, 0 );

      // Draw quad with repeat wrap mode
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_S, GLES30.GL_REPEAT );
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_T, GLES30.GL_REPEAT );
      GLES30.glUniform1f ( mOffsetLoc, -0.7f );
      GLES30.glDrawElements ( GLES30.GL_TRIANGLES, 6, GLES30.GL_UNSIGNED_SHORT, mIndices );

      // Draw quad with clamp to edge wrap mode
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_S, GLES30.GL_CLAMP_TO_EDGE );
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_T, GLES30.GL_CLAMP_TO_EDGE );
      GLES30.glUniform1f ( mOffsetLoc, 0.0f );
      GLES30.glDrawElements ( GLES30.GL_TRIANGLES, 6, GLES30.GL_UNSIGNED_SHORT, mIndices );

      // Draw quad with mirrored repeat
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_S, GLES30.GL_MIRRORED_REPEAT );
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_T, GLES30.GL_MIRRORED_REPEAT );
      GLES30.glUniform1f ( mOffsetLoc, 0.7f );
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

   // Offset location
   private int mOffsetLoc;

   // Texture handle
   private int mTextureId;

   // Additional member variables
   private int mWidth;
   private int mHeight;
   private FloatBuffer mVertices;
   private ShortBuffer mIndices;

   private final float[] mVerticesData =
   {
      -0.3f,  0.3f, 0.0f, 1.0f,  // Position 0
      -1.0f,  -1.0f,              // TexCoord 0
      -0.3f, -0.3f, 0.0f, 1.0f, // Position 1
      -1.0f,  2.0f,              // TexCoord 1
      0.3f, -0.3f, 0.0f, 1.0f, // Position 2
      2.0f,  2.0f,              // TexCoord 2
      0.3f,  0.3f, 0.0f, 1.0f,  // Position 3
      2.0f,  -1.0f               // TexCoord 3
   };

   private final short[] mIndicesData =
   {
      0, 1, 2, 0, 2, 3
   };

}
