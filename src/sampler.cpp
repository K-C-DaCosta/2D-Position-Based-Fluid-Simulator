#include "../include/sampler.h"
#include "../include/macro_util.h"
#include <stdio.h>
#include <stdlib.h>

/*I dont think this class is needed or does anything*/
Sampler::Sampler(){
    glGenSamplers(1,&samplerID);OPENGL_ERROR();
    glSamplerParameteri(samplerID,GL_TEXTURE_MIN_FILTER,GL_LINEAR);OPENGL_ERROR();
    glSamplerParameteri(samplerID,GL_TEXTURE_MAG_FILTER,GL_LINEAR);OPENGL_ERROR();
}

void Sampler::setTextureUnit(GLint sloc, GLuint unit){
    this->unit = unit;
    glUniform1i(sloc,unit);OPENGL_ERROR();
}

Sampler::~Sampler(){
    glDeleteSamplers(1,&samplerID);OPENGL_ERROR();
}

void Sampler::bind(GLuint unit){
    glBindSampler(unit,samplerID);OPENGL_ERROR();

}


