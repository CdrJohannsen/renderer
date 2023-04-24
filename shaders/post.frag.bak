#version 130
varying vec2 v_texCoord;

uniform sampler2D u_texture;
uniform sampler2D u_depth;

vec4 lerp(float a,float b,vec4 f){
    return f * (b - a) + a;
}

void main(){
    vec4 color = texture2D(u_texture,v_texCoord);
    vec4 depth = texture2D(u_depth,v_texCoord);
    float a = (color.r+color.g+color.b)/3;
    //gl_FragColor = vec4(a,a,a,1.0f);
    depth = depth*2-1;
    float near = 0.1f;
    float far = 1.0f;
    float linearDepth = (2.0 * near * far) / (far + near - depth.r * (far - near));
    if (v_texCoord.x>=1.5f){
        gl_FragColor = vec4(1.0)-vec4(1.0f)*linearDepth;
    } else{
        gl_FragColor = color;
    }
}
