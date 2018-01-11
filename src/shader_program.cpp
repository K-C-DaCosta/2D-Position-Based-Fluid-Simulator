#include "../include/shader_program.h"
#include "../include/macro_util.h"
#include "../include/general_io_util.h"
#include <iostream>

ShaderProgram::ShaderProgram(std::string vert_path, std::string frag_path){
    programID = glCreateProgram();OPENGL_ERROR();
    shaderCount = 0;
    for(int I = 0; I < 4; I++)
        shaderList[I] = NULL;

    Shader *vertShader = new Shader(GL_VERTEX_SHADER  ,GIOUtil::loadTextFile(vert_path));
    Shader *fragShader = new Shader(GL_FRAGMENT_SHADER,GIOUtil::loadTextFile(frag_path));

    try{
        vertShader->compileShader();
        fragShader->compileShader();
        attachShader(vertShader);
        attachShader(fragShader);
        linkProgram();
    }catch(ShaderCompileException e){
        std::cout << e.what() << std::endl;

        std::cout << "Shader files that contain errors:" << std::endl;
        std::cout << vert_path << std::endl;
        std::cout << frag_path << std::endl;
    }catch(ProgramLinkException e){
        std::cout << e.what() << std::endl;

        std::cout << "Shader files that contain errors:" << std::endl;
        std::cout << vert_path << std::endl;
        std::cout << frag_path << std::endl;
    }
}

ShaderProgram::ShaderProgram(){
    programID = glCreateProgram();OPENGL_ERROR();
    shaderCount = 0;
    for(int I = 0; I < 4; I++)
        shaderList[I] = NULL;
}

ShaderProgram::~ShaderProgram(){
    for(int I = 0; I < 4; I++)
        delete shaderList[I];
    glDeleteProgram(programID);OPENGL_ERROR();

}

void ShaderProgram::attachShader(Shader *s){
    glAttachShader(programID,s->getID());OPENGL_ERROR();
    shaderList[shaderCount++] = s;
}

void ShaderProgram::linkProgram(){
    GLint result;
    const int BUFFER_SIZE = 2048;
    GLchar infoLog[BUFFER_SIZE];
    glLinkProgram(programID);OPENGL_ERROR();
    glGetProgramiv(programID,GL_LINK_STATUS,&result);
    if(result == GL_FALSE){
        glGetProgramiv(programID,GL_INFO_LOG_LENGTH,&result);
        glGetProgramInfoLog(programID,BUFFER_SIZE,&result,infoLog);OPENGL_ERROR();
        std::cout << infoLog << std::endl;
        throw ProgramLinkException();
    }
}

void ShaderProgram::useProgram(){
    glUseProgram(programID);OPENGL_ERROR();
}

GLuint ShaderProgram::getID(){
    return programID;
}
