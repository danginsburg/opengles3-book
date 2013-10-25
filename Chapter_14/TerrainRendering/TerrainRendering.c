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
// TerrainRendering.c
//
//    Demonstrates rendering a terrain with vertex texture fetch
//
#include <stdlib.h>
#include <math.h>
#include "esUtil.h"

#define POSITION_LOC    0

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // Uniform locations
   GLint  mvpLoc;
   GLint  lightDirectionLoc;

   // Sampler location
   GLint samplerLoc;

   // Texture handle
   GLuint textureId;

   // VBOs
   GLuint positionVBO;
   GLuint indicesIBO;

   // Number of indices
   int    numIndices;

   // dimension of grid
   int    gridSize;

   // MVP matrix
   ESMatrix  mvpMatrix;
} UserData;

///
// Load texture from disk
//
GLuint LoadTexture (  void *ioContext, char *fileName )
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

   glTexImage2D ( GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, buffer );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

   free ( buffer );

   return texId;
}

///
// Initialize the MVP matrix
//
int InitMVP ( ESContext *esContext )
{
   ESMatrix perspective;
   ESMatrix modelview;
   float    aspect;
   UserData *userData = esContext->userData;

   // Compute the window aspect ratio
   aspect = ( GLfloat ) esContext->width / ( GLfloat ) esContext->height;

   // Generate a perspective matrix with a 60 degree FOV
   esMatrixLoadIdentity ( &perspective );
   esPerspective ( &perspective, 60.0f, aspect, 0.1f, 20.0f );

   // Generate a model view matrix to rotate/translate the terrain
   esMatrixLoadIdentity ( &modelview );

   // Center the terrain
   esTranslate ( &modelview, -0.5f, -0.5f, -0.7f );

   // Rotate
   esRotate ( &modelview, 45.0f, 1.0, 0.0, 0.0 );

   // Compute the final MVP by multiplying the
   // modelview and perspective matrices together
   esMatrixMultiply ( &userData->mvpMatrix, &modelview, &perspective );

   return TRUE;
}

