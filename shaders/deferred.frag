#version 330 core

#define MAX_LIGHTS 20
out vec4 FragColor;

in vec2 TexCoords;

struct DirectionalLight {
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 diffuse;
    vec3 specular;

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
};

uniform DirectionalLight u_dir_lights[MAX_LIGHTS];
uniform PointLight u_point_lights[MAX_LIGHTS];
uniform SpotLight u_spot_lights[MAX_LIGHTS];

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform sampler2D gEmissive;

vec3 calcDirLight(DirectionalLight light, vec3 diffuseColor, float shininess, vec3 normal, vec3 view, vec3 position){
    vec3 light_dir = normalize(-light.direction);
    vec3 reflection = reflect(light.direction, normal);
    vec3 diffuse = light.diffuse * max(dot(normal, light_dir), 0.0) * diffuseColor.xyz;
    vec3 specular = light.specular * pow(max(dot(reflection, view), 0.0), shininess/1.0f) * diffuseColor;
    return diffuse+specular;
}

vec3 calcPointLight(PointLight light, vec3 diffuseColor, float shininess, vec3 normal, vec3 view, vec3 position){
    vec3 light_dir = normalize(light.position - position);
    vec3 reflection = reflect(-light_dir, normal);
    float distance_light = length(-light.position + position);
    float attentuation = 1.0f / (1.0f + (light.linear * distance_light) + (light.quadratic * distance_light * distance_light));
    vec3 diffuse = attentuation * light.diffuse * max(dot(normal, light_dir), 0.0) * diffuseColor;
    vec3 specular = attentuation * light.specular * pow(max(dot(view, reflection), 0.0), shininess) * diffuseColor;
    return diffuse+specular;
}

vec3 calcSpotLight(SpotLight light, vec3 diffuseColor, float shininess, vec3 normal, vec3 view, vec3 position){
    vec3 light_dir = normalize(light.position - position);
    vec3 reflection = reflect(-light_dir, normal);
    float theta = dot(light_dir, normalize(light.direction));
    float epsilon = light.innerCone - light.outerCone;
    float distance_light = length(-light.position + position);
    float attentuation = 1.0f / (1.0f + (light.outerCone * distance_light));
    float intensity = clamp((theta - light.outerCone) / epsilon, 0.0f, 1.0f);
    vec3  specular, diffuse;
    if (theta > light.outerCone){
        diffuse = intensity * light.diffuse * max(dot(normal, light_dir), 0.0) * diffuseColor;
        specular = intensity * light.specular * pow(max(dot(reflection, view), 0.0), shininess) * diffuseColor;
    } else {
        diffuse = vec3(0.0);
        specular = vec3(0.0);
    }
    return diffuse+specular;
}

void main()
{
    // Vector from fragment to camera (camera always at 0,0,0)
    /*

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
    */

    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gColorSpec, TexCoords).rgb;
    float Specular = texture(gColorSpec, TexCoords).a;
    vec3 Emissive = texture(gEmissive, TexCoords).rgb;
    vec3 view = normalize(-FragPos);
    // f_color = vec4(color + u_material.emissive, 1.0f);
    vec3 color = vec3(0.0f);
    color += calcDirLight(u_dir_lights[0],Diffuse, Specular, Normal, view, FragPos);
    for (int i=0;i<=2;i++){
        color+=calcPointLight(u_point_lights[i],Diffuse, Specular, Normal, view, FragPos);
    }
    for (int i=0;i<=13;i++){
        color+=calcSpotLight(u_spot_lights[i],Diffuse, Specular, Normal, view, FragPos);
    }
    
    gl_FragDepth = 1000.0f/(FragPos.z);
    const float gamma = 2.1;
    const float exposure = 1.05;
    vec3 mapped = vec3(1.0) - exp(-color * exposure);
    // mapped = pow(mapped, vec3( 1.0 / gamma ));
    // reinhard tone mapping
    // vec3 mapped = color / (color + vec3(1.0));
    // gamma correction 
    // mapped = pow(mapped, vec3(1.0 / gamma));
    // color *= 16;
    vec3 x = max(vec3(0.0),color-0.004);
    vec3 retColor = (x*(6.2*x+.5))/(x*(6.2*x+1.7)+0.06);

  
    FragColor = vec4(retColor + Emissive, 1.0);
    /*
    if (color.r > 1.0){
        if (color.g > 1.0){
            if (color.b > 1.0){
                FragColor = vec4(1.0,0.0,0.0,1.0);
            }
        }
    }
    */
    return;

    
    if (gl_FragCoord.x < 960){
        if (gl_FragCoord.y < 540){
            FragColor = vec4(mapped,1.0f);
        }else{
            FragColor = vec4(Diffuse,1.0f);
        }
    }else if (gl_FragCoord.x > 960){
        if (gl_FragCoord.y < 540){
            FragColor = vec4(Normal,1.0f);
        }else{
            FragColor = vec4(color,1.0f);
        }
    }
    
    // FragColor = vec4(vec3(-FragPos.z),1.0f);
    // FragColor = vec4(Specular);
}
