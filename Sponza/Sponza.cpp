
#include <unistd.h>
#include <stdlib.h>
#include "esUtil.h"
#include <iostream>

#include <fstream>
#include <string.h>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

static std::string modelpath = "./Sponza_Zombie/sponza.obj";

typedef float vfloat4 __attribute__((ext_vector_type(4)));

std::map<std::string, GLuint*> textureIdMap;

typedef struct
{
    // Handle to a program object
    GLuint      programObject;

    // Uniform locations
    GLint       mvpLoc;
    GLint       samplerLoc;
    GLuint      sampler;

#if 0
    // Vertex daata
    GLfloat     *vertices;
    GLuint      *indices;
    int         numIndices;
    vfloat4     lightPos;
#else
    const aiScene* scene;
    GLuint scene_list;
    aiVector3D scene_min;
    aiVector3D scene_max;
    aiVector3D scene_center;
    GLuint* textureIds;
#endif

    int         objIdx;
    GLuint      *vao;
    GLuint      *vbo_vertices;
    GLuint      *vbo_indices;
    GLuint      *vbo_texCoord;
    GLuint      *numVertices;
    GLuint      *texIDs;
    ESMatrix    *mvp;
    // MVP matrix
    GLfloat     angle;
    ESMatrix    mvpMatrix;
    bool        rotate;
    GLfloat     angle_x;
    GLfloat     angle_y;
    GLfloat     distant;

    //ObjModel    obj;
} UserData;

int mesh_walk (const struct aiNode* nd){
    int meshes = nd->mNumMeshes;
    for (int n = 0; n < nd->mNumChildren; ++n){
        meshes += mesh_walk(nd->mChildren[n]);
    }
    return meshes;
}

int mesh_init (UserData *userData, const struct aiScene *scene, const struct aiNode* nd, float scale){

    // TODO - calculate per-mesh MVP
    aiMatrix4x4 m = nd->mTransformation;

    aiMatrix4x4 m2;
    aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), m2);
    m = m * m2;

    // update transform
    m.Transpose();

    int meshes = nd->mNumMeshes;
    for (int n = 0; n < nd->mNumMeshes; n++){

        const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

        int numIndices = mesh->mNumFaces * 3;
        userData->numVertices[userData->objIdx] = numIndices;
        GLushort *indices = new GLushort[numIndices];
        for (int t = 0; t < mesh->mNumFaces; ++t) {
            const struct aiFace* face = &mesh->mFaces[t];
            if(face->mNumIndices != 3){
                printf("have non-triangle face\n");
                continue;
            }
            for (int i = 0; i < face->mNumIndices; i++) {
                indices[t*3 + i] = face->mIndices[i];
            }
        }
        GLfloat *texCoords = new GLfloat[mesh->mNumVertices * 2];
        for (int i = 0; i < mesh->mNumVertices; i++){
            texCoords[i*2] = mesh->mTextureCoords[0][i].x;
            texCoords[i*2+1] = 1 - mesh->mTextureCoords[0][i].y;
        }

        glBindBuffer ( GL_ARRAY_BUFFER, userData->vbo_vertices[userData->objIdx] );
        glBufferData ( GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(GLfloat)*3, mesh->mVertices, GL_STATIC_DRAW);
        glBindBuffer ( GL_ARRAY_BUFFER, userData->vbo_texCoord[userData->objIdx] );
        glBufferData ( GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(GLfloat)*2, texCoords, GL_STATIC_DRAW);
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vbo_indices[userData->objIdx]);
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLushort), indices, GL_STATIC_DRAW);


        glBindVertexArray ( userData->vao[userData->objIdx] );

        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vbo_indices[userData->objIdx]);
        glBindBuffer ( GL_ARRAY_BUFFER, userData->vbo_vertices[userData->objIdx] );
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*3, (const void*)0);

        glBindBuffer ( GL_ARRAY_BUFFER, userData->vbo_texCoord[userData->objIdx] );
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*2, (const void*)0);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        int texIndex = 0;
        aiString texPath;
        //contains filename of texture
        const aiMaterial *mtl = scene->mMaterials[mesh->mMaterialIndex];
        if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath)){
            //bind texture
            unsigned int texId = *textureIdMap[texPath.data];
            userData->texIDs[userData->objIdx] = texId;
        }

        glBindVertexArray ( 0 );

        userData->objIdx++;
        delete []indices;
        delete []texCoords;
    }
    for (int n = 0; n < nd->mNumChildren; ++n){
        meshes += mesh_init(userData, scene, nd->mChildren[n], scale);
    }
    return meshes;
}

