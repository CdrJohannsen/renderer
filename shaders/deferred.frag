#version 330 core

#define MAX_LIGHTS 20
out vec4 FragColor;

in vec2 TexCoords;

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
    vec3 ambient = light.ambient * diffuseColor.xyz;
    vec3 diffuse = light.diffuse * max(dot(normal, light_dir), 0.0) * diffuseColor.xyz;
    vec3 specular = light.specular * pow(max(dot(reflection, view), 0.1), shininess/1.0f) * diffuseColor;
    return ambient+diffuse+specular;
}

vec3 calcPointLight(PointLight light, vec3 diffuseColor, float shininess, vec3 normal, vec3 view, vec3 position){
    vec3 light_dir = normalize(light.position - position);
    vec3 reflection = reflect(-light_dir, normal);
    float distance_light = length(-light.position - position);
    float attentuation = 1.0f / (1.0f + (light.linear * distance_light) + (light.quadratic * distance_light));
    vec3 ambient = attentuation * light.ambient * diffuseColor;
    vec3 diffuse = attentuation * light.diffuse * max(dot(normal, light_dir), 0.0) * diffuseColor;
    vec3 specular = attentuation * light.specular * pow(max(dot(reflection, view), 0.00001), shininess/1.0f) * diffuseColor;
    return ambient+diffuse+specular;
}

vec3 calcSpotLight(SpotLight light, vec3 diffuseColor, float shininess, vec3 normal, vec3 view, vec3 position){
    vec3 light_dir = normalize(light.position - position);
    vec3 reflection = reflect(-light_dir, normal);
    float theta = dot(light_dir, normalize(light.direction));
    float epsilon = light.innerCone - light.outerCone;
    float distance_light = length(-light.position - position);
    float attentuation = 1.0f / (1.0f + (light.outerCone * distance_light));
    float intensity = clamp((theta - light.outerCone) / epsilon, 0.0f, 1.0f);
    vec3 ambient, specular, diffuse;
    if (theta > light.outerCone){
        diffuse = intensity * light.diffuse * max(dot(normal, light_dir), 0.0) * diffuseColor;
        specular = intensity * light.specular * pow(max(dot(reflection, view), 0.1), shininess/1.0f) * diffuseColor;
        ambient = light.ambient * diffuseColor;
    } else {
        ambient = light.ambient * diffuseColor;
    }
    return ambient+diffuse+specular;
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
    vec3 d0 = calcDirLight(u_dir_lights[0],Diffuse, Specular, Normal, view, FragPos);
    vec3 p0 = calcPointLight(u_point_lights[0],Diffuse, Specular, Normal, view, FragPos);
    vec3 p1 = calcPointLight(u_point_lights[1],Diffuse, Specular, Normal, view, FragPos);
    vec3 p2 = calcPointLight(u_point_lights[2],Diffuse, Specular, Normal, view, FragPos);
    vec3 s0 = calcSpotLight(u_spot_lights[0],Diffuse, Specular, Normal, view, FragPos);
    vec3 s1 = calcSpotLight(u_spot_lights[1],Diffuse, Specular, Normal, view, FragPos);
    vec3 s2 = calcSpotLight(u_spot_lights[2],Diffuse, Specular, Normal, view, FragPos);
    vec3 s3 = calcSpotLight(u_spot_lights[3],Diffuse, Specular, Normal, view, FragPos);
    vec3 s4 = calcSpotLight(u_spot_lights[4],Diffuse, Specular, Normal, view, FragPos);
    vec3 s5 = calcSpotLight(u_spot_lights[5],Diffuse, Specular, Normal, view, FragPos);
    vec3 s6 = calcSpotLight(u_spot_lights[6],Diffuse, Specular, Normal, view, FragPos);
    vec3 s7 = calcSpotLight(u_spot_lights[7],Diffuse, Specular, Normal, view, FragPos);
    vec3 s8 = calcSpotLight(u_spot_lights[8],Diffuse, Specular, Normal, view, FragPos);
    vec3 s9 = calcSpotLight(u_spot_lights[9],Diffuse, Specular, Normal, view, FragPos);
    vec3 s10 = calcSpotLight(u_spot_lights[10],Diffuse, Specular, Normal, view, FragPos);
    vec3 s11 = calcSpotLight(u_spot_lights[11],Diffuse, Specular, Normal, view, FragPos);
    vec3 s12 = calcSpotLight(u_spot_lights[12],Diffuse, Specular, Normal, view, FragPos);
    vec3 s13 = calcSpotLight(u_spot_lights[13],Diffuse, Specular, Normal, view, FragPos);
    color = s0+s1+s2+s3+s4+s5+s6+s7+s8+s9+s10+s11+s12+s13+d0+p0+p1+p2;
    
    gl_FragDepth = 1000.0f/(FragPos.z);
    const float gamma = 1.8;
    const float exposure = 0.1;
    vec3 mapped = vec3(1.0) - exp(-color * exposure);
    mapped = pow(mapped, vec3( 1.0 / gamma ));
    // reinhard tone mapping
    // vec3 mapped = color / (color + vec3(1.0));
    // gamma correction 
    // mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(mapped + Emissive, 1.0);
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
