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
// ParticleSystemTransformFeedback.c
//
//    This is an example that demonstrates a particle system
//    using transform feedback.
//
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include "esUtil.h"
#include "Noise3D.h"

#define NUM_PARTICLES   200
#define EMISSION_RATE   0.3f
#define ACCELERATION   -1.0f

#define ATTRIBUTE_POSITION      0
#define ATTRIBUTE_VELOCITY      1
#define ATTRIBUTE_SIZE          2
#define ATTRIBUTE_CURTIME       3
#define ATTRIBUTE_LIFETIME      4

typedef struct
{
   float position[2];
   float velocity[2];
   float size;
   float curtime;
   float lifetime;
} Particle;

typedef struct
{
   // Handle to a program object
   GLuint emitProgramObject;
   GLuint drawProgramObject;

   // Emit shader uniform locations
   GLint emitTimeLoc;
   GLint emitEmissionRateLoc;
   GLint emitNoiseSamplerLoc;

   // Draw shader uniform location
   GLint drawTimeLoc;
   GLint drawColorLoc;
   GLint drawAccelerationLoc;
   GLint samplerLoc;

   // Texture handles
   GLuint textureId;
   GLuint noiseTextureId;

   // Particle vertex data
   Particle particleData[ NUM_PARTICLES ];

   // Particle VBOs
   GLuint particleVBOs[2];

   // Index into particleVBOs (0 or 1) as to which is the source.
   // Ping-pong between the two VBOs
   GLuint curSrcIndex;

   // Current time
   float time;

   // synch object to synchronize the transform feedback results and the draw
   GLsync emitSync;

} UserData;

///
// Load texture from disk
//
GLuint LoadTexture ( void *ioContext, char *fileName )
{
   int width,
       height;
   char *buffer = esLoadTGA ( ioContext, fileName, &width, &height );
   GLuint texId;

   if ( buffer == NULL )
   {
      esLogMessage ( "Error loading (%s) image.\n", fileName );
      return 0;
   }

   glGenTextures ( 1, &texId );
   glBindTexture ( GL_TEXTURE_2D, texId );

   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

   free ( buffer );

   return texId;
}

