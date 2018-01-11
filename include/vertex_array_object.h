#include <GL/gl.h>
#include "vertex_buffer_object.h"

#ifndef VERTEX_ARRAY_OBJECT_H
#define VERTEX_ARRAY_OBJECT_H

//if set the VAO call will generate sphere geometry
#define VAO_SPHERE 0x01
// If set the VAO class will generate plane geometry
#define VAO_PLANE  0x02
// If set VAO will free all data and buffers on delete, otherwise only the VAO is deleted
#define VAO_DELETE_DATA 0x04
// If set the VAO class will generate cube geometry
#define VAO_CUBE 0x08

#define VAO_CYLINDER 0x10

#define VAO_CIRCLE 0x20


class VAO{
public:
    VAO();
    VAO(uint8_t flags, GLenum usage);
    VAO(uint8_t flags, GLenum usage, float radius , float normFactor, CVec4 disp);//circle
    VAO(uint8_t flags, GLenum usage, float l, float r, float t, float b, float n, float f);
    VAO(uint8_t flags, GLenum usage, float radius , float height); //cylinder creation
    ~VAO();
    void bind();
    void update();
    void renderObject(GLenum mode);
    std::vector<CVec4>* getVertexBuffer();
    std::vector<CVec4>* getNormalBuffer();
    std::vector<int>  * getIndexBuffer();
private:
    void addVBO(VBO* vbo,std::vector<CVec4>* data);
    void addVBO(VBO* vbo,std::vector<int>* data);
    void initSphere(float radius,int zStep,int yStep);
    void initPlane(int n, CVec4 p, CVec4 q, CVec4 o);
    void initCube(float l, float r, float t,float b, float n,float f);
    void initCylinder(uint32_t n, float radius, float height, CVec4 centerDisp);
    void initCircle(uint32_t n, float radius,float normFactor, CVec4 disp);

private:
    uint8_t flags;
    GLuint vaoID;
    std::vector<VBO*> vboList;
    std::vector<void*> dataList;
    std::vector<int>   indexBuffer;
    std::vector<CVec4> vertexBuffer;
    std::vector<CVec4> normalBuffer;
    std::vector<CVec4> uvBuffer;
    CVec4 aabb_min;
    CVec4 aabb_max;
};
#endif // VERTEX_ARRAY_OBJECT_H
