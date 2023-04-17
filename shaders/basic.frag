#version 330 core

layout(location = 0) out vec4 f_color;

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

struct DirectionalLight {
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
};

struct PointLight {
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;

    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    float innerCone;
    float outerCone;

    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
};

uniform Material u_material;
uniform DirectionalLight u_dir_light;
uniform PointLight u_point_light;
uniform SpotLight u_spot_light;

uniform sampler2D u_diffuse_map;
uniform sampler2D u_normal_map;
uniform sampler2D u_specular_map;

void main()
{
    // Vector from fragment to camera (camera always at 0,0,0)
    vec3 view = normalize(-v_position);

    vec3 normal;

    if (u_material.hasNormal) {
        normal = texture(u_normal_map, v_texCoord).rgb;
        normal = normalize(normal * 2.0f - 1.0f);
        normal = normalize(v_tbn * normal);
    } else {
        normal = v_normal;
        normal = vec3(1.0f);
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

    vec3 light = normalize(-u_dir_light.direction);
    vec3 reflection = reflect(u_dir_light.direction, normal);
    vec3 ambient = u_dir_light.ambient * diffuseColor.xyz;
    vec3 diffuse = u_dir_light.diffuse * max(dot(normal, light), 0.0) * diffuseColor.xyz;
    vec3 specular = u_dir_light.specular * pow(max(dot(reflection, view), 0.1), shininess/1.0f) * u_material.specular;


    light = normalize(u_point_light.position - v_position);
    reflection = reflect(-light, normal);
    float distance_light = length(-u_point_light.position - v_position);
    float attentuation = 1.0f / ((1.0f) + (u_point_light.linear * distance_light) + (u_point_light.quadratic * distance_light));
    ambient += attentuation * u_point_light.ambient * diffuseColor.xyz;
    diffuse += attentuation * u_point_light.diffuse * max(dot(normal, light), 0.0) * diffuseColor.xyz;
    specular += attentuation * u_point_light.specular * pow(max(dot(reflection, view), 0.00001), shininess/21.0f) * u_material.specular;

    light = normalize(u_spot_light.position - v_position);
    reflection = reflect(-light, normal);
    float theta = dot(light, u_spot_light.direction);
    float epsilon = u_spot_light.innerCone - u_spot_light.outerCone;
    float intensity = clamp((theta - u_spot_light.outerCone) / epsilon, 0.0f, 1.0f);
    if (theta > u_spot_light.outerCone){
        diffuse += intensity * u_spot_light.diffuse * max(dot(normal, light), 0.0) * diffuseColor.xyz;
        specular += intensity * u_spot_light.specular * pow(max(dot(reflection, view), 0.1), shininess/21.0f) * u_material.specular;
        ambient += u_spot_light.ambient * diffuseColor.xyz;
    } else {
        ambient += u_spot_light.ambient * diffuseColor.xyz;
    }

    //ambient = vec3(1.0);
    //diffuse = vec3(0.0);
    //specular = u_dir_light.specular;
    //ambient = vec3(pow(max(dot(reflection, view),0.1),3.0f)) * u_material.specular * u_spot_light.specular * intensity;
    f_color = vec4(ambient + diffuse + specular + u_material.emissive, 1.0f);
}
