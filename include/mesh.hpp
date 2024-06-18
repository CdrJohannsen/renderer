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
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    float shininess;
    bool hasDiffuse = false;
    bool hasNormal = false;
    bool hasSpecular = false;
};

struct Material {
    ModMaterial material;
    GLuint diffuseMap;
    GLuint normalMap;
    GLuint specularMap;
};

struct ModLight {
    int8_t type; // 1: Direction  2: Point  3: Spot
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
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
    int diffuseLocation;
    int specularLocation;
    int emissiveLocation;
    int shininessLocation;
    int diffuseMapLocation;
    int normalMapLocation;
    int specularMapLocation;
    int hasDiffuseLocation;
    int hasNormalLocation;
    int hasSpecularLocation;
};

class Model {
   public:
    void init(char* filename, Shader* shader, Shader* light_shader);

    void updateLights(glm::mat4 view, glm::mat4 pos);

    void render();

    ~Model();

   private:
    vector<Mesh*> meshes;
    vector<Material> materials;
    vector<DirLight> dir_lights;
    vector<PointLight> point_lights;
    vector<SpotLight> spot_lights;
};
