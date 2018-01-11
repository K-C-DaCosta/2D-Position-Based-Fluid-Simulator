#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>

#ifndef SHADER_H
#define SHADER_H


struct ShaderCompileException : public std::exception{
    const char * what () const throw () {
        return "A shader compile Error occoured";
    }
};

class Shader{
public:
    Shader(GLenum flag,const char *path);
    Shader(GLenum flag,std::string source);
    ~Shader();
    GLuint getID();
    GLenum getFlag();
    void compileShader();
private:
    void attachSourceToShader();
private:
    std::string source;
    GLuint shaderID;
    GLenum flag;
};

#endif // SHADER_H
