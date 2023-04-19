#version 120

attribute vec2 a_position;
attribute vec2 a_texCoord;

varying vec2 v_texCoord;

uniform mat4 u_viewProj;

void main(){
    gl_Position = u_viewProj * vec4(a_position,0.5f,1.0f);

    v_texCoord = a_texCoord;
}
