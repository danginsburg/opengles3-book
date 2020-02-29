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
// Book:        OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:    Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:    0-321-93388-5
// ISBN-13:    978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:        http://www.opengles-book.com
//                http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// Simple_VertexShader.cpp
//
//     This is a simple example that draws a rotating cube in perspective
//     using a vertex shader to transform the object
//

#include <stdlib.h>
#include "esUtil.h"
#include "Wavefront.h"
#include <iostream>
using namespace std;
using namespace WaveFront;

typedef float vfloat4 __attribute__((ext_vector_type(4)));

typedef struct
{
    // Handle to a program object
    GLuint      programObject;

    // Uniform locations
    GLint       mvpLoc;

    // Vertex daata
    GLfloat     *vertices;
    GLuint      *indices;
    int         numIndices;
    vfloat4     lightPos;

    // Rotation angle
    GLfloat     angle;

    // MVP matrix
    ESMatrix    mvpMatrix;

    ObjModel    obj;
} UserData;

///
// Initialize the shader and program object
//
int Init ( ESContext *esContext)
{
    UserData *userData = (UserData*)(esContext->userData);
    ObjModel *obj = &(userData->obj);

    string vShaderStr =
        #include "shader.v"
        ;

    string fShaderStr =
        #include "shader.f"
        ;

    // Load the shaders and get a linked program object
    userData->programObject = esLoadProgram ( vShaderStr.c_str(), fShaderStr.c_str() );

    // Get the uniform locations
    userData->mvpLoc = glGetUniformLocation ( userData->programObject, "u_mvpMatrix" );

    obj->parse("bunny.obj");
    userData->vertices = obj->getVertexArray();
    userData->indices = obj->getIndexArray();
    userData->numIndices = obj->getNumTriangles()*3;
    printf("%d %d %p %p\n", obj->getNumVertices(), userData->numIndices, userData->vertices, userData->indices);

    // Starting rotation angle for the cube
    userData->angle = 0.0f;
    userData->lightPos = (vfloat4){ 0.0, 0.25, 0.25, 1.0 };

    glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
    glFrontFace( GL_CCW );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glEnable ( GL_CULL_FACE );
    return GL_TRUE;
}

///
// Update MVP matrix based on time
//
void Update ( ESContext *esContext, float deltaTime )
{
    UserData *userData = (UserData*)(esContext->userData);
    ESMatrix perspective;
    ESMatrix modelview;
    float     aspect;

    // Compute a rotation angle based on time to rotate the cube
    //userData->angle += ( deltaTime * 40.0f );

    if ( userData->angle >= 360.0f ){
        userData->angle -= 360.0f;
    }

    // Compute the window aspect ratio
    aspect = ( GLfloat ) esContext->width / ( GLfloat ) esContext->height;

    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity ( &perspective );
    esPerspective ( &perspective, 60.0f, aspect, 1.0f, 20.0f );

    // Generate a model view matrix to rotate/translate the cube
    esMatrixLoadIdentity ( &modelview );

    // Translate away from the viewer
    esTranslate ( &modelview, 0.0, -0.5, -1.5 );

    // Rotate the cube
    //esRotate ( &modelview, userData->angle, 1.0, 0.0, 1.0 );
    esScale( &modelview, 5.0, 5.0, 5.0 );

    // Compute the final MVP by multiplying the
    // modevleiw and perspective matrices together
    esMatrixMultiply ( &userData->mvpMatrix, &modelview, &perspective );
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext)
{
    UserData *userData = (UserData*)(esContext->userData);
    ObjModel *obj = &(userData->obj);

    // Set the viewport
    glViewport ( 0, 0, esContext->width, esContext->height );

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Use the program object
    glUseProgram ( userData->programObject );

    // Load the vertex position
    glVertexAttribPointer ( 0, 3, GL_FLOAT,
                            GL_FALSE, obj->getAttribeStride() * sizeof ( GLfloat ), userData->vertices );

    glEnableVertexAttribArray ( 0 );

    glVertexAttribPointer ( 1, 3, GL_FLOAT,
                            GL_FALSE, obj->getAttribeStride() * sizeof ( GLfloat ), userData->vertices + 3);

    glEnableVertexAttribArray ( 1 );

    // Set the light source location
    glVertexAttrib4f ( 2, userData->lightPos[0], userData->lightPos[1], userData->lightPos[2], userData->lightPos[3] );

    // Load the MVP matrix
    glUniformMatrix4fv ( userData->mvpLoc, 1, GL_FALSE, ( GLfloat * ) &userData->mvpMatrix.m[0][0] );

    // Draw the cube
    glDrawElements ( GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, userData->indices );
}

void KeyFunc( ESContext *esContext, unsigned char keyval, int dummy0, int dummy1)
{
    UserData *userData = (UserData*)(esContext->userData);
    switch(keyval){
        case 'q':
            userData->lightPos[0] += 0.05;
            break;
        case 'w':
            userData->lightPos[1] += 0.05;
            break;
        case 'e':
            userData->lightPos[2] += 0.05;
            break;
        case 'a':
            userData->lightPos[0] -= 0.05;
            break;
        case 's':
            userData->lightPos[1] -= 0.05;
            break;
        case 'd':
            userData->lightPos[2] -= 0.05;
            break;
    }
}

void MouseFunc( ESContext *esContext, ESMouseHandle *handle)
{
    switch(handle->event){
        case ES_MOUSE_BUTTON_DOWN:
            printf("button[%d] down at (%d, %d) clicks %d\n", handle->button, handle->x, handle->y, handle->clicks);
            break;
        case ES_MOUSE_BUTTON_UP:
            printf("button[%d] up at (%d, %d) clicks %d\n", handle->button, handle->x, handle->y, handle->clicks);
            break;
        case ES_MOUSE_MOTION:
            printf("moving MV(%d, %d) to (%d, %d)\n", handle->mv_x, handle->mv_y, handle->x, handle->y);
            break;
    }
}

///
// Cleanup
//
void Shutdown ( ESContext *esContext )
{
    UserData *userData = (UserData*)(esContext->userData);

    // Delete program object
    glDeleteProgram ( userData->programObject );
}


#ifdef __cplusplus
extern "C"{
#endif

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc ( sizeof ( UserData ) );

    esCreateWindow ( esContext, "Bunny TEST", 640, 480, ES_WINDOW_RGB | ES_WINDOW_DEPTH );

    if ( !Init ( esContext) ){
        return GL_FALSE;
    }

    esRegisterShutdownFunc ( esContext, Shutdown );
    esRegisterUpdateFunc ( esContext, Update );
    esRegisterDrawFunc ( esContext, Draw );
    esRegisterKeyFunc( esContext, KeyFunc );
    esRegisterMouseFunc( esContext, MouseFunc );

    return GL_TRUE;
}

#ifdef __cplusplus
}
#endif
