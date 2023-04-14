#version 330 core

layout(location = 0) out vec4 f_color;

in vec4 v_color;
in vec2 v_texCoord;

uniform vec4 u_color;
uniform sampler2D u_texture;

void main(){
    f_color = v_color;
    //vec4 texColor = texture(u_texture,v_texCoord);
    //f_color = texColor;
}
