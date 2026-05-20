#version 330 core
layout(location = 0) in vec2 a_position;

out vec2 v_position;

uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
    v_position = a_position.xy;
    v_position.y = 1. - v_position.y * 4;
    v_position.x = v_position.x * 2.;
    v_position.x = float((gl_VertexID == 1) || (gl_VertexID == 2) || (gl_VertexID == 4));
    v_position.y = float((gl_VertexID == 1) || (gl_VertexID == 3) || (gl_VertexID == 4));
    gl_Position = vec4(a_position, -1, 1);
}
