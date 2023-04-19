#version 120

varying vec2 v_texCoord;

uniform sampler2D u_texture;

void main(){
    gl_FragColor = vec4(texture2D(u_texture, v_texCoord).a);
}
