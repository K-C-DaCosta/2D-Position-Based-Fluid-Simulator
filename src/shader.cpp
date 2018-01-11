#include "../include/shader.h"
#include "../include/macro_util.h"
#include <iostream>

Shader::Shader(GLenum flag, std::string source){
    this->flag = flag;
    this->source = source;
    this->shaderID = glCreateShader(flag); OPENGL_ERROR();
    attachSourceToShader();
}

Shader::~Shader(){

}

void Shader::attachSourceToShader(){
    GLchar* sourcePtr[1];
    sourcePtr[0] = (GLchar*)source.c_str();
    glShaderSource(shaderID,1,sourcePtr,NULL);OPENGL_ERROR();
}

void Shader::compileShader(){
    const int BUFFER_SIZE = 1024;
    GLint result;
    GLchar infoLog[BUFFER_SIZE];

    glCompileShader(shaderID); OPENGL_ERROR();
    glGetShaderiv(shaderID,GL_COMPILE_STATUS,&result); OPENGL_ERROR();

    if(result == GL_FALSE){
       glGetShaderiv(shaderID,GL_INFO_LOG_LENGTH,&result);      OPENGL_ERROR();
       glGetShaderInfoLog(shaderID,BUFFER_SIZE,&result,infoLog);OPENGL_ERROR();
       std::cout << infoLog << std::endl;
       throw ShaderCompileException();
    }
}

GLuint Shader::getID(){
    return shaderID;
}

GLenum Shader::getFlag(){
    return flag;
}

