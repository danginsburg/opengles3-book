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
// MipMap2D.c
//
//    This is a simple example that demonstrates generating a mipmap chain
//    and rendering with it
//
#include <stdlib.h>
#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // Sampler location
   GLint samplerLoc;

   // Offset location
   GLint offsetLoc;

   // Texture handle
   GLuint textureId;

} UserData;


///
//  From an RGB8 source image, generate the next level mipmap
//
GLboolean GenMipMap2D ( GLubyte *src, GLubyte **dst, int srcWidth, int srcHeight, int *dstWidth, int *dstHeight )
{
   int x,
       y;
   int texelSize = 3;

   *dstWidth = srcWidth / 2;

   if ( *dstWidth <= 0 )
   {
      *dstWidth = 1;
   }

   *dstHeight = srcHeight / 2;

   if ( *dstHeight <= 0 )
   {
      *dstHeight = 1;
   }

   *dst = malloc ( sizeof ( GLubyte ) * texelSize * ( *dstWidth ) * ( *dstHeight ) );

   if ( *dst == NULL )
   {
      return GL_FALSE;
   }

   for ( y = 0; y < *dstHeight; y++ )
   {
      for ( x = 0; x < *dstWidth; x++ )
      {
         int srcIndex[4];
         float r = 0.0f,
               g = 0.0f,
               b = 0.0f;
         int sample;

         // Compute the offsets for 2x2 grid of pixels in previous
         // image to perform box filter
         srcIndex[0] =
            ( ( ( y * 2 ) * srcWidth ) + ( x * 2 ) ) * texelSize;
         srcIndex[1] =
            ( ( ( y * 2 ) * srcWidth ) + ( x * 2 + 1 ) ) * texelSize;
         srcIndex[2] =
            ( ( ( ( y * 2 ) + 1 ) * srcWidth ) + ( x * 2 ) ) * texelSize;
         srcIndex[3] =
            ( ( ( ( y * 2 ) + 1 ) * srcWidth ) + ( x * 2 + 1 ) ) * texelSize;

         // Sum all pixels
         for ( sample = 0; sample < 4; sample++ )
         {
            r += src[srcIndex[sample]];
            g += src[srcIndex[sample] + 1];
            b += src[srcIndex[sample] + 2];
         }

         // Average results
         r /= 4.0;
         g /= 4.0;
         b /= 4.0;

         // Store resulting pixels
         ( *dst ) [ ( y * ( *dstWidth ) + x ) * texelSize ] = ( GLubyte ) ( r );
         ( *dst ) [ ( y * ( *dstWidth ) + x ) * texelSize + 1] = ( GLubyte ) ( g );
         ( *dst ) [ ( y * ( *dstWidth ) + x ) * texelSize + 2] = ( GLubyte ) ( b );
      }
   }

   return GL_TRUE;
}

///
//  Generate an RGB8 checkerboard image
//
GLubyte *GenCheckImage ( int width, int height, int checkSize )
{
   int x,
       y;
   GLubyte *pixels = malloc ( width * height * 3 );

   if ( pixels == NULL )
   {
      return NULL;
   }

   for ( y = 0; y < height; y++ )
      for ( x = 0; x < width; x++ )
      {
         GLubyte rColor = 0;
         GLubyte bColor = 0;

         if ( ( x / checkSize ) % 2 == 0 )
         {
            rColor = 255 * ( ( y / checkSize ) % 2 );
            bColor = 255 * ( 1 - ( ( y / checkSize ) % 2 ) );
         }
         else
         {
            bColor = 255 * ( ( y / checkSize ) % 2 );
            rColor = 255 * ( 1 - ( ( y / checkSize ) % 2 ) );
         }

         pixels[ ( y * width + x ) * 3] = rColor;
         pixels[ ( y * width + x ) * 3 + 1] = 0;
         pixels[ ( y * width + x ) * 3 + 2] = bColor;
      }

   return pixels;
}