///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   GLfloat *positions;
   GLuint *indices;

   UserData *userData = esContext->userData;
   const char vShaderStr[] =
      "#version 300 es                                      \n"
      "uniform mat4 u_mvpMatrix;                            \n"
      "uniform vec3 u_lightDirection;                       \n"
      "layout(location = 0) in vec4 a_position;             \n"
      "uniform sampler2D s_texture;                         \n"
      "out vec4 v_color;                                    \n"
      "void main()                                          \n"
      "{                                                    \n"
      "   // compute vertex normal from height map          \n"
      "   float hxl = textureOffset( s_texture,             \n"
      "                  a_position.xy, ivec2(-1,  0) ).w;  \n"
      "   float hxr = textureOffset( s_texture,             \n"
      "                  a_position.xy, ivec2( 1,  0) ).w;  \n"
      "   float hyl = textureOffset( s_texture,             \n"
      "                  a_position.xy, ivec2( 0, -1) ).w;  \n"
      "   float hyr = textureOffset( s_texture,             \n"
      "                  a_position.xy, ivec2( 0,  1) ).w;  \n"
      "   vec3 u = normalize( vec3(0.05, 0.0, hxr-hxl) );   \n"
      "   vec3 v = normalize( vec3(0.0, 0.05, hyr-hyl) );   \n"
      "   vec3 normal = cross( u, v );                      \n"
      "                                                     \n"
      "   // compute diffuse lighting                       \n"
      "   float diffuse = dot( normal, u_lightDirection );  \n"
      "   v_color = vec4( vec3(diffuse), 1.0 );             \n"
      "                                                     \n"
      "   // get vertex position from height map            \n"
      "   float h = texture ( s_texture, a_position.xy ).w; \n"
      "   vec4 v_position = vec4 ( a_position.xy,           \n"
      "                            h/2.5,                   \n"
      "                            a_position.w );          \n"
      "   gl_Position = u_mvpMatrix * v_position;           \n"
      "}                                                    \n";

   const char fShaderStr[] =
      "#version 300 es                                      \n"
      "precision mediump float;                             \n"
      "in vec4 v_color;                                     \n"
      "layout(location = 0) out vec4 outColor;              \n"
      "void main()                                          \n"
      "{                                                    \n"
      "  outColor = v_color;                                \n"
      "}                                                    \n";

   // Load the shaders and get a linked program object
   userData->programObject = esLoadProgram ( vShaderStr, fShaderStr );

   // Get the uniform locations
   userData->mvpLoc = glGetUniformLocation ( userData->programObject, "u_mvpMatrix" );
   userData->lightDirectionLoc = glGetUniformLocation ( userData->programObject,
                                                        "u_lightDirection" );

   // Get the sampler location
   userData->samplerLoc = glGetUniformLocation ( userData->programObject, "s_texture" );

   // Load the heightmap
   userData->textureId = LoadTexture ( esContext->platformData, "heightmap.tga" );

   if ( userData->textureId == 0 )
   {
      return FALSE;
   }

   // Generate the position and indices of a square grid for the base terrain
   userData->gridSize = 200;
   userData->numIndices = esGenSquareGrid ( userData->gridSize, &positions, &indices );

   // Index buffer for base terrain
   glGenBuffers ( 1, &userData->indicesIBO );
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->indicesIBO );
   glBufferData ( GL_ELEMENT_ARRAY_BUFFER, userData->numIndices * sizeof ( GLuint ),
                  indices, GL_STATIC_DRAW );
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
   free ( indices );

   // Position VBO for base terrain
   glGenBuffers ( 1, &userData->positionVBO );
   glBindBuffer ( GL_ARRAY_BUFFER, userData->positionVBO );
   glBufferData ( GL_ARRAY_BUFFER,
                  userData->gridSize * userData->gridSize * sizeof ( GLfloat ) * 3,
                  positions, GL_STATIC_DRAW );
   free ( positions );

   glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );

   return TRUE;
}

///
// Draw a flat grid
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   InitMVP ( esContext );

   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );

   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // Use the program object
   glUseProgram ( userData->programObject );

   // Load the vertex position
   glBindBuffer ( GL_ARRAY_BUFFER, userData->positionVBO );
   glVertexAttribPointer ( POSITION_LOC, 3, GL_FLOAT,
                           GL_FALSE, 3 * sizeof ( GLfloat ), ( const void * ) NULL );
   glEnableVertexAttribArray ( POSITION_LOC );

   // Bind the index buffer
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->indicesIBO );

   // Bind the height map
   glActiveTexture ( GL_TEXTURE0 );
   glBindTexture ( GL_TEXTURE_2D, userData->textureId );

   // Load the MVP matrix
   glUniformMatrix4fv ( userData->mvpLoc, 1, GL_FALSE, ( GLfloat * ) &userData->mvpMatrix.m[0][0] );

   // Load the light direction
   glUniform3f ( userData->lightDirectionLoc, 0.86f, 0.14f, 0.49f );

   // Set the height map sampler to texture unit to 0
   glUniform1i ( userData->samplerLoc, 0 );

   // Draw the grid
   glDrawElements ( GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, ( const void * ) NULL );
}

///
// Cleanup
//
void Shutdown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   glDeleteBuffers ( 1, &userData->positionVBO );
   glDeleteBuffers ( 1, &userData->indicesIBO );

   // Delete program object
   glDeleteProgram ( userData->programObject );
}


int esMain ( ESContext *esContext )
{
   esContext->userData = malloc ( sizeof ( UserData ) );

   esCreateWindow ( esContext, "TerrainRendering", 640, 480, ES_WINDOW_RGB | ES_WINDOW_DEPTH );

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }

   esRegisterShutdownFunc ( esContext, Shutdown );
   esRegisterDrawFunc ( esContext, Draw );

   return GL_TRUE;
}

