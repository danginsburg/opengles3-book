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

package com.openglesbook.simpletexturecubemap;

import java.nio.ByteBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.openglesbook.common.ESShader;
import com.openglesbook.common.ESShapes;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;

public class SimpleTextureCubemapRenderer implements GLSurfaceView.Renderer
{
   ///
   // Constructor
   //
   public SimpleTextureCubemapRenderer ( Context context )
   {
   }

   ///
   // Create a simple cubemap with a 1x1 face with a different
   // color for each face
   private int createSimpleTextureCubemap( )
   {
      int[] textureId = new int[1];

      // Face 0 - Red
      byte[] cubePixels0 = { 127, 0, 0 };
      // Face 1 - Green
      byte[] cubePixels1 = { 0, 127, 0 };
      // Face 2 - Blue
      byte[] cubePixels2 = { 0, 0, 127 };
      // Face 3 - Yellow
      byte[] cubePixels3 = { 127, 127, 0 };
      // Face 4 - Purple
      byte[] cubePixels4 = { 127, 0, 127 };
      // Face 5 - White
      byte[] cubePixels5 = { 127, 127, 127 };

      ByteBuffer cubePixels = ByteBuffer.allocateDirect ( 3 );

      // Generate a texture object
      GLES30.glGenTextures ( 1, textureId, 0 );

      // Bind the texture object
      GLES30.glBindTexture ( GLES30.GL_TEXTURE_CUBE_MAP, textureId[0] );

      // Load the cube face - Positive X
      cubePixels.put ( cubePixels0 ).position ( 0 );
      GLES30.glTexImage2D ( GLES30.GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GLES30.GL_RGB, 1, 1, 0,
                            GLES30.GL_RGB, GLES30.GL_UNSIGNED_BYTE, cubePixels );

      // Load the cube face - Negative X
      cubePixels.put ( cubePixels1 ).position ( 0 );
      GLES30.glTexImage2D ( GLES30.GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GLES30.GL_RGB, 1, 1, 0,
                            GLES30.GL_RGB, GLES30.GL_UNSIGNED_BYTE, cubePixels );

      // Load the cube face - Positive Y
      cubePixels.put ( cubePixels2 ).position ( 0 );
      GLES30.glTexImage2D ( GLES30.GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GLES30.GL_RGB, 1, 1, 0,
                            GLES30.GL_RGB, GLES30.GL_UNSIGNED_BYTE, cubePixels );

      // Load the cube face - Negative Y
      cubePixels.put ( cubePixels3 ).position ( 0 );
      GLES30.glTexImage2D ( GLES30.GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GLES30.GL_RGB, 1, 1, 0,
                            GLES30.GL_RGB, GLES30.GL_UNSIGNED_BYTE, cubePixels );

      // Load the cube face - Positive Z
      cubePixels.put ( cubePixels4 ).position ( 0 );
      GLES30.glTexImage2D ( GLES30.GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GLES30.GL_RGB, 1, 1, 0,
                            GLES30.GL_RGB, GLES30.GL_UNSIGNED_BYTE, cubePixels );

      // Load the cube face - Negative Z
      cubePixels.put ( cubePixels5 ).position ( 0 );
      GLES30.glTexImage2D ( GLES30.GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GLES30.GL_RGB, 1, 1, 0,
                            GLES30.GL_RGB, GLES30.GL_UNSIGNED_BYTE, cubePixels );

      // Set the filtering mode
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_CUBE_MAP, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_NEAREST );
      GLES30.glTexParameteri ( GLES30.GL_TEXTURE_CUBE_MAP, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_NEAREST );

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
         "layout(location = 1) in vec3 a_normal;     \n" +
         "out vec3 v_normal;       	  				\n" +
         "void main()                  				\n" +
         "{                            				\n" +
         "   gl_Position = a_position; 				\n" +
         "   v_normal = a_normal;      				\n" +
         "}                            				\n";

      String fShaderStr =

         "#version 300 es              						 \n" +
         "precision mediump float;                            \n" +
         "in vec3 v_normal;                              	 \n" +
         "layout(location = 0) out vec4 outColor;             \n" +
         "uniform samplerCube s_texture;                      \n" +
         "void main()                                         \n" +
         "{                                                   \n" +
         "  outColor = texture( s_texture, v_normal );		 \n" +
         "}                                                   \n";

      // Load the shaders and get a linked program object
      mProgramObject = ESShader.loadProgram ( vShaderStr, fShaderStr );

      // Get the sampler location
      mSamplerLoc = GLES30.glGetUniformLocation ( mProgramObject, "s_texture" );

      // Load the texture
      mTextureId = createSimpleTextureCubemap ();

      // Generate the vertex data
      mSphere.genSphere ( 20, 0.75f );

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

      GLES30.glCullFace ( GLES30.GL_BACK );
      GLES30.glEnable ( GLES30.GL_CULL_FACE );

      // Use the program object
      GLES30.glUseProgram ( mProgramObject );

      // Load the vertex position
      GLES30.glVertexAttribPointer ( 0, 3, GLES30.GL_FLOAT,
                                     false, 0, mSphere.getVertices() );
      // Load the texture coordinate

      GLES30.glVertexAttribPointer ( 1, 3, GLES30.GL_FLOAT,
                                     false, 0, mSphere.getNormals() );

      GLES30.glEnableVertexAttribArray ( 0 );
      GLES30.glEnableVertexAttribArray ( 1 );

      // Bind the texture
      GLES30.glActiveTexture ( GLES30.GL_TEXTURE0 );
      GLES30.glBindTexture ( GLES30.GL_TEXTURE_CUBE_MAP, mTextureId );

      // Set the sampler texture unit to 0
      GLES30.glUniform1i ( mSamplerLoc, 0 );

      GLES30.glDrawElements ( GLES30.GL_TRIANGLES, mSphere.getNumIndices(), GLES30.GL_UNSIGNED_SHORT, mSphere.getIndices() );
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

   // Texture ID
   private int mTextureId;

   // Vertex data
   private ESShapes mSphere = new ESShapes();

   // Additional member variables
   private int mWidth;
   private int mHeight;
}
