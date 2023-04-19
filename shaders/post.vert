#version 130

varying vec2 v_texCoord;

uniform sampler2D u_texture;
uniform sampler2D u_depth;

void main(){
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    v_texCoord.x = (x+1.0)*0.5;
    v_texCoord.y = (y+1.0)*0.5;
    gl_Position = vec4(x, y, 0, 1);
}
