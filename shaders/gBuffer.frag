#version 330 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gColor;
layout(location = 3) out vec3 gEmissive;
layout(location = 4) out vec3 gMRA;

in vec3 v_position;
in vec2 v_texCoord;
in mat3 v_tbn;
in vec3 v_normal;

struct Material {
    vec3 albedo;
    vec3 normal;
    vec3 emissive;
    float metallic;
    float roughness;
    bool hasAlbedo;
    bool hasNormal;
    bool hasMetallic;
    bool hasRoughness;
    bool hasAo;
};

uniform Material u_material;

uniform sampler2D u_albedo_map;
uniform sampler2D u_normal_map;
uniform sampler2D u_metallic_map;
uniform sampler2D u_roughness_map;
uniform sampler2D u_ao_map;

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

    vec4 albedoColor;
    if (u_material.hasAlbedo) {
        albedoColor = texture(u_albedo_map, v_texCoord);
    } else {
        albedoColor = vec4(u_material.albedo,1.0f);
    }

    float metallic;
    if (u_material.hasMetallic) {
        metallic = texture(u_metallic_map,v_texCoord).r;
    } else {
        metallic = u_material.metallic;
    }

    float roughness;
    if (u_material.hasRoughness) {
        roughness = texture(u_roughness_map,v_texCoord).r;
    } else {
        roughness = u_material.roughness;
    }

    float ao;
    if (u_material.hasAo) {
        ao = texture(u_ao_map,v_texCoord).r;
    }

    if (albedoColor.w < 0.9){
        discard;
    }
    gPosition = v_position;
    gNormal = normal;
    gColor= albedoColor;
    gEmissive.rgb = u_material.emissive;
    gMRA.r = metallic;
    gMRA.g = roughness;
    gMRA.b = ao;
}
