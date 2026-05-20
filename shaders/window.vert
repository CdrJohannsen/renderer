#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;

out vec3 v_position;
out vec2 v_texCoord;
out vec3 v_normal;

uniform mat4 u_modelViewProj;
uniform mat4 u_modelView;

void main()
{
    gl_Position = u_modelViewProj * vec4(a_position, 1.0f);

    vec3 n = normalize(a_normal);

    v_normal = mat3(u_modelView) * n ;

    v_position = vec3(u_modelView * vec4(a_position, 1.0f));
    v_texCoord = a_texCoord;
}
