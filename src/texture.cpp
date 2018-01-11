#include <GL/glew.h>
#include "../include/texture.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/macro_util.h"


Texture::Texture(GLenum target, const char *imagePath){
    IMGUTL::readBMPFile(&imageData,imagePath);
    texTarg = target;
    glGenTextures(1,&texID); OPENGL_ERROR();
    glBindTexture(target,texID);OPENGL_ERROR();


    glTexImage2D(target,0,GL_RGB,imageData.nCols,imageData.nRows,0,GL_RGB,GL_UNSIGNED_BYTE,(void*)imageData.pixel);OPENGL_ERROR();
    glGenerateMipmap(target);
    glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(target,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(target,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);OPENGL_ERROR();
    glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 5);OPENGL_ERROR();

}

Texture::~Texture(){
    free(imageData.pixel);
    glDeleteTextures(1,&texID);OPENGL_ERROR();
}

void Texture::bind(){
    glBindTexture(texTarg,texID);OPENGL_ERROR();
}

void Texture::attachSampler(uint32_t samplerLocation, uint32_t textureUnit){
    glUniform1i(samplerLocation,textureUnit);OPENGL_ERROR();
}

