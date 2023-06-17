#version 330 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gColorSpec;

in vec3 v_position;
in vec2 v_texCoord;
in mat3 v_tbn;
in vec3 v_normal;

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;
    bool hasDiffuse;
    bool hasNormal;
    bool hasSpecular;
};

uniform Material u_material;

uniform sampler2D u_diffuse_map;
uniform sampler2D u_normal_map;
uniform sampler2D u_specular_map;

void main()
{
    // Vector from fragment to camera (camera always at 0,0,0)
    vec3 view = normalize(-v_position);

    vec3 normal;
    if (u_material.hasNormal) {
        vec3 normal_t = texture(u_normal_map, v_texCoord).rgb;
        normal_t = normalize(normal_t * 2.0f - 1.0f);
        normal = normalize(v_tbn * normal_t);
    } else {
        normal = v_normal;
        //normal = vec3(1.0f);
    }

    vec4 diffuseColor;
    if (u_material.hasDiffuse) {
        diffuseColor = texture(u_diffuse_map, v_texCoord);;
    } else {
        diffuseColor = vec4(u_material.diffuse,1.0f);
    }

    float shininess;
    if (u_material.hasSpecular) {
        shininess = texture(u_specular_map,v_texCoord).r;
    } else {
        shininess = u_material.shininess;
    }

    if (diffuseColor.w < 0.9){
        discard;
    }
    gPosition = v_position;
    gNormal = normal;
    gColorSpec.rgb = diffuseColor.rgb;
    gColorSpec.a = shininess;

    /*
    gPosition = v_position;
    gNormal = vec3(1.0f);
    gColorSpec.rgb = vec3(1.0f);
    gColorSpec.a = 1.0f;
    */
}