void InitEmitParticles ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   char vShaderStr[] =
      "#version 300 es                                                     \n"
      "#define NUM_PARTICLES           200                                 \n"
      "#define ATTRIBUTE_POSITION      0                                   \n"
      "#define ATTRIBUTE_VELOCITY      1                                   \n"
      "#define ATTRIBUTE_SIZE          2                                   \n"
      "#define ATTRIBUTE_CURTIME       3                                   \n"
      "#define ATTRIBUTE_LIFETIME      4                                   \n"
      "uniform float u_time;                                               \n"
      "uniform float u_emissionRate;                                       \n"
      "uniform mediump sampler3D s_noiseTex;                               \n"
      "                                                                    \n"
      "layout(location = ATTRIBUTE_POSITION) in vec2 a_position;           \n"
      "layout(location = ATTRIBUTE_VELOCITY) in vec2 a_velocity;           \n"
      "layout(location = ATTRIBUTE_SIZE) in float a_size;                  \n"
      "layout(location = ATTRIBUTE_CURTIME) in float a_curtime;            \n"
      "layout(location = ATTRIBUTE_LIFETIME) in float a_lifetime;          \n"
      "                                                                    \n"
      "out vec2 v_position;                                                \n"
      "out vec2 v_velocity;                                                \n"
      "out float v_size;                                                   \n"
      "out float v_curtime;                                                \n"
      "out float v_lifetime;                                               \n"
      "                                                                    \n"
      "float randomValue( inout float seed )                               \n"
      "{                                                                   \n"
      "   float vertexId = float( gl_VertexID ) / float( NUM_PARTICLES );  \n"
      "   vec3 texCoord = vec3( u_time, vertexId, seed );                  \n"
      "   seed += 0.1;                                                     \n"
      "   return texture( s_noiseTex, texCoord ).r;                        \n"
      "}                                                                   \n"
      "void main()                                                         \n"
      "{                                                                   \n"
      "  float seed = u_time;                                              \n"
      "  float lifetime = a_curtime - u_time;                              \n"
      "  if( lifetime <= 0.0 && randomValue(seed) < u_emissionRate )       \n"
      "  {                                                                 \n"
      "     v_position = vec2( 0.0, -1.0 );                                \n"
      "     v_velocity = vec2( randomValue(seed) * 2.0 - 1.00,             \n"
      "                        randomValue(seed) * 1.4 + 1.0 );            \n"
      "     v_size = randomValue(seed) * 20.0 + 60.0;                      \n"
      "     v_curtime = u_time;                                            \n"
      "     v_lifetime = 2.0;                                              \n"
      "  }                                                                 \n"
      "  else                                                              \n"
      "  {                                                                 \n"
      "     v_position = a_position;                                       \n"
      "     v_velocity = a_velocity;                                       \n"
      "     v_size = a_size;                                               \n"
      "     v_curtime = a_curtime;                                         \n"
      "     v_lifetime = a_lifetime;                                       \n"
      "  }                                                                 \n"
      "  gl_Position = vec4( v_position, 0.0, 1.0 );                       \n"
      "}                                                                   \n";

   char fShaderStr[] =
      "#version 300 es                                      \n"
      "precision mediump float;                             \n"
      "layout(location = 0) out vec4 fragColor;             \n"
      "void main()                                          \n"
      "{                                                    \n"
      "  fragColor = vec4(1.0);                             \n"
      "}                                                    \n";

   userData->emitProgramObject = esLoadProgram ( vShaderStr, fShaderStr );

   {
      const char *feedbackVaryings[5] =
      {
         "v_position",
         "v_velocity",
         "v_size",
         "v_curtime",
         "v_lifetime"
      };

      // Set the vertex shader outputs as transform feedback varyings
      glTransformFeedbackVaryings ( userData->emitProgramObject, 5, feedbackVaryings, GL_INTERLEAVED_ATTRIBS );

      // Link program must occur after calling glTransformFeedbackVaryings
      glLinkProgram ( userData->emitProgramObject );

      // Get the uniform locations - this also needs to happen after glLinkProgram is called again so
      // that the uniforms that output to varyings are active
      userData->emitTimeLoc = glGetUniformLocation ( userData->emitProgramObject, "u_time" );
      userData->emitEmissionRateLoc = glGetUniformLocation ( userData->emitProgramObject, "u_emissionRate" );
      userData->emitNoiseSamplerLoc = glGetUniformLocation ( userData->emitProgramObject, "s_noiseTex" );
   }
}

