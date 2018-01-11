#include <GL/glew.h>

#ifndef SAMPLER_H
#define SAMPLER_H

class Sampler{
public:
    Sampler();
    void setTextureUnit(GLint sloc, GLuint unit);
    ~Sampler();
    void bind();
    void bind(GLuint unit);
private:
    GLuint unit;
    GLuint samplerID;
};

#endif // SAMPLER_H
