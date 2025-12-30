#version 330 core

#define MAX_LIGHTS 20
out vec4 FragColor;

in vec2 TexCoords;

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;

    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    float innerCone;
    float outerCone;

    vec3 color;
};

const float PI = 3.14159265359;

const float LIGHT_MULTIPLIER=50.0f;

uniform DirectionalLight u_dir_lights[MAX_LIGHTS];
uniform PointLight u_point_lights[MAX_LIGHTS];
uniform SpotLight u_spot_lights[MAX_LIGHTS];

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColor;
uniform sampler2D gEmissive;
uniform sampler2D gMRA;

// vec3 calcDirLight(DirectionalLight light, vec3 diffuseColor, float shininess, vec3 normal, vec3 view, vec3 position){
//     vec3 light_dir = normalize(-light.direction);
//     vec3 reflection = reflect(light.direction, normal);
//     vec3 diffuse = light.diffuse * max(dot(normal, light_dir), 0.0) * diffuseColor.xyz;
//     vec3 specular = light.specular * pow(max(dot(reflection, view), 0.0), shininess/1.0f) * diffuseColor;
//     return diffuse+specular;
// }
//

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 calcPointLight(PointLight light, vec3 albedo, float metallic,float roughness,float ao, vec3 normal, vec3 view, vec3 position){
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    vec3 light_dir = normalize(light.position - position);
    vec3 H=normalize(view+light_dir);
    float distance_light = length(light.position - position);
    float attenuation = 1.0 / (distance_light*distance_light);
    vec3 radiance = light.color * attenuation*LIGHT_MULTIPLIER;
    
    // cook-torrance brdf
    float NDF = DistributionGGX(normal, H, roughness);        
    float G   = GeometrySmith(normal, view, light_dir, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, view), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, light_dir), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
        
    // add to outgoing radiance Lo
    float NdotL = max(dot(normal,light_dir), 0.0);                
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 calcSpotLight(SpotLight light, vec3 albedo, float metallic,float roughness,float ao, vec3 normal, vec3 view, vec3 position){
    vec3 light_dir = normalize(light.position - position);
    float theta = dot(light_dir, normalize(light.direction));
    vec3  color;
    if (theta > light.outerCone){
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);
        vec3 H=normalize(view+light_dir);
        float distance_light = length(light.position - position);
        float attenuation = 1.0 / (distance_light*distance_light);
        vec3 radiance = light.color* attenuation*LIGHT_MULTIPLIER;

        float epsilon = light.innerCone - light.outerCone;
        float intensity = clamp((theta - light.outerCone) / epsilon, 0.0f, 1.0f);
        
        // cook-torrance brdf
        float NDF = DistributionGGX(normal, H, roughness);        
        float G   = GeometrySmith(normal, view, light_dir, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, view), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, light_dir), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(normal,light_dir), 0.0);                
        color=intensity* (kD * albedo / PI + specular) * radiance * NdotL;
    } else {
        color = vec3(0.0);
    }
    return color;
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
    vec3 Albedo = texture(gColor, TexCoords).rgb;
    float metallic = texture(gMRA, TexCoords).r;
    float roughness = texture(gMRA, TexCoords).g;
    float ao = texture(gMRA, TexCoords).b;
    vec3 Emissive = texture(gEmissive, TexCoords).rgb;
    vec3 view = normalize(-FragPos);
    // f_color = vec4(color + u_material.emissive, 1.0f);
    vec3 color = vec3(0.0f);
    // color += calcDirLight(u_dir_lights[0],Diffuse, Specular, Normal, view, FragPos);
    for (int i=0;i<=2;i++){
        color+=calcPointLight(u_point_lights[i],Albedo, metallic,roughness,ao, Normal, view, FragPos);
    }
    for (int i=0;i<=13;i++){
        color+=calcSpotLight(u_spot_lights[i],Albedo, metallic,roughness,ao, Normal, view, FragPos);
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
}
