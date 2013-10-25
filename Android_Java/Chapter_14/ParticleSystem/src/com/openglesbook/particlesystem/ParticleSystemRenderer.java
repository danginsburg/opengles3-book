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

// ParticleSystem
//
//    This is an example that demonstrates rendering a particle system
//    using a vertex shader and point sprites.
//

package com.openglesbook.particlesystem;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import com.openglesbook.common.ESShader;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.os.SystemClock;

public class ParticleSystemRenderer implements GLSurfaceView.Renderer
{

   ///
   // Constructor
   //
   public ParticleSystemRenderer ( Context context )
   {
      mContext = context;

      // Fill in particle data array
      for ( int i = 0; i < ( NUM_PARTICLES * PARTICLE_SIZE ); i += PARTICLE_SIZE )
      {
         // Lifetime of particle
         mParticleData[i + 0] = ( ( float ) ( ( int ) ( Math.random() * 10000 ) % 10000 ) / 10000.0f );;

         // End position of particle
         mParticleData[i + 1] = ( ( float ) ( ( int ) ( Math.random() * 10000 ) % 10000 ) / 5000.0f ) - 1.0f;
         mParticleData[i + 2] = ( ( float ) ( ( int ) ( Math.random() * 10000 ) % 10000 ) / 5000.0f ) - 1.0f;
         mParticleData[i + 3] = ( ( float ) ( ( int ) ( Math.random() * 10000 ) % 10000 ) / 5000.0f ) - 1.0f;

         // Start position of particle
         mParticleData[i + 4] = ( ( float ) ( ( int ) ( Math.random() * 10000 ) % 10000 ) / 40000.0f ) - 0.125f;
         mParticleData[i + 5] = ( ( float ) ( ( int ) ( Math.random() * 10000 ) % 10000 ) / 40000.0f ) - 0.125f;
         mParticleData[i + 6] = ( ( float ) ( ( int ) ( Math.random() * 10000 ) % 10000 ) / 40000.0f ) - 0.125f;
      }

      mParticles = ByteBuffer.allocateDirect ( mParticleData.length * 4 )
                   .order ( ByteOrder.nativeOrder() ).asFloatBuffer();
      mParticles.put ( mParticleData ).position ( 0 );
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
      String vShaderStr =
         "#version 300 es                                      \n" +
         "uniform float u_time;                                \n" +
         "uniform vec3 u_centerPosition;                       \n" +
         "layout(location = 0) in float a_lifetime;            \n" +
         "layout(location = 1) in vec3 a_startPosition;        \n" +
         "layout(location = 2) in vec3 a_endPosition;          \n" +
         "out float v_lifetime;                                \n" +
         "void main()                                          \n" +
         "{                                                    \n" +
         "  if ( u_time <= a_lifetime )                        \n" +
         "  {                                                  \n" +
         "    gl_Position.xyz = a_startPosition +              \n" +
         "                      (u_time * a_endPosition);      \n" +
         "    gl_Position.xyz += u_centerPosition;             \n" +
         "    gl_Position.w = 1.0;                             \n" +
         "  }                                                  \n" +
         "  else                                               \n" +
         "     gl_Position = vec4( -1000, -1000, 0, 0 );       \n" +
         "  v_lifetime = 1.0 - ( u_time / a_lifetime );        \n" +
         "  v_lifetime = clamp ( v_lifetime, 0.0, 1.0 );       \n" +
         "  gl_PointSize = ( v_lifetime * v_lifetime ) * 40.0; \n" +
         "}                                                    \n";

      String fShaderStr =
         "#version 300 es                                      \n" +
         "precision mediump float;                             \n" +
         "uniform vec4 u_color;                                \n" +
         "in float v_lifetime;                                 \n" +
         "layout(location = 0) out vec4 fragColor;             \n" +
         "uniform sampler2D s_texture;                         \n" +
         "void main()                                          \n" +
         "{                                                    \n" +
         "  vec4 texColor;                                     \n" +
         "  texColor = texture( s_texture, gl_PointCoord );    \n" +
         "  fragColor = vec4( u_color ) * texColor;            \n" +
         "  fragColor.a *= v_lifetime;                         \n" +
         "}                                                    \n";

      // Load the shaders and get a linked program object
      mProgramObject = ESShader.loadProgram ( vShaderStr, fShaderStr );

      // Get the uniform locations
      mTimeLoc = GLES30.glGetUniformLocation ( mProgramObject, "u_time" );
      mCenterPositionLoc = GLES30.glGetUniformLocation ( mProgramObject, "u_centerPosition" );
      mColorLoc = GLES30.glGetUniformLocation ( mProgramObject, "u_color" );
      mSamplerLoc = GLES30.glGetUniformLocation ( mProgramObject, "s_texture" );

      GLES30.glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

      // Load the texture images from 'assets'
      mTextureId = loadTextureFromAsset ( "smoke.png" );

      // Initialize time to cause reset on first update
      mTime = 1.0f;
   }

