#include <stdint.h>
#include "image_util.h"
#include <GL/glew.h>

#ifndef TEXTURE_H
#define TEXTURE_H
class Texture{
public:
    Texture(GLenum target,const char * imagePath);
    ~Texture();
    void bind();
    void attachSampler(uint32_t samplerLocation,uint32_t textureUnit );
private:
    GLenum texTarg;
    GLuint texID;
    RGBpixmap imageData;
};

#endif // TEXTURE_H
