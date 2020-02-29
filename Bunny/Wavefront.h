#ifndef _WAVEFRONT_H_
#define _WAVEFRONT_H_

#include <iostream>
#include <GLES3/gl3.h>
using namespace std;

namespace WaveFront
{
    class ObjModel
    {
    public:
        ObjModel() {} ;
        ~ObjModel();
        void parse(string file);
        int getNumVertices() { return numVertices; };
        int getNumTriangles() { return numTriangles; };
        int getAttribeStride() { return 6; }
        GLfloat* getVertexArray() { return vertexArray; };
        GLuint* getIndexArray() { return indexArray; };
        GLfloat* getNormalArray() { return normalArray; };

    private:
        int numVertices = 0;
        int numTriangles = 0;
        GLfloat *vertexArray = NULL;
        GLuint *indexArray = NULL;
        GLfloat *normalArray = NULL;
    };
};

#endif
