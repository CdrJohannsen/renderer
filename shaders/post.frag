#version 330 core
out vec4 FragColor;

in vec2 v_texCoord;

uniform sampler2D u_texture;
uniform sampler2D u_depth;

const float offset = 1.0 / 3000.0; 

void main(){
    vec4 color = texture2D(u_texture,v_texCoord);
    vec4 depth = texture2D(u_depth,v_texCoord);
    FragColor = color;
    return;

    vec2 offsets[9] = vec2[](
            vec2(-offset,  offset), // top-left
            vec2( 0.0f,    offset), // top-center
            vec2( offset,  offset), // top-right
            vec2(-offset,  0.0f),   // center-left
            vec2( 0.0f,    0.0f),   // center-center
            vec2( offset,  0.0f),   // center-right
            vec2(-offset, -offset), // bottom-left
            vec2( 0.0f,   -offset), // bottom-center
            vec2( offset, -offset)  // bottom-right    
            );

    float kernel[9] = float[](
            1, 1, 1,
            1,  -8.0, 1,
            1, 1, 1
            );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture2D(u_texture,v_texCoord + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++){
        col += sampleTex[i] * kernel[i];
    }

    FragColor = vec4(col,1.0f);
}
