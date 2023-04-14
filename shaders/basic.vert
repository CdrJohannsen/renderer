#version 330 core

layout(location=0) in vec4 a_position;
layout(location=1) in vec2 a_texCoord;
layout(location=2) in vec4 a_color;

out vec4 v_color;
out vec2 v_texCoord;

uniform mat4 u_model;
uniform mat4 u_modelViewProj;

void main(){
    gl_Position = u_modelViewProj * a_position;
    v_color=a_position;
    v_texCoord=a_texCoord;
}
