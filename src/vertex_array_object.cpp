

#include <GL/glew.h>
#include "../include/vertex_array_object.h"
#include "../include/shape_util.h"
#include "../include/math_util.h"
VAO::VAO(){
    glGenVertexArrays(1,&vaoID);OPENGL_ERROR();
    glBindVertexArray(vaoID);OPENGL_ERROR();
    flags = 0;
}

VAO::VAO(uint8_t flags,GLenum usage){
    this->flags = flags;
    glGenVertexArrays(1,&vaoID);OPENGL_ERROR();
    glBindVertexArray(vaoID);OPENGL_ERROR();

    if(flags&VAO_SPHERE){
        initSphere(0.5,5,5);
    }else if(flags&VAO_PLANE){
        initPlane(64,CVec4(160,0,0,0),CVec4(0,0,-160,0),CVec4(-20,0,20,0));
    }else if(flags&VAO_CUBE){
        initCube(-1,1,1,-1,0,-6);
    }else if(flags&VAO_CYLINDER){
        initCylinder(128,1.5,1,{0,0,0,0});
    }else if(flags&VAO_CIRCLE){
        initCircle(128,2,1,{0,0,0,0});
    }

    addVBO(new VBO(GL_ARRAY_BUFFER,usage,LAYOUT_VERTEX ,4) ,&vertexBuffer);
    addVBO(new VBO(GL_ARRAY_BUFFER,usage,LAYOUT_NORMALS,4), &normalBuffer);

    if(uvBuffer.size() > 0){
        addVBO(new VBO(GL_ARRAY_BUFFER,usage,LAYOUT_TEXTURE_UV,4),&uvBuffer);
    }

    addVBO(new VBO(usage), &indexBuffer);
    update();

    MTHUTL::getBoundingBox(vertexBuffer,(Vec4*)&aabb_min,(Vec4*)&aabb_max);
}

VAO::VAO(uint8_t flags, GLenum usage, float l ,float r, float t, float b, float n, float f){
    this->flags = flags;
    glGenVertexArrays(1,&vaoID);OPENGL_ERROR();
    glBindVertexArray(vaoID);OPENGL_ERROR();

    initCube(l,r,t,b,n,f);

    addVBO(new VBO(GL_ARRAY_BUFFER,usage,LAYOUT_VERTEX ,4) ,&vertexBuffer);
    addVBO(new VBO(GL_ARRAY_BUFFER,usage,LAYOUT_NORMALS,4),&normalBuffer);
    addVBO(new VBO(usage), &indexBuffer);
    update();
}

VAO::VAO(uint8_t flags, GLenum usage, float radius , float normFactor, CVec4 disp){
    //this automatically chooses to create a cirlce without checking flags

    this->flags = flags;
    glGenVertexArrays(1,&vaoID);OPENGL_ERROR();
    glBindVertexArray(vaoID);OPENGL_ERROR();

    initCircle(128,radius,normFactor,disp);

    addVBO(new VBO(GL_ARRAY_BUFFER,usage,LAYOUT_VERTEX    ,4),&vertexBuffer);
    addVBO(new VBO(GL_ARRAY_BUFFER,usage,LAYOUT_NORMALS   ,4),&normalBuffer);
    addVBO(new VBO(GL_ARRAY_BUFFER,usage,LAYOUT_TEXTURE_UV,4),&uvBuffer);
    addVBO(new VBO(usage), &indexBuffer);
    update();
}


VAO::VAO(uint8_t flags, GLenum usage, float radius , float height){
    //this automatically chooses to create a cirlce without checking flags

    this->flags = flags;
    glGenVertexArrays(1,&vaoID);OPENGL_ERROR();
    glBindVertexArray(vaoID);OPENGL_ERROR();

    initCylinder(128,radius,height,{0,0,0,0});

    addVBO(new VBO(GL_ARRAY_BUFFER,usage,LAYOUT_VERTEX    ,4),&vertexBuffer);
    addVBO(new VBO(GL_ARRAY_BUFFER,usage,LAYOUT_NORMALS   ,4),&normalBuffer);
    addVBO(new VBO(usage), &indexBuffer);
    update();
}


VAO::~VAO(){
    glDeleteVertexArrays(1,&vaoID);OPENGL_ERROR();
    dataList.clear();
    indexBuffer.clear();
    vertexBuffer.clear();
    normalBuffer.clear();

    if( flags&VAO_DELETE_DATA ){
        for(uint32_t I = 0; I < vboList.size();I++){
            delete vboList[I];
        }
    }
}

void VAO::bind(){
    glBindVertexArray(vaoID);OPENGL_ERROR();
}

void VAO::renderObject(GLenum mode){
    bind();
    glDrawElements(mode,indexBuffer.size(),GL_UNSIGNED_INT,(void*)0);
}

std::vector<CVec4> *VAO::getVertexBuffer(){
    return &vertexBuffer;
}

std::vector<CVec4> *VAO::getNormalBuffer(){
    return &normalBuffer;
}

std::vector<int> *VAO::getIndexBuffer(){
    return &indexBuffer;
}

void VAO::initSphere(float radius, int zStep, int yStep){
    ShapeUtil::createSphere(radius,zStep,yStep,&indexBuffer,&vertexBuffer,&normalBuffer);
}

void VAO::initPlane(int n, CVec4 p, CVec4 q, CVec4 o){
    ShapeUtil::createPlane(n,p,q,o,&indexBuffer,&vertexBuffer,&normalBuffer);
}

void VAO::initCube(float l,float r,float t,float b, float n,float f){
    ShapeUtil::createCube(l,r,t,b,n,f,&indexBuffer,&vertexBuffer,&normalBuffer);
}

void VAO::initCylinder(uint32_t n, float radius, float height, CVec4 centerDisp){
    ShapeUtil::createCylinder(n,radius,height,centerDisp,&indexBuffer,&vertexBuffer,&normalBuffer);
}

void VAO::initCircle(uint32_t n, float radius, float normFactor, CVec4 disp){
    ShapeUtil::createCircle(n,radius,normFactor,disp,&indexBuffer,&vertexBuffer,&normalBuffer,&uvBuffer);
}

void VAO::update(){
    bind();
    for(uint32_t I = 0; I < vboList.size(); I++){
        vboList[I]->updateVBO(dataList[I]);
    }
}

void VAO::addVBO(VBO *vbo, std::vector<CVec4> *data){
    vboList.push_back(vbo);
    dataList.push_back(data);
}

void VAO::addVBO(VBO *vbo, std::vector<int> *data){
    vboList.push_back(vbo);
    dataList.push_back(data);
}