   private void update()
   {
      if ( mLastTime == 0 )
      {
         mLastTime = SystemClock.uptimeMillis();
      }

      long curTime = SystemClock.uptimeMillis();
      long elapsedTime = curTime - mLastTime;
      float deltaTime = elapsedTime / 1000.0f;
      mLastTime = curTime;

      mTime += deltaTime;

      GLES30.glUseProgram ( mProgramObject );

      if ( mTime >= 1.0f )
      {
         float [] centerPos = new float[3];
         float [] color = new float[4];

         mTime = 0.0f;

         // Pick a new start location and color
         centerPos[0] = ( ( float ) ( ( int ) ( Math.random() * 10000 ) % 10000 ) / 10000.0f ) - 0.5f;
         centerPos[1] = ( ( float ) ( ( int ) ( Math.random() * 10000 ) % 10000 ) / 10000.0f ) - 0.5f;
         centerPos[2] = ( ( float ) ( ( int ) ( Math.random() * 10000 ) % 10000 ) / 10000.0f ) - 0.5f;

         GLES30.glUniform3f ( mCenterPositionLoc, centerPos[0], centerPos[1], centerPos[2] );

         // Random color
         color[0] = ( ( float ) ( ( int ) ( Math.random() * 1000 ) % 10000 ) / 20000.0f ) + 0.5f;
         color[1] = ( ( float ) ( ( int ) ( Math.random() * 1000 ) % 10000 ) / 20000.0f ) + 0.5f;
         color[2] = ( ( float ) ( ( int ) ( Math.random() * 1000 ) % 10000 ) / 20000.0f ) + 0.5f;
         color[3] = 0.5f;

         GLES30.glUniform4f ( mColorLoc, color[0], color[1], color[2], color[3] );
      }

      // Load uniform time variable
      GLES30.glUniform1f ( mTimeLoc, mTime );
   }

   ///
   // Draw a triangle using the shader pair created in onSurfaceCreated()
   //
   public void onDrawFrame ( GL10 glUnused )
   {
      update();

      // Set the viewport
      GLES30.glViewport ( 0, 0, mWidth, mHeight );

      // Clear the color buffer
      GLES30.glClear ( GLES30.GL_COLOR_BUFFER_BIT );

      // Use the program object
      GLES30.glUseProgram ( mProgramObject );

      // Load the vertex attributes
      //[0]
      mParticles.position ( 0 );
      GLES30.glVertexAttribPointer ( ATTRIBUTE_LIFETIME_LOCATION, 1, GLES30.GL_FLOAT,
                                     false, PARTICLE_SIZE * ( 4 ),
                                     mParticles );

      //[1]
      mParticles.position ( 1 );
      GLES30.glVertexAttribPointer ( ATTRIBUTE_ENDPOSITION_LOCATION, 3, GLES30.GL_FLOAT,
                                     false, PARTICLE_SIZE * ( 4 ),
                                     mParticles );

      //[4]
      mParticles.position ( 4 );
      GLES30.glVertexAttribPointer ( ATTRIBUTE_STARTPOSITION_LOCATION, 3, GLES30.GL_FLOAT,
                                     false, PARTICLE_SIZE * ( 4 ),
                                     mParticles );


      GLES30.glEnableVertexAttribArray ( ATTRIBUTE_LIFETIME_LOCATION );
      GLES30.glEnableVertexAttribArray ( ATTRIBUTE_ENDPOSITION_LOCATION );
      GLES30.glEnableVertexAttribArray ( ATTRIBUTE_STARTPOSITION_LOCATION );

      // Blend particles
      GLES30.glEnable ( GLES30.GL_BLEND );
      GLES30.glBlendFunc ( GLES30.GL_SRC_ALPHA, GLES30.GL_ONE );

      // Bind the texture
      GLES30.glActiveTexture ( GLES30.GL_TEXTURE0 );
      GLES30.glBindTexture ( GLES30.GL_TEXTURE_2D, mTextureId );

      // Set the sampler texture unit to 0
      GLES30.glUniform1i ( mSamplerLoc, 0 );

      GLES30.glDrawArrays ( GLES30.GL_POINTS, 0, NUM_PARTICLES );
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

   // Uniform location
   private int mTimeLoc;
   private int mColorLoc;
   private int mCenterPositionLoc;
   private int mSamplerLoc;

   // Texture handle
   private int mTextureId;

   // Current time
   float time;

   // Additional Member variables
   private int mWidth;
   private int mHeight;
   private long mLastTime = 0;

   private final int NUM_PARTICLES = 1000;
   private final int PARTICLE_SIZE = 7;

   private final int ATTRIBUTE_LIFETIME_LOCATION      = 0;
   private final int ATTRIBUTE_STARTPOSITION_LOCATION = 1;
   private final int ATTRIBUTE_ENDPOSITION_LOCATION   = 2;

   // Particle vertex data
   private float [] mParticleData = new float[ NUM_PARTICLES * PARTICLE_SIZE ];

   private FloatBuffer mParticles;

   private float mTime;
   private Context mContext;
}
