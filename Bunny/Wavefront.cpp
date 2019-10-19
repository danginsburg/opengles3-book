#include <iostream>
#include <stdio.h>
#include "Wavefront.h"

using namespace WaveFront;

typedef float vfloat3 __attribute__((ext_vector_type(3)));

void ObjModel::parse(string file)
{
    FILE *fptr = fopen(file.c_str(), "r");
    char *line = NULL;
    size_t len;

    getline(&line, &len, fptr);

    getline(&line, &len, fptr);
    sscanf(line, "# vertex count = %d", &numVertices);
    getline(&line, &len, fptr);
    sscanf(line, "# face count = %d", &numTriangles);

    unsigned char *vcnt = NULL;
    if(numVertices > 0){
        vertexArray = new GLfloat[numVertices*6];
        vcnt = (unsigned char*)calloc(numVertices, 1);
        printf("allocate Vertices %d %p\n", numVertices, vertexArray);
        for(int i = 0; i < numVertices*6; i+=6){
            getline(&line, &len, fptr);
            sscanf(line, "v %f %f %f", vertexArray+i, vertexArray+i+1, vertexArray+i+2);
            /*
            *(vertexArray+i) *= 5.0f;
            *(vertexArray+i+1) *= 5.0f;
            *(vertexArray+i+2) *= 5.0f;
            */
        }
    }
    if(numTriangles > 0){
        indexArray = new GLuint[numTriangles*3];
        normalArray = new GLfloat[numTriangles*3];
        printf("allocate Indices %d %p %p\n", numTriangles*3, indexArray, normalArray);
        for(int i = 0; i < numTriangles*3; i+=3){
            getline(&line, &len, fptr);
            sscanf(line, "f %d %d %d", indexArray+i, indexArray+i+1, indexArray+i+2);
            *(indexArray+i) -= 1;
            *(indexArray+i+1) -= 1;
            *(indexArray+i+2) -= 1;

            #if 1
            //calculate normal
            {
                unsigned int vidx[3] = { *(indexArray+i), *(indexArray+i+1), *(indexArray+i+2) };
                //printf("%d %d %d\n", vidx[0], vidx[1], vidx[2]);
                vfloat3 v1 = (vfloat3){ *(vertexArray+vidx[0]*6), *(vertexArray+vidx[0]*6+1), *(vertexArray+vidx[0]*6+2) };
                vfloat3 v2 = (vfloat3){ *(vertexArray+vidx[1]*6), *(vertexArray+vidx[1]*6+1), *(vertexArray+vidx[1]*6+2) };
                vfloat3 v3 = (vfloat3){ *(vertexArray+vidx[2]*6), *(vertexArray+vidx[2]*6+1), *(vertexArray+vidx[2]*6+2) };
                //vfloat3 v1 = *((vfloat3*)( vertexArray + vidx[0]*6 )));
                //vfloat3 v2 = *((vfloat3*)( vertexArray + vidx[1]*6 ));
                //vfloat3 v3 = *((vfloat3*)( vertexArray + vidx[2]*6 ));
                #if 1
                vfloat3 c1 = v3 - v2;
                vfloat3 c2 = v1 - v2;
                vfloat3 normal = (vfloat3){ c1.y*c2.z - c1.z*c2.y, c1.z*c2.x - c1.x*c2.z, c1.x*c2.y - c1.y*c2.x };
                //printf("n %f %f %f\n", normal.x, normal.y, normal.z);

                vfloat3 n1 = (vfloat3){ *(vertexArray+vidx[0]*6+3), *(vertexArray+vidx[0]*6+4), *(vertexArray+vidx[0]*6+5) };
                vfloat3 n2 = (vfloat3){ *(vertexArray+vidx[1]*6+3), *(vertexArray+vidx[1]*6+4), *(vertexArray+vidx[1]*6+5) };
                vfloat3 n3 = (vfloat3){ *(vertexArray+vidx[2]*6+3), *(vertexArray+vidx[2]*6+4), *(vertexArray+vidx[2]*6+5) };
                
                n1 = (n1*((float)vcnt[vidx[0]]) + normal)/(float)(vcnt[vidx[0]]+1);
                n2 = (n2*((float)vcnt[vidx[1]]) + normal)/(float)(vcnt[vidx[1]]+1);
                n3 = (n3*((float)vcnt[vidx[2]]) + normal)/(float)(vcnt[vidx[2]]+1);

                *( vertexArray + vidx[0]*6 + 3 ) = n1.x;
                *( vertexArray + vidx[0]*6 + 4 ) = n1.y;
                *( vertexArray + vidx[0]*6 + 5 ) = n1.z;
                *( vertexArray + vidx[1]*6 + 3 ) = n2.x;
                *( vertexArray + vidx[1]*6 + 4 ) = n2.y;
                *( vertexArray + vidx[1]*6 + 5 ) = n2.z;
                *( vertexArray + vidx[2]*6 + 3 ) = n3.x;
                *( vertexArray + vidx[2]*6 + 4 ) = n3.y;
                *( vertexArray + vidx[2]*6 + 5 ) = n3.z;

                vcnt[vidx[0]]++;
                vcnt[vidx[1]]++;
                vcnt[vidx[2]]++;
                #endif
            }
            #endif
        }
        free(vcnt);
    }
}

ObjModel::~ObjModel()
{
    if(vertexArray)
        delete vertexArray;
    if(indexArray)
        delete indexArray;
}

#ifdef _TEST_
int main(int argc, char **argv)
{
    ObjModel model;
    model.parse(string(argv[1]));
    cout << model.getNumVertices() << " " << model.getNumTriangles() << endl;
    
    return 0;
}
#endif
