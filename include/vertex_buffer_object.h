#include "vector.h"
#include <vector>
#include <GL/gl.h>

#ifndef VERTEX_BUFFER_OBJECT_H
#define VERTEX_BUFFER_OBJECT_H
class VBO{
public:
    VBO(GLenum target, GLenum usage, GLint layoutLocation,GLuint attribSize);
    VBO(GLenum usage);
    ~VBO();
    bool updateVBO(void *buffer);
    void bind();
private:
    GLenum target;
    GLenum usage;
    GLuint vboID;
    GLuint attribSize;
    GLint layoutLocation;
};

#endif // VERTEX_BUFFER_OBJECT_H
