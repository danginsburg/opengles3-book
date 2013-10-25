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

// Example_6_3
//
//    This example demonstrates using client-side vertex arrays
//    and a constant vertex attribute
//

package com.openglesbook.example6_3;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.openglesbook.common.ESShader;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;

public class Example6_3Renderer implements GLSurfaceView.Renderer
{

   ///
   // Constructor
   //
   public Example6_3Renderer ( Context context )
   {

      mVertices = ByteBuffer.allocateDirect ( mVerticesData.length * 4 )
                  .order ( ByteOrder.nativeOrder() ).asFloatBuffer();
      mVertices.put ( mVerticesData ).position ( 0 );
   }

   ///
   // Initialize the shader and program object
   //
   public void onSurfaceCreated ( GL10 glUnused, EGLConfig config )
   {
      String vShaderStr =
         "#version 300 es                            \n" +
         "layout(location = 0) in vec4 a_color;      \n" +
         "layout(location = 1) in vec4 a_position;   \n" +
         "out vec4 v_color;                          \n" +
         "void main()                                \n" +
         "{                                          \n" +
         "    v_color = a_color;                     \n" +
         "    gl_Position = a_position;              \n" +
         "}";


      String fShaderStr =
         "#version 300 es            \n" +
         "precision mediump float;   \n" +
         "in vec4 v_color;           \n" +
         "out vec4 o_fragColor;      \n" +
         "void main()                \n" +
         "{                          \n" +
         "    o_fragColor = v_color; \n" +
         "}" ;

      // Load the shaders and get a linked program object
      mProgramObject = ESShader.loadProgram ( vShaderStr, fShaderStr );

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

      // Set the vertex color to red
      GLES30.glVertexAttrib4f ( 0, 1.0f, 0.0f, 0.0f, 1.0f );

      // Load the vertex position
      mVertices.position ( 0 );
      GLES30.glVertexAttribPointer ( 1, 3, GLES30.GL_FLOAT,
                                     false,
                                     0, mVertices );

      GLES30.glEnableVertexAttribArray ( 1 );

      GLES30.glDrawArrays ( GLES30.GL_TRIANGLES, 0, 3 );

      GLES30.glDisableVertexAttribArray ( 1 );
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

   // Additional member variables
   private int mWidth;
   private int mHeight;
   private FloatBuffer mVertices;

   private final float[] mVerticesData =
   {
      0.0f,  0.5f, 0.0f, // v0
      -0.5f, -0.5f, 0.0f, // v1
      0.5f, -0.5f, 0.0f  // v2
   };
}