///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   Particle particleData[ NUM_PARTICLES ];
   UserData *userData = ( UserData * ) esContext->userData;
   int i;

   char vShaderStr[] =
      "#version 300 es                                                     \n"
      "#define ATTRIBUTE_POSITION      0                                   \n"
      "#define ATTRIBUTE_VELOCITY      1                                   \n"
      "#define ATTRIBUTE_SIZE          2                                   \n"
      "#define ATTRIBUTE_CURTIME       3                                   \n"
      "#define ATTRIBUTE_LIFETIME      4                                   \n"
      "                                                                    \n"
      "layout(location = ATTRIBUTE_POSITION) in vec2 a_position;           \n"
      "layout(location = ATTRIBUTE_VELOCITY) in vec2 a_velocity;           \n"
      "layout(location = ATTRIBUTE_SIZE) in float a_size;                  \n"
      "layout(location = ATTRIBUTE_CURTIME) in float a_curtime;            \n"
      "layout(location = ATTRIBUTE_LIFETIME) in float a_lifetime;          \n"
      "                                                                    \n"
      "uniform float u_time;                                               \n"
      "uniform vec2 u_acceleration;                                        \n"
      "                                                                    \n"
      "void main()                                                         \n"
      "{                                                                   \n"
      "  float deltaTime = u_time - a_curtime;                             \n"
      "  if ( deltaTime <= a_lifetime )                                    \n"
      "  {                                                                 \n"
      "     vec2 velocity = a_velocity + deltaTime * u_acceleration;       \n"
      "     vec2 position = a_position + deltaTime * velocity;             \n"
      "     gl_Position = vec4( position, 0.0, 1.0 );                      \n"
      "     gl_PointSize = a_size * ( 1.0 - deltaTime / a_lifetime );      \n"
      "  }                                                                 \n"
      "  else                                                              \n"
      "  {                                                                 \n"
      "     gl_Position = vec4( -1000, -1000, 0, 0 );                      \n"
      "     gl_PointSize = 0.0;                                            \n"
      "  }                                                                 \n"
      "}";

   char fShaderStr[] =
      "#version 300 es                                      \n"
      "precision mediump float;                             \n"
      "layout(location = 0) out vec4 fragColor;             \n"
      "uniform vec4 u_color;                                \n"
      "uniform sampler2D s_texture;                         \n"
      "void main()                                          \n"
      "{                                                    \n"
      "  vec4 texColor;                                     \n"
      "  texColor = texture( s_texture, gl_PointCoord );    \n"
      "  fragColor = texColor * u_color;                    \n"
      "}                                                    \n";

   InitEmitParticles ( esContext );

   // Load the shaders and get a linked program object
   userData->drawProgramObject = esLoadProgram ( vShaderStr, fShaderStr );

   // Get the uniform locations
   userData->drawTimeLoc = glGetUniformLocation ( userData->drawProgramObject, "u_time" );
   userData->drawColorLoc = glGetUniformLocation ( userData->drawProgramObject, "u_color" );
   userData->drawAccelerationLoc = glGetUniformLocation ( userData->drawProgramObject, "u_acceleration" );
   userData->samplerLoc = glGetUniformLocation ( userData->drawProgramObject, "s_texture" );

   userData->time = 0.0f;
   userData->curSrcIndex = 0;

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

   userData->textureId = LoadTexture ( esContext->platformData, "smoke.tga" );

   if ( userData->textureId <= 0 )
   {
      return FALSE;
   }

   // Create a 3D noise texture for random values
   userData->noiseTextureId = Create3DNoiseTexture ( 128, 50.0 );

   // Initialize particle data
   for ( i = 0; i < NUM_PARTICLES; i++ )
   {
      Particle *particle = &particleData[i];
      particle->position[0] = 0.0f;
      particle->position[1] = 0.0f;
      particle->velocity[0] = 0.0f;
      particle->velocity[1] = 0.0f;
      particle->size = 0.0f;
      particle->curtime = 0.0f;
      particle->lifetime = 0.0f;
   }


   // Create the particle VBOs
   glGenBuffers ( 2, &userData->particleVBOs[0] );

   for ( i = 0; i < 2; i++ )
   {
      glBindBuffer ( GL_ARRAY_BUFFER, userData->particleVBOs[i] );
      glBufferData ( GL_ARRAY_BUFFER, sizeof ( Particle ) * NUM_PARTICLES, particleData, GL_DYNAMIC_COPY );
   }

   return TRUE;
}

void SetupVertexAttributes ( ESContext *esContext, GLuint vboID )
{
   glBindBuffer ( GL_ARRAY_BUFFER, vboID );
   glVertexAttribPointer ( ATTRIBUTE_POSITION, 2, GL_FLOAT,
                           GL_FALSE, sizeof ( Particle ),
                           ( const void * ) NULL );

   glVertexAttribPointer ( ATTRIBUTE_VELOCITY, 2, GL_FLOAT,
                           GL_FALSE, sizeof ( Particle ),
                           ( const void * ) offsetof ( Particle, velocity[0] ) );

   glVertexAttribPointer ( ATTRIBUTE_SIZE, 1, GL_FLOAT,
                           GL_FALSE, sizeof ( Particle ),
                           ( const void * ) offsetof ( Particle, size ) );

   glVertexAttribPointer ( ATTRIBUTE_CURTIME, 1, GL_FLOAT,
                           GL_FALSE, sizeof ( Particle ),
                           ( const void * ) offsetof ( Particle, curtime ) );

   glVertexAttribPointer ( ATTRIBUTE_LIFETIME, 1, GL_FLOAT,
                           GL_FALSE, sizeof ( Particle ),
                           ( const void * ) offsetof ( Particle, lifetime ) );

   glEnableVertexAttribArray ( ATTRIBUTE_POSITION );
   glEnableVertexAttribArray ( ATTRIBUTE_VELOCITY );
   glEnableVertexAttribArray ( ATTRIBUTE_SIZE );
   glEnableVertexAttribArray ( ATTRIBUTE_CURTIME );
   glEnableVertexAttribArray ( ATTRIBUTE_LIFETIME );
}

