#version 330 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

layout (shared,std140,row_major) uniform MatrixBlock {
    mat4 ViewMat;
    mat4 PerspMat;
    mat4 ModelViewMat;
    mat4 NormalMat;
};

layout (location=1) in vec4 vertex;
layout (location=2) in vec4 normal;
layout (location=3) in vec4 texCoord;

out vec4 eyeVertex;
out vec4 eyeNormal;

void main(){
    eyeVertex = ModelViewMat*vertex;
    eyeNormal = NormalMat*normal;
    gl_Position = PerspMat*eyeVertex;
}