int mesh_render (UserData *userData, const struct aiScene *scene, const struct aiNode* nd){
    int meshes = nd->mNumMeshes;
    for (int n = 0; n < nd->mNumMeshes; n++){
        const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
        int texIndex = 0;
        aiString texPath;
        //contains filename of texture
        const aiMaterial *mtl = scene->mMaterials[mesh->mMaterialIndex];
        if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath)){
            //bind texture
            unsigned int texId = *textureIdMap[texPath.data];
            printf("mesh[%d] texture[%u]\n", n, texId);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texId);
            glUniform1i(userData->samplerLoc, 0);
        }
        glBindVertexArray ( userData->vao[userData->objIdx] );
        glDrawElements ( GL_TRIANGLES, mesh->mNumFaces * 3, GL_UNSIGNED_SHORT, (const void*) 0 );
        glBindVertexArray ( 0 );
        userData->objIdx++;
    }
}

///
// Initialize the shader and program object
//
int Init ( ESContext *esContext)
{
    UserData *userData = (UserData*)(esContext->userData);

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
    userData->samplerLoc = glGetUniformLocation( userData->programObject, "s_texture" );
    userData->angle_x = 0;
    userData->angle_y = 0;
    userData->rotate = false;
    userData->distant = 0.5;

    Assimp::Importer importer;
    printf("load %s\n", modelpath.c_str());
    userData->scene = importer.ReadFile( modelpath, aiProcessPreset_TargetRealtime_Quality);
    
    if(!userData->scene){
        printf("model:%s import failed\n", modelpath.c_str());
        exit(1);
    }
    
    const aiScene* scene = userData->scene;
    for (int m = 0; m < scene->mNumMaterials; m++){
        int texIndex = 0;
        aiReturn texFound = AI_SUCCESS;
        aiString path;  // filename
        while (texFound == AI_SUCCESS){
            texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
            if(texFound == AI_SUCCESS){
                //printf("texture[%d] %s\n", texIndex, path.C_Str());
                textureIdMap[path.data] = NULL; //fill map with textures, pointers still NULL yet
                texIndex++;
            }
        }
    }
    
    int numTextures = textureIdMap.size();
    printf("Total %d Textures\n", numTextures);
    
    userData->textureIds = new GLuint[numTextures];
    glGenTextures(numTextures, userData->textureIds); /* Texture name generation */

    //load Textures
    std::map<std::string, GLuint*>::iterator itr = textureIdMap.begin();
    for (int i=0; i<numTextures; i++){
        std::string filename = (*itr).first;  // get filename
        (*itr).second =  &(userData->textureIds[i]);
        std::replace( filename.begin(), filename.end(), '\\', '/');
        std::string fileloc = "./Sponza_Zombie/" + filename;
        printf("%s\n", fileloc.c_str());
        itr++;
        int x, y, n;
        unsigned char *data = stbi_load(fileloc.c_str(), &x, &y, &n, STBI_rgb_alpha);
        if(data){
            // Binding of texture name
            glBindTexture(GL_TEXTURE_2D, userData->textureIds[i]);
            // Texture specification
            glTexImage2D(GL_TEXTURE_2D, 0, n, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);// Texture specification.
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
            glGenerateMipmap(GL_TEXTURE_2D);

            // we also want to be able to deal with odd texture dimensions
            glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
            glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
            glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
            glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
            stbi_image_free(data);
        } else {
            printf("Texture %s loading failed\n", fileloc.c_str());
            exit(0);
        }
    }

    //int meshes = mesh_init(userData, scene, scene->mRootNode, 1.0);
    int meshes = mesh_walk(scene->mRootNode);
    printf("total %d meshes\n", meshes);

    userData->vao = (GLuint*)malloc(sizeof(GLuint)*meshes);
    glGenVertexArrays(meshes, userData->vao);

    userData->vbo_indices = (GLuint*)malloc(sizeof(GLuint)*meshes);
    userData->vbo_vertices = (GLuint*)malloc(sizeof(GLuint)*meshes);
    userData->vbo_texCoord = (GLuint*)malloc(sizeof(GLuint)*meshes);
    glGenBuffers(meshes, userData->vbo_indices);
    glGenBuffers(meshes, userData->vbo_vertices);
    glGenBuffers(meshes, userData->vbo_texCoord);
    
    userData->numVertices = (GLuint*)malloc(sizeof(GLuint)*meshes);
    userData->texIDs = (GLuint*)malloc(sizeof(GLuint)*meshes);

    mesh_init(userData, scene, scene->mRootNode, 1.0);

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
    esPerspective ( &perspective, 60.0f, aspect, 0.5f, 30.0f );

    // Generate a model view matrix to rotate/translate the cube
    esMatrixLoadIdentity ( &modelview );

    // Translate away from the viewer
    esTranslate ( &modelview, 0.0, -0.3, -(userData->distant) );

    // Rotate the cube
    //esRotate ( &modelview, -90, 0.0, 1.0, 0.0 );
    esRotate ( &modelview, userData->angle_y, 1.0, 0.0, 0.0 );
    esRotate ( &modelview, userData->angle_x, 0.0, 1.0, 0.0 );
    esScale( &modelview, 0.001, 0.001, 0.001 );

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

    // Set the viewport
    glViewport ( 0, 0, esContext->width, esContext->height );

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Use the program object
    glUseProgram ( userData->programObject );

    // Load the MVP matrix
    glUniformMatrix4fv ( userData->mvpLoc, 1, GL_FALSE, ( GLfloat * ) &userData->mvpMatrix.m[0][0] );

    // Draw the cube
    //glDrawElements ( GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, userData->indices );

    for(int i = 0; i < userData->objIdx; i++){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, userData->texIDs[i]);
        glUniform1i(userData->samplerLoc, 0);

        glBindVertexArray ( userData->vao[i] );
        glDrawElements ( GL_TRIANGLES, userData->numVertices[i], GL_UNSIGNED_SHORT, (const void*) 0 );
        glBindVertexArray ( 0 );
    }
}

