#version 330 core
layout(location = 0) in vec3 a_position;

out vec3 localPos;

uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
    localPos = a_position;
    localPos.y = -localPos.y;
    vec4 pos = u_projection * u_view * vec4(a_position, 1.0);
    gl_Position = pos.xyww;
}
