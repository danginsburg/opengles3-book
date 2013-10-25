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
// VertexArrayObjects.c
//
//    This example demonstrates drawing a primitive with
//    Vertex Array Objects (VAOs)
//
#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // VertexBufferObject Ids
   GLuint vboIds[2];

   // VertexArrayObject Id
   GLuint vaoId;

} UserData;


#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

#define VERTEX_STRIDE         ( sizeof(GLfloat) *     \
                                ( VERTEX_POS_SIZE +    \
                                  VERTEX_COLOR_SIZE ) )


int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   const char vShaderStr[] =
      "#version 300 es                            \n"
      "layout(location = 0) in vec4 a_position;   \n"
      "layout(location = 1) in vec4 a_color;      \n"
      "out vec4 v_color;                          \n"
      "void main()                                \n"
      "{                                          \n"
      "    v_color = a_color;                     \n"
      "    gl_Position = a_position;              \n"
      "}";


   const char fShaderStr[] =
      "#version 300 es            \n"
      "precision mediump float;   \n"
      "in vec4 v_color;           \n"
      "out vec4 o_fragColor;      \n"
      "void main()                \n"
      "{                          \n"
      "    o_fragColor = v_color; \n"
      "}" ;

   GLuint programObject;

   // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
   GLfloat vertices[3 * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE )] =
   {
      0.0f,  0.5f, 0.0f,        // v0
      1.0f,  0.0f, 0.0f, 1.0f,  // c0
      -0.5f, -0.5f, 0.0f,        // v1
      0.0f,  1.0f, 0.0f, 1.0f,  // c1
      0.5f, -0.5f, 0.0f,        // v2
      0.0f,  0.0f, 1.0f, 1.0f,  // c2
   };
   // Index buffer data
   GLushort indices[3] = { 0, 1, 2 };

   // Create the program object
   programObject = esLoadProgram ( vShaderStr, fShaderStr );

   if ( programObject == 0 )
   {
      return GL_FALSE;
   }

   // Store the program object
   userData->programObject = programObject;

   // Generate VBO Ids and load the VBOs with data
   glGenBuffers ( 2, userData->vboIds );

   glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[0] );
   glBufferData ( GL_ARRAY_BUFFER, sizeof ( vertices ),
                  vertices, GL_STATIC_DRAW );
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1] );
   glBufferData ( GL_ELEMENT_ARRAY_BUFFER, sizeof ( indices ),
                  indices, GL_STATIC_DRAW );

   // Generate VAO Id
   glGenVertexArrays ( 1, &userData->vaoId );

   // Bind the VAO and then setup the vertex
   // attributes
   glBindVertexArray ( userData->vaoId );

   glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[0] );
   glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1] );

   glEnableVertexAttribArray ( VERTEX_POS_INDX );
   glEnableVertexAttribArray ( VERTEX_COLOR_INDX );

   glVertexAttribPointer ( VERTEX_POS_INDX, VERTEX_POS_SIZE,
                           GL_FLOAT, GL_FALSE, VERTEX_STRIDE, ( const void * ) 0 );

   glVertexAttribPointer ( VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
                           GL_FLOAT, GL_FALSE, VERTEX_STRIDE,
                           ( const void * ) ( VERTEX_POS_SIZE * sizeof ( GLfloat ) ) );

   // Reset to the default VAO
   glBindVertexArray ( 0 );

   glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   return GL_TRUE;
}

void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   glViewport ( 0, 0, esContext->width, esContext->height );
   glClear ( GL_COLOR_BUFFER_BIT );
   glUseProgram ( userData->programObject );

   // Bind the VAO
   glBindVertexArray ( userData->vaoId );

   // Draw with the VAO settings
   glDrawElements ( GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, ( const void * ) 0 );

   // Return to the default VAO
   glBindVertexArray ( 0 );
}

void Shutdown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   glDeleteProgram ( userData->programObject );
   glDeleteBuffers ( 2, userData->vboIds );
   glDeleteVertexArrays ( 1, &userData->vaoId );
}

int esMain ( ESContext *esContext )
{
   esContext->userData = malloc ( sizeof ( UserData ) );

   esCreateWindow ( esContext, "VertexArrayObjects", 320, 240, ES_WINDOW_RGB );

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }

   esRegisterShutdownFunc ( esContext, Shutdown );
   esRegisterDrawFunc ( esContext, Draw );

   return GL_TRUE;
}
