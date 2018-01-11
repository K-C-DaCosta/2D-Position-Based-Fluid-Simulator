#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "shader.h"

/*
    This is a simple shader utility class. Its job is to the book keeping
    for my shader programs
*/



#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

struct ProgramLinkException : public std::exception{
    const char * what () const throw () {
        return "A link error occoured";
    }
};

class ShaderProgram{
    public:
        ShaderProgram(std::string vert_path,std::string frag_path);
        ShaderProgram();
        ~ShaderProgram();
        GLuint getID();
        void attachShader(Shader *s);
        void linkProgram();
        void useProgram();
    private:
        GLuint programID;
        Shader* shaderList[4];
        int shaderCount;
};

#endif
