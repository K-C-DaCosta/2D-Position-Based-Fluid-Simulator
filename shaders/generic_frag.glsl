#version 330 core
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

in vec4 eyeVertex;
in vec4 eyeNormal;

out vec4 color;

void main(){
    color = vec4(1,1,1,1);
}
