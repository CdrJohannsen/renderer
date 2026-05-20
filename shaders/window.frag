#version 330 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gColor;
layout(location = 3) out vec3 gEmissive;
layout(location = 4) out vec3 gMRA;

in vec3 v_position;
in vec2 v_texCoord;
in vec3 v_normal;

uniform sampler2D u_window_texture;

void main()
{
    vec4 color = texture(u_window_texture, v_texCoord);

    if (color.a < 0.1){
        discard;
    }

    gPosition = v_position;
    if(!gl_FrontFacing){
        gNormal = -v_normal;
    }else{
        gNormal = v_normal;
    }
    gColor = color;
    // gEmissive = color.rgb;
    gEmissive = vec3(0);
    gMRA.r = 0;
    gMRA.g = 0.5;
    gMRA.b = 0;
}
