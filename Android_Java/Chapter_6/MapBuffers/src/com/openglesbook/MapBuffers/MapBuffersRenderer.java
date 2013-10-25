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

// MapBuffers
//
//    This example demonstrates mapping buffer objects
//

package com.openglesbook.MapBuffers;

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

public class MapBuffersRenderer implements GLSurfaceView.Renderer
{
   ///
   // Constructor
   //
   public MapBuffersRenderer ( Context context )
   {
      //
   }

   ///
   // Initialize the shader and program object
   //
   public void onSurfaceCreated ( GL10 glUnused, EGLConfig config )
   {
      String vShaderStr =
         "#version 300 es                            \n" +
         "layout(location = 0) in vec4 a_position;   \n" +
         "layout(location = 1) in vec4 a_color;      \n" +
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

      mVBOIds[0] = 0;
      mVBOIds[1] = 0;

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

      drawPrimitiveWithVBOsMapBuffers();
   }

   private void drawPrimitiveWithVBOsMapBuffers()
   {
      int offset = 0;
      int numVertices = 3;
      int numIndices = 3;
      int vtxStride = 4 * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE );

      // mVBOIds[0] - used to store vertex attribute data
      // mVBOIds[l] - used to store element indices
      if ( mVBOIds[0] == 0 && mVBOIds[1] == 0 )
      {
         // Only allocate on the first draw
         GLES30.glGenBuffers ( 2, mVBOIds, 0 );

         GLES30.glBindBuffer ( GLES30.GL_ARRAY_BUFFER, mVBOIds[0] );
         GLES30.glBufferData ( GLES30.GL_ARRAY_BUFFER, vtxStride * numVertices,
                               null, GLES30.GL_STATIC_DRAW );
         vtxMappedBuf =
            ( ( ByteBuffer ) GLES30.glMapBufferRange (
                 GLES30.GL_ARRAY_BUFFER, 0, vtxStride * numVertices,
                 GLES30.GL_MAP_WRITE_BIT | GLES30.GL_MAP_INVALIDATE_BUFFER_BIT )
            ).order ( ByteOrder.nativeOrder() ).asFloatBuffer();

         // Copy the data into the mapped buffer
         vtxMappedBuf.put ( mVerticesData ).position ( 0 );

         // Unamp the buffer
         GLES30.glUnmapBuffer ( GLES30.GL_ARRAY_BUFFER );

         // Map the index buffer
         GLES30.glBindBuffer ( GLES30.GL_ELEMENT_ARRAY_BUFFER, mVBOIds[1] );
         GLES30.glBufferData ( GLES30.GL_ELEMENT_ARRAY_BUFFER, 2 * numIndices,
                               null, GLES30.GL_STATIC_DRAW );
         idxMappedBuf =
            ( ( ByteBuffer ) GLES30.glMapBufferRange (
                 GLES30.GL_ELEMENT_ARRAY_BUFFER, 0, 2 * numIndices,
                 GLES30.GL_MAP_WRITE_BIT | GLES30.GL_MAP_INVALIDATE_BUFFER_BIT )
            ).order ( ByteOrder.nativeOrder() ).asShortBuffer();

         // Copy the data into the mapped buffer
         idxMappedBuf.put ( mIndicesData ).position ( 0 );

         // Unamp the buffer
         GLES30.glUnmapBuffer ( GLES30.GL_ELEMENT_ARRAY_BUFFER );
      }

      GLES30.glBindBuffer ( GLES30.GL_ARRAY_BUFFER, mVBOIds[0] );

      GLES30.glBindBuffer ( GLES30.GL_ELEMENT_ARRAY_BUFFER, mVBOIds[1] );

      GLES30.glEnableVertexAttribArray ( VERTEX_POS_INDX );
      GLES30.glEnableVertexAttribArray ( VERTEX_COLOR_INDX );

      GLES30.glVertexAttribPointer ( VERTEX_POS_INDX, VERTEX_POS_SIZE,
                                     GLES30.GL_FLOAT, false, vtxStride, offset );

      offset += ( VERTEX_POS_SIZE * 4 );

      GLES30.glVertexAttribPointer ( VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
                                     GLES30.GL_FLOAT, false, vtxStride, offset );

      GLES30.glDrawElements ( GLES30.GL_TRIANGLES, numIndices,
                              GLES30.GL_UNSIGNED_SHORT, 0 );

      GLES30.glDisableVertexAttribArray ( VERTEX_POS_INDX );
      GLES30.glDisableVertexAttribArray ( VERTEX_COLOR_INDX );

      GLES30.glBindBuffer ( GLES30.GL_ARRAY_BUFFER, 0 );
      GLES30.glBindBuffer ( GLES30.GL_ELEMENT_ARRAY_BUFFER, 0 );
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

   private FloatBuffer vtxMappedBuf;
   private ShortBuffer idxMappedBuf;

   // VertexBufferObject Ids
   private int [] mVBOIds = new int[2];

   // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
   private final float[] mVerticesData =
   {
      0.0f,  0.5f, 0.0f,        // v0
      1.0f,  0.0f, 0.0f, 1.0f,  // c0
      -0.5f, -0.5f, 0.0f,        // v1
      0.0f,  1.0f, 0.0f, 1.0f,  // c1
      0.5f, -0.5f, 0.0f,        // v2
      0.0f,  0.0f, 1.0f, 1.0f,  // c2
   };

   private final short[] mIndicesData =
   {
      0, 1, 2
   };

   final int VERTEX_POS_SIZE   = 3; // x, y and z
   final int VERTEX_COLOR_SIZE = 4; // r, g, b, and a

   final int VERTEX_POS_INDX   = 0;
   final int VERTEX_COLOR_INDX = 1;
}
