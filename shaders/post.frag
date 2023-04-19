#version 130
varying vec2 v_texCoord;

uniform sampler2D u_texture;
uniform sampler2D u_depth;

void main(){
    vec4 color = texture2D(u_texture,v_texCoord);
    vec4 depth = texture2D(u_depth,v_texCoord);
    float a = (color.r+color.g+color.b)/3;
    //gl_FragColor = vec4(a,a,a,1.0f);
    gl_FragColor = color;//*(1-pow(depth.r,32));
}
