#version 130
varying vec2 v_texCoord;

uniform sampler2D u_texture;
uniform sampler2D u_depth;

float lerp(float a,float b,float f){
    return a + f * (b - a);
}

void main(){
    vec4 color = texture2D(u_texture,v_texCoord);
    vec4 depth = texture2D(u_depth,v_texCoord);
    float a = (color.r+color.g+color.b)/3;
    //gl_FragColor = vec4(a,a,a,1.0f);
    depth = 0.2f + (1.0f-0.01) * depth;
    //gl_FragColor = vec4(1.0f)*(lerp(0.1f,1.0f,pow(depth.r,32)));
    gl_FragColor = 0.9f / depth;
}