void EmitParticles ( ESContext *esContext, float deltaTime )
{
   UserData *userData = esContext->userData;
   GLuint srcVBO = userData->particleVBOs[ userData->curSrcIndex ];
   GLuint dstVBO = userData->particleVBOs[ ( userData->curSrcIndex + 1 ) % 2 ];

   glUseProgram ( userData->emitProgramObject );

   SetupVertexAttributes ( esContext, srcVBO );

   // Set transform feedback buffer
   glBindBuffer ( GL_TRANSFORM_FEEDBACK_BUFFER, dstVBO );
   glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0, dstVBO );

   // Turn off rasterization - we are not drawing
   glEnable ( GL_RASTERIZER_DISCARD );

   // Set uniforms
   glUniform1f ( userData->emitTimeLoc, userData->time );
   glUniform1f ( userData->emitEmissionRateLoc, EMISSION_RATE );

   // Bind the 3D noise texture
   glActiveTexture ( GL_TEXTURE0 );
   glBindTexture ( GL_TEXTURE_3D, userData->noiseTextureId );
   glUniform1i ( userData->emitNoiseSamplerLoc, 0 );

   // Emit particles using transform feedback
   glBeginTransformFeedback ( GL_POINTS );
   glDrawArrays ( GL_POINTS, 0, NUM_PARTICLES );
   glEndTransformFeedback();

   // Create a sync object to ensure transform feedback results are completed before the draw that uses them.
   userData->emitSync = glFenceSync ( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );

   // Restore state
   glDisable ( GL_RASTERIZER_DISCARD );
   glUseProgram ( 0 );
   glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0 );
   glBindBuffer ( GL_ARRAY_BUFFER, 0 );
   glBindTexture ( GL_TEXTURE_3D, 0 );

   // Ping pong the buffers
   userData->curSrcIndex = ( userData->curSrcIndex + 1 ) % 2;
}


///
//  Update time-based variables
//
void Update ( ESContext *esContext, float deltaTime )
{
   UserData *userData = ( UserData * ) esContext->userData;

   userData->time += deltaTime;

   EmitParticles ( esContext, deltaTime );
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // Block the GL server until transform feedback results are completed
   glWaitSync ( userData->emitSync, 0, GL_TIMEOUT_IGNORED );
   glDeleteSync ( userData->emitSync );

   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );

   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( userData->drawProgramObject );

   // Load the VBO and vertex attributes
   SetupVertexAttributes ( esContext, userData->particleVBOs[ userData->curSrcIndex ] );

   // Set uniforms
   glUniform1f ( userData->drawTimeLoc, userData->time );
   glUniform4f ( userData->drawColorLoc, 1.0f, 1.0f, 1.0f, 1.0f );
   glUniform2f ( userData->drawAccelerationLoc, 0.0f, ACCELERATION );

   // Blend particles
   glEnable ( GL_BLEND );
   glBlendFunc ( GL_SRC_ALPHA, GL_ONE );

   // Bind the texture
   glActiveTexture ( GL_TEXTURE0 );
   glBindTexture ( GL_TEXTURE_2D, userData->textureId );

   // Set the sampler texture unit to 0
   glUniform1i ( userData->samplerLoc, 0 );

   glDrawArrays ( GL_POINTS, 0, NUM_PARTICLES );
}

///
// Cleanup
//
void ShutDown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // Delete texture object
   glDeleteTextures ( 1, &userData->textureId );

   // Delete program object
   glDeleteProgram ( userData->drawProgramObject );
   glDeleteProgram ( userData->emitProgramObject );

   glDeleteBuffers ( 2, &userData->particleVBOs[0] );
}


int esMain ( ESContext *esContext )
{
   esContext->userData = malloc ( sizeof ( UserData ) );

   esCreateWindow ( esContext, "ParticleSystemTransformFeedback", 640, 480, ES_WINDOW_RGB );

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }

   esRegisterDrawFunc ( esContext, Draw );
   esRegisterUpdateFunc ( esContext, Update );
   esRegisterShutdownFunc ( esContext, ShutDown );

   return GL_TRUE;
}

