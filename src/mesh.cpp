#include "mesh.hpp"

#include <stb/stb_image.h>

#include <fstream>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

Mesh::Mesh(vector<Vert> &vertices, uint64_t numVertices, vector<uint32_t> &indices, uint64_t numIndices,
           Material material, Shader *shader) {
    this->material = material;
    this->shader = shader;
    this->numIndices = numIndices;

    vertexBuffer = new VertexBuffer(vertices.data(), vertices.size());
    indexBuffer = new IndexBuffer(indices.data(), indices.size(), sizeof(indices[0]));

    diffuseLocation = glGetUniformLocation(shader->getShaderID(), "u_material.diffuse");
    specularLocation = glGetUniformLocation(shader->getShaderID(), "u_material.specular");
    emissiveLocation = glGetUniformLocation(shader->getShaderID(), "u_material.emissive");
    shininessLocation = glGetUniformLocation(shader->getShaderID(), "u_material.shininess");
    diffuseMapLocation = glGetUniformLocation(shader->getShaderID(), "u_diffuse_map");
    normalMapLocation = glGetUniformLocation(shader->getShaderID(), "u_normal_map");
    specularMapLocation = glGetUniformLocation(shader->getShaderID(), "u_specular_map");
    hasDiffuseLocation = glGetUniformLocation(shader->getShaderID(), "u_material.hasDiffuse");
    hasNormalLocation = glGetUniformLocation(shader->getShaderID(), "u_material.hasNormal");
    hasSpecularLocation = glGetUniformLocation(shader->getShaderID(), "u_material.hasSpecular");
}

Mesh::~Mesh() {
    delete vertexBuffer;
    delete indexBuffer;
}

