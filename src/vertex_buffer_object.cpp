#include <GL/glew.h>
#include "../include/vertex_buffer_object.h"
#include "../include/macro_util.h"


VBO::VBO(GLenum target, GLenum usage, GLint layoutLocation, GLuint attribSize){
    this->target = target;
    this->usage  = usage ;
    this->layoutLocation = layoutLocation;
    this->attribSize = attribSize;
    glGenBuffers(1,&vboID);OPENGL_ERROR();
}

VBO::VBO(GLenum usage){
    this->target = GL_ELEMENT_ARRAY_BUFFER;
    this->usage = usage;
    this->layoutLocation = 0;
    this->attribSize = 0;
    glGenBuffers(1,&vboID);OPENGL_ERROR();
}

VBO::~VBO(){
    glDeleteBuffers(1,&vboID);OPENGL_ERROR();
}

bool VBO::updateVBO(void* buffer){
    glBindBuffer(target,vboID);OPENGL_ERROR();
    if(this->target == GL_ELEMENT_ARRAY_BUFFER){  
        std::vector<int>* vBuffer = static_cast<std::vector<int>*>(buffer);
        glBufferData(target,vBuffer->size()*sizeof(int),vBuffer->data(),usage);OPENGL_ERROR();
    }else{
        std::vector<CVec4>* vBuffer = static_cast<std::vector<CVec4>*>(buffer);
        glBufferData(target,vBuffer->size()*sizeof(CVec4),vBuffer->data(),usage);OPENGL_ERROR();
        glVertexAttribPointer(layoutLocation,attribSize,GL_FLOAT,GL_FALSE,0,NULL);OPENGL_ERROR();
        glEnableVertexAttribArray(layoutLocation);OPENGL_ERROR();
    }
    return true;//return true for now (might change later)
}

void VBO::bind(){
    glBindBuffer(target,vboID);OPENGL_ERROR();
}