void KeyFunc( ESContext *esContext, unsigned char keyval, int dummy0, int dummy1)
{
    UserData *userData = (UserData*)(esContext->userData);
    switch(keyval){
        case 'q':
            //userData->lightPos[0] += 0.05;
            break;
        case 'w':
            //userData->lightPos[1] += 0.05;
            break;
        case 'e':
            //userData->lightPos[2] += 0.05;
            break;
        case 'a':
            //userData->lightPos[0] -= 0.05;
            break;
        case 's':
            //userData->lightPos[1] -= 0.05;
            break;
        case 'd':
            //userData->lightPos[2] -= 0.05;
            break;
    }
}

void MouseFunc( ESContext *esContext, ESMouseHandle *handle)
{
    UserData *userData = (UserData*)(esContext->userData);
    switch(handle->event){
        case ES_MOUSE_BUTTON_DOWN:
            userData->rotate = true;
            break;
        case ES_MOUSE_BUTTON_UP:
            userData->rotate = false;
            break;
        case ES_MOUSE_MOTION:
            if(userData->rotate){
                userData->angle_x -= handle->mv_x * 0.1;
                userData->angle_x = userData->angle_x > 360 ? 
                    userData->angle_x - 360 : 
                        userData->angle_x < 0 ? 
                            userData->angle_x + 360 : 
                            userData->angle_x;
                userData->angle_y -= handle->mv_y * 0.1;
                userData->angle_y = userData->angle_y > 360 ? 
                    userData->angle_y - 360 : 
                        userData->angle_y < 0 ? 
                            userData->angle_y + 360 : 
                            userData->angle_y;
            }
            break;
        case ES_MOUSE_WHEEL:
            userData->distant -= handle->y * 0.02;
            userData->distant = userData->distant < 0.1 ? 0.1 : userData->distant > 8 ? 8 : userData->distant;
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

    esCreateWindow ( esContext, "Sponza", 1280, 720, ES_WINDOW_RGB | ES_WINDOW_DEPTH );

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
