#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec2 a_texCoord;

out vec3 v_position;
out vec2 v_texCoord;
out mat3 v_tbn;
out vec3 v_normal;

uniform mat4 u_modelViewProj;
uniform mat4 u_modelView;
uniform mat4 u_invModelView;

void main()
{
    gl_Position = u_modelViewProj * vec4(a_position, 1.0f);

    vec3 t = normalize(a_tangent);
    vec3 n = normalize(a_normal);
    t = normalize(t - dot(t,n)*n);
    vec3 b = normalize(cross(n,t));
    mat3 tbn = mat3(u_modelView) * mat3(t, b, n);
    //mat3 tbn = mat3(t,b,n);
    /*vec3 t = normalize(a_tangent);
    vec3 n = normalize(a_normal);
    t = normalize(t - dot(t,n)*n);
    vec3 b = normalize(cross(n,t));
    mat3 tbn = mat3(u_modelView) * transpose(mat3(t, b, n));*/
    v_tbn = tbn;

    v_normal = mat3(u_modelView) * a_normal ;

    v_position = vec3(u_modelView * vec4(a_position, 1.0f));
    v_texCoord = a_texCoord;
}
