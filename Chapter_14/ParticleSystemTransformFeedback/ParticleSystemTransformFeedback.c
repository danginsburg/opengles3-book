//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// ParticleSystemTransformFeedback.c
//
//    This is an example that demonstrates a particle system
//    using transform feedback.
//
#include <stdlib.h>
#include <math.h>
#include "esUtil.h"

#define NUM_PARTICLES   1000
#define EMISSION_RATE   0.005f // Duration in seconds between particle emissions
#define ACCELERATION    -9.8f  // Gravity

#define ATTRIBUTE_POSITION      0
#define ATTRIBUTE_VELOCITY      1
#define ATTRIBUTE_SIZE          2

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
   GLuint programObject;

   // Uniform location
   GLint timeLoc;
   GLint colorLoc;
   GLint centerPositionLoc;
   GLint samplerLoc;

   // Texture handle
   GLuint textureId;

   // Particle vertex data
   Particle particleData[ NUM_PARTICLES ];

   // Number of currently active particles
   int numParticles;

   // Track time since last emission
   float emissionTime;

   // Current time
   float time;

} UserData;

///
// Load texture from disk
//
GLuint LoadTexture ( char *fileName )
{
   int width,
       height;
   char *buffer = esLoadTGA ( fileName, &width, &height );
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


///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   UserData *userData = (UserData*) esContext->userData;
   
   GLbyte vShaderStr[] =
      "#version 300 es                                      \n"
      "uniform float u_time;                                \n"
      "uniform vec3 u_centerPosition;                       \n"
      "layout(location = 0) in vec2 a_position;             \n"
      "layout(location = 2) in float a_size;                \n"
      "void main()                                          \n"
      "{                                                    \n"
      "  gl_Position = vec4( a_position, 0.0, 1.0 );        \n"
      "  gl_PointSize = a_size;                             \n"
      "}";
      
   GLbyte fShaderStr[] =  
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

   // Load the shaders and get a linked program object
   userData->programObject = esLoadProgram ( vShaderStr, fShaderStr );
   
   // Get the uniform locations
   userData->timeLoc = glGetUniformLocation ( userData->programObject, "u_time" );
   userData->centerPositionLoc = glGetUniformLocation ( userData->programObject, "u_centerPosition" );
   userData->colorLoc = glGetUniformLocation ( userData->programObject, "u_color" );
   userData->samplerLoc = glGetUniformLocation ( userData->programObject, "s_texture" );

   userData->numParticles = 0;
   userData->emissionTime = 0.0f;

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

   userData->textureId = LoadTexture ( "smoke.tga" );
   if ( userData->textureId <= 0 )
   {
      return FALSE;
   }
   
   return TRUE;
}

void EmitParticles ( ESContext *esContext, float deltaTime )
{
   UserData *userData = (UserData*) esContext->userData;
   userData->emissionTime += deltaTime;
   while ( userData->emissionTime > EMISSION_RATE )
   {
      if ( userData->numParticles < NUM_PARTICLES )
      {
         Particle *particle = &userData->particleData[ userData->numParticles ];
         userData->numParticles++;

         particle->velocity[0] = ( rand() % 1000 ) / 1000.0f * 1.0f - 0.5f;
         particle->velocity[1] = ( rand() % 1000 ) / 1000.0f * 2.0f + 3.0f;

         particle->position[0] = 0.0f;
         particle->position[1] = -1.0f;

         particle->size = ( rand() % 1000 ) / 1000.0f * 2.5f + 15.0f;

         particle->curtime = 0.0f;
         particle->lifetime = 2.0f;
      }
      userData->emissionTime -= EMISSION_RATE;
   }
}

void UpdateParticles( ESContext *esContext, float deltaTime )
{
   UserData *userData = (UserData*) esContext->userData;
   int i;
   for( i = 0; i < userData->numParticles; i++ )
   {
      Particle *particle = &userData->particleData[i];

      particle->curtime += deltaTime;
      if ( particle->curtime > particle->lifetime )
      {
         if ( i < ( userData->numParticles - 1 ) )
         {
            userData->particleData[i] = userData->particleData[userData->numParticles-1];
         }
         userData->numParticles--;
      }
      else
      {
         particle->velocity[1] = particle->velocity[1] + deltaTime * ACCELERATION;

         particle->position[0] = particle->position[0] + deltaTime * particle->velocity[0];
         particle->position[1] = particle->position[1] + deltaTime * particle->velocity[1];
      }
   }
}

///
//  Update time-based variables
//
void Update ( ESContext *esContext, float deltaTime )
{
   UserData *userData = (UserData*) esContext->userData;
  
   userData->time += deltaTime;

   EmitParticles ( esContext, deltaTime );
   UpdateParticles ( esContext, deltaTime );

   /*glUseProgram ( userData->programObject );

   if ( userData->time >= 1.0f )
   {
      float centerPos[3];
      float color[4];

      userData->time = 0.0f;

      // Pick a new start location and color
      centerPos[0] = ( (float)(rand() % 10000) / 10000.0f ) - 0.5f;
      centerPos[1] = ( (float)(rand() % 10000) / 10000.0f ) - 0.5f;
      centerPos[2] = ( (float)(rand() % 10000) / 10000.0f ) - 0.5f;
      
      glUniform3fv ( userData->centerPositionLoc, 1, &centerPos[0] );

      // Random color
      color[0] = ( (float)(rand() % 10000) / 20000.0f ) + 0.5f;
      color[1] = ( (float)(rand() % 10000) / 20000.0f ) + 0.5f;
      color[2] = ( (float)(rand() % 10000) / 20000.0f ) + 0.5f;
      color[3] = 0.5;

      glUniform4fv ( userData->colorLoc, 1, &color[0] );
   }

   // Load uniform time variable
   glUniform1f ( userData->timeLoc, userData->time );*/
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
      
   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );
   
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( userData->programObject );

   // Load the vertex attributes
   glVertexAttribPointer ( ATTRIBUTE_POSITION, 2, GL_FLOAT, 
                           GL_FALSE, sizeof(Particle), 
                           &userData->particleData[0].position );
   
   glVertexAttribPointer ( ATTRIBUTE_SIZE, 1, GL_FLOAT, 
                           GL_FALSE, sizeof(Particle), 
                           &userData->particleData[0].size );
   
   
   glEnableVertexAttribArray ( ATTRIBUTE_POSITION );
   glEnableVertexAttribArray ( ATTRIBUTE_SIZE );

   glUniform4f( userData->colorLoc, 1.0f, 0.25f, 0.0f, 1.0f );

   // Blend particles
   glEnable ( GL_BLEND );
   glBlendFunc ( GL_SRC_ALPHA, GL_ONE );

   // Bind the texture
   glActiveTexture ( GL_TEXTURE0 );
   glBindTexture ( GL_TEXTURE_2D, userData->textureId );

   // Set the sampler texture unit to 0
   glUniform1i ( userData->samplerLoc, 0 );

   glDrawArrays( GL_POINTS, 0, userData->numParticles );
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
   glDeleteProgram ( userData->programObject );
}


int esMain ( ESContext *esContext )
{
   esContext->userData = malloc ( sizeof( UserData ) );

   esCreateWindow ( esContext, "ParticleSystemTransformFeedback", 640, 480, ES_WINDOW_RGB );
   
   if ( !Init ( esContext ) )
      return GL_FALSE;

   esRegisterDrawFunc ( esContext, Draw );
   esRegisterUpdateFunc ( esContext, Update );
   esRegisterShutdownFunc ( esContext, ShutDown );
   
   return GL_TRUE;
}