inline void Mesh::render() {
    vertexBuffer->bind();
    indexBuffer->bind();
    glUniform3fv(diffuseLocation, 1, (float *)&material.material.diffuse);
    glUniform3fv(specularLocation, 1, (float *)&material.material.specular);
    glUniform3fv(emissiveLocation, 1, (float *)&material.material.emissive);
    glUniform1f(shininessLocation, material.material.shininess);

    glUniform1i(hasDiffuseLocation, material.material.hasDiffuse);
    glUniform1i(hasNormalLocation, material.material.hasNormal);
    glUniform1i(hasSpecularLocation, material.material.hasSpecular);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.diffuseMap);
    glUniform1i(diffuseMapLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material.normalMap);
    glUniform1i(normalMapLocation, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, material.specularMap);
    glUniform1i(specularMapLocation, 2);

    glActiveTexture(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

void Model::init(char *filename, Shader *shader, Shader *light_shader) {
    uint64_t numMeshes = 0;
    uint64_t numMaterials = 0;
    uint64_t numLights = 0;

    ifstream input = ifstream(filename, ios::in | ios::binary);
    if (!input.is_open()) {
        cout << "Could not read model" << endl;
        return;
    }

    // Lights
    glm::vec3 inv(-1.0f);
    input.read((char *)&numLights, sizeof(uint64_t));
    int numDir = 0;
    int numPoint = 0;
    int numSpot = 0;
    light_shader->bind();
    for (uint64_t i = 0; i < numLights; i++) {
        ModLight light = {};
        input.read((char *)&light, sizeof(ModLight));
        if (light.type == 1) {
            glm::vec3 scale(4.0f);
            light.diffuse /= scale;
            light.specular /= scale;
            light.ambient /= scale;
            light.ambient = light.diffuse * 0.4f;
        } else {
            light.direction *= inv;
            glm::vec3 scale(5000.0f);
            light.diffuse /= scale;
            light.specular /= scale;
            light.ambient /= scale;
        }

        if (light.type == 0) {
            cout << "Error when loading Model: undefined light source";
        } else if (light.type == 1) {
            DirLight l(numDir, light_shader, light.direction, light.diffuse, light.specular, light.ambient);
            numDir++;
            dir_lights.push_back(l);
        } else if (light.type == 2) {
            PointLight l(numPoint, light_shader, light.position, light.diffuse, light.specular, light.ambient,
                         light.linear, light.quadratic);
            numPoint++;
            point_lights.push_back(l);
        } else if (light.type == 3) {
            SpotLight l(numSpot, light_shader, light.position, light.direction, light.diffuse, light.specular,
                        light.ambient, cos(light.innerCone / 2), cos(light.outerCone / 2));
            numSpot++;
            spot_lights.push_back(l);
        }
    }
    // Add "sun" if there are no lights
    if (numLights == 0) {
        DirLight light(0, light_shader, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), 0.4f);
        dir_lights.push_back(light);
    }
    shader->bind();

    // Materials
    input.read((char *)&numMaterials, sizeof(uint64_t));
    for (uint64_t i = 0; i < numMaterials; i++) {
        Material material = {};
        input.read((char *)&material, sizeof(ModMaterial));

        uint64_t diffuseMapNameLength = 0;
        input.read((char *)&diffuseMapNameLength, sizeof(uint64_t));
        std::string diffuseMapName(diffuseMapNameLength, '\0');
        input.read((char *)&diffuseMapName[0], diffuseMapNameLength);

        uint64_t normalMapNameLength = 0;
        input.read((char *)&normalMapNameLength, sizeof(uint64_t));
        std::string normalMapName(normalMapNameLength, '\0');
        input.read((char *)&normalMapName[0], normalMapNameLength);

        uint64_t specularMapNameLength = 0;
        input.read((char *)&specularMapNameLength, sizeof(uint64_t));
        std::string specularMapName(specularMapNameLength, '\0');
        input.read((char *)&specularMapName[0], specularMapNameLength);

        int32_t textureWidth = 0;
        int32_t textureHeigth = 0;
        int32_t bitsPerPixel = 0;
        glGenTextures(3, &material.diffuseMap);
        stbi_set_flip_vertically_on_load(true);
        if (material.material.hasDiffuse) {
            auto textureBuffer = stbi_load(diffuseMapName.c_str(), &textureWidth, &textureHeigth, &bitsPerPixel, 4);
            // assert(textureBuffer);
            // assert(material.diffuseMap);

            if (textureBuffer) {
                glBindTexture(GL_TEXTURE_2D, material.diffuseMap);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeigth, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                             textureBuffer);

                stbi_image_free(textureBuffer);
            }
        }

        if (material.material.hasNormal) {
            auto textureBuffer = stbi_load(normalMapName.c_str(), &textureWidth, &textureHeigth, &bitsPerPixel, 4);
            // assert(textureBuffer);
            // assert(material.normalMap);

            if (textureBuffer) {
                glBindTexture(GL_TEXTURE_2D, material.normalMap);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeigth, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                             textureBuffer);

                stbi_image_free(textureBuffer);
            }
        }

        if (material.material.hasSpecular) {
            auto textureBuffer = stbi_load(specularMapName.c_str(), &textureWidth, &textureHeigth, &bitsPerPixel, 4);
            // assert(textureBuffer);
            // assert(material.specularMap);

            if (textureBuffer) {
                glBindTexture(GL_TEXTURE_2D, material.specularMap);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeigth, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                             textureBuffer);

                stbi_image_free(textureBuffer);
            }
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        materials.push_back(material);
    }

    // Meshes
    input.read((char *)&numMeshes, sizeof(uint64_t));

    for (uint64_t i = 0; i < numMeshes; i++) {
        Material material;
        vector<Vert> vertices;
        uint64_t numVerts = 0;

        vector<uint32_t> indices;
        uint64_t numIndices = 0;

        uint64_t materialIndex = 0;

        input.read((char *)&materialIndex, sizeof(uint64_t));
        input.read((char *)&numVerts, sizeof(uint64_t));
        input.read((char *)&numIndices, sizeof(uint64_t));

        for (uint64_t i = 0; i < numVerts; i++) {
            Vert vertex;
            input.read((char *)&vertex.position.x, sizeof(float));
            input.read((char *)&vertex.position.y, sizeof(float));
            input.read((char *)&vertex.position.z, sizeof(float));
            input.read((char *)&vertex.normal.x, sizeof(float));
            input.read((char *)&vertex.normal.y, sizeof(float));
            input.read((char *)&vertex.normal.z, sizeof(float));
            input.read((char *)&vertex.tangent.x, sizeof(float));
            input.read((char *)&vertex.tangent.y, sizeof(float));
            input.read((char *)&vertex.tangent.z, sizeof(float));
            input.read((char *)&vertex.texCoord.x, sizeof(float));
            input.read((char *)&vertex.texCoord.y, sizeof(float));
            vertices.push_back(vertex);
        }
        for (uint64_t i = 0; i < numIndices; i++) {
            uint32_t index;
            input.read((char *)&index, sizeof(uint32_t));
            indices.push_back(index);
        }
        Mesh *mesh = new Mesh(vertices, numVerts, indices, numIndices, materials[materialIndex], shader);
        meshes.push_back(mesh);
    }
}

void Model::updateLights(glm::mat4 view, glm::mat4 pos) {
    for (DirLight light : dir_lights) {
        light.update(view, pos);
    }
    for (PointLight light : point_lights) {
        light.update(view, pos);
    }
    for (SpotLight light : spot_lights) {
        light.update(view, pos);
    }
}
void Model::render() {
    for (Mesh *mesh : meshes) {
        mesh->render();
    }
}

Model::~Model() {
    for (Mesh *mesh : meshes) {
        delete mesh;
    }
}
