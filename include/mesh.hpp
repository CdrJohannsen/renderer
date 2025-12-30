#pragma once
#include <vector>

#include "index_buffer.hpp"
#include "light.hpp"
#include "shader.hpp"
#include "vert.hpp"
#include "vertex_buffer.hpp"

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb/stb_image.h>
// #include "vertex_buffer.h"
// #include "index_buffer.h"

struct ModMaterial {
    glm::vec3 albedo;
    glm::vec3 normal;
    glm::vec3 emissive;
    float metallic;
    float roughness;
    bool hasAlbedo = false;
    bool hasNormal = false;
    bool hasMetallic = false;
    bool hasRoughness = false;
    bool hasAo = false;
};

struct Material {
    ModMaterial material;
    GLuint albedoMap;
    GLuint normalMap;
    GLuint metallicMap;
    GLuint roughnessMap;
    GLuint aoMap;
};

struct ModLight {
    int8_t type;  // 1: Direction  2: Point  3: Spot
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float innerCone;
    float outerCone;
    float linear;
    float quadratic;
};

class Mesh {
   public:
    Mesh(vector<Vert>& vertices, uint64_t numVertices, vector<uint32_t>& indices, uint64_t numIndices,
         Material material, Shader* shader);

    ~Mesh();

    inline void render();

   private:
    VertexBuffer* vertexBuffer;
    IndexBuffer* indexBuffer;
    Shader* shader;
    Material material;
    uint64_t numIndices = 0;
    int albedoLocation;
    int emissiveLocation;
    int metallicLocation;
    int roughnessLocation;
    int albedoMapLocation;
    int normalMapLocation;
    int metallicMapLocation;
    int roughnessMapLocation;
    int aoMapLocation;
    int hasAlbedoLocation;
    int hasNormalLocation;
    int hasMetallicLocation;
    int hasRoughnessLocation;
    int hasAoLocation;
};

class Model {
   public:
    void init(char* filename, Shader* shader, Shader* light_shader);

    void updateLights(glm::mat4 view, glm::mat4 pos);

    void render();

#ifdef _DEBUG
    void renderDebugUI();
#endif

    ~Model();

   private:
    vector<Mesh*> meshes;
    vector<Material> materials;
    vector<DirLight> dir_lights;
    vector<PointLight> point_lights;
    vector<SpotLight> spot_lights;
};