///
// Create a mipmapped 2D texture image
//
GLuint CreateMipMappedTexture2D( )
{
   // Texture object handle
   GLuint textureId;
   int    width = 256,
          height = 256;
   int    level;
   GLubyte *pixels;
   GLubyte *prevImage;
   GLubyte *newImage;

   pixels = GenCheckImage ( width, height, 8 );

   if ( pixels == NULL )
   {
      return 0;
   }

   // Generate a texture object
   glGenTextures ( 1, &textureId );

   // Bind the texture object
   glBindTexture ( GL_TEXTURE_2D, textureId );

   // Load mipmap level 0
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, width, height,
                  0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

   level = 1;
   prevImage = &pixels[0];

   while ( width > 1 && height > 1 )
   {
      int newWidth,
          newHeight;

      // Generate the next mipmap level
      GenMipMap2D ( prevImage, &newImage, width, height,
                    &newWidth, &newHeight );

      // Load the mipmap level
      glTexImage2D ( GL_TEXTURE_2D, level, GL_RGB,
                     newWidth, newHeight, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, newImage );

      // Free the previous image
      free ( prevImage );

      // Set the previous image for the next iteration
      prevImage = newImage;
      level++;

      // Half the width and height
      width = newWidth;
      height = newHeight;
   }

   free ( newImage );

   // Set the filtering mode
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

   return textureId;

}


///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   char vShaderStr[] =
      "#version 300 es                            \n"
      "uniform float u_offset;                    \n"
      "layout(location = 0) in vec4 a_position;   \n"
      "layout(location = 1) in vec2 a_texCoord;   \n"
      "out vec2 v_texCoord;                       \n"
      "void main()                                \n"
      "{                                          \n"
      "   gl_Position = a_position;               \n"
      "   gl_Position.x += u_offset;              \n"
      "   v_texCoord = a_texCoord;                \n"
      "}                                          \n";

   char fShaderStr[] =
      "#version 300 es                                     \n"
      "precision mediump float;                            \n"
      "in vec2 v_texCoord;                                 \n"
      "layout(location = 0) out vec4 outColor;             \n"
      "uniform sampler2D s_texture;                        \n"
      "void main()                                         \n"
      "{                                                   \n"
      "   outColor = texture( s_texture, v_texCoord );     \n"
      "}                                                   \n";

   // Load the shaders and get a linked program object
   userData->programObject = esLoadProgram ( vShaderStr, fShaderStr );

   // Get the sampler location
   userData->samplerLoc = glGetUniformLocation ( userData->programObject, "s_texture" );

   // Get the offset location
   userData->offsetLoc = glGetUniformLocation ( userData->programObject, "u_offset" );

   // Load the texture
   userData->textureId = CreateMipMappedTexture2D ();

   glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   GLfloat vVertices[] = { -0.5f,  0.5f, 0.0f, 1.5f,  // Position 0
                            0.0f,  0.0f,              // TexCoord 0 
                           -0.5f, -0.5f, 0.0f, 0.75f, // Position 1
                            0.0f,  1.0f,              // TexCoord 1
                            0.5f, -0.5f, 0.0f, 0.75f, // Position 2
                            1.0f,  1.0f,              // TexCoord 2
                            0.5f,  0.5f, 0.0f, 1.5f,  // Position 3
                            1.0f,  0.0f               // TexCoord 3
                         };
   GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );

   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( userData->programObject );

   // Load the vertex position
   glVertexAttribPointer ( 0, 4, GL_FLOAT,
                           GL_FALSE, 6 * sizeof ( GLfloat ), vVertices );
   // Load the texture coordinate
   glVertexAttribPointer ( 1, 2, GL_FLOAT,
                           GL_FALSE, 6 * sizeof ( GLfloat ), &vVertices[4] );

   glEnableVertexAttribArray ( 0 );
   glEnableVertexAttribArray ( 1 );

   // Bind the texture
   glActiveTexture ( GL_TEXTURE0 );
   glBindTexture ( GL_TEXTURE_2D, userData->textureId );

   // Set the sampler texture unit to 0
   glUniform1i ( userData->samplerLoc, 0 );

   // Draw quad with nearest sampling
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glUniform1f ( userData->offsetLoc, -0.6f );
   glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );

   // Draw quad with trilinear filtering
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
   glUniform1f ( userData->offsetLoc, 0.6f );
   glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
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
   esContext->userData = malloc ( sizeof ( UserData ) );

   esCreateWindow ( esContext, "MipMap 2D", 320, 240, ES_WINDOW_RGB );

   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }

   esRegisterDrawFunc ( esContext, Draw );
   esRegisterShutdownFunc ( esContext, ShutDown );

   return GL_TRUE;
}
