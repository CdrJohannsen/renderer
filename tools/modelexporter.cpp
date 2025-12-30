#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Position {
    float x, y, z;
};

using Color = Position;

struct Position2D {
    float x, y;
};

struct Material {
    Color albedo;
    Color normal;
    Color emissive;
    float metallic;
    float roughness;
    bool hasAlbedo = false;
    bool hasNormal = false;
    bool hasMetallic = false;
    bool hasRoughness = false;
    bool hasAo = false;
    aiString albedoMapName;
    aiString normalMapName;
    aiString metallicMapName;
    aiString roughnessMapName;
    aiString aoMapName;
};

struct ModMaterial {
    Color albedo;
    Color normal;
    Color emissive;
    float metallic;
    float roughness;
    bool hasAlbedo = false;
    bool hasNormal = false;
    bool hasMetallic = false;
    bool hasRoughness = false;
    bool hasAo = false;
};

struct Mesh {
    vector<Position> positions;
    vector<Position> normals;
    vector<Position> tangents;
    vector<Position2D> texCoord;
    vector<uint32_t> indices;
    int materialIndex;
};

struct Light {
    int8_t type;  // 1: Direction  2: Point  3: Spot
    Position position;
    Position direction;
    Position color;
    float innerCone;
    float outerCone;
    float linear;
    float quadratic;
};

vector<Mesh> meshes;
vector<Material> materials;
vector<Light> lights;

void processMesh(aiMesh* mesh, const aiScene* scene) {
    Mesh m;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Position position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        m.positions.push_back(position);

        Position normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        m.normals.push_back(normal);

        Position2D texCoord;
        Position tangent;
        if (*mesh->mNumUVComponents > 0) {
            texCoord.x = mesh->mTextureCoords[0][i].x;
            texCoord.y = mesh->mTextureCoords[0][i].y;

            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;
        }
        m.texCoord.push_back(texCoord);
        m.tangents.push_back(tangent);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        assert(face.mNumIndices == 3);
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            m.indices.push_back(face.mIndices[j]);
        }
    }
    m.materialIndex = mesh->mMaterialIndex;
    meshes.push_back(m);
}

void processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

char* getFilename(char* filename) {
    int length = strlen(filename);
    char* lastSlash = filename;
    for (int i = 0; i < length; i++) {
        if (filename[i] == '/' || filename[i] == '\\') {
            lastSlash = filename + i + 1;
        }
    }
    return lastSlash;
}

void processMaterials(const aiScene* scene) {
    for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
        Material mat = {};
        aiMaterial* material = scene->mMaterials[i];

        aiColor3D albedo(0.0f, 0.0f, 0.0f);
        if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, albedo)) {
            cout << "Albedo color could not be loaded!" << endl;
        }
        mat.albedo = {albedo.r, albedo.g, albedo.b};

        float metallic = 0;
        if (AI_SUCCESS != material->Get(AI_MATKEY_REFLECTIVITY, metallic)) {
            cout << "Metallic color could not be loaded!" << endl;
        }
        mat.metallic = metallic;

        float roughness = 0.0f;
        if (AI_SUCCESS != material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness)) {
            cout << "Roughness color could not be loaded!" << endl;
        }
        mat.roughness = roughness;

        aiColor3D emissive(0.0f, 0.0f, 0.0f);
        if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive)) {
            cout << "Emissive color could not be loaded!" << endl;
        }
        mat.emissive = {emissive.r, emissive.g, emissive.b};

        uint32_t numDiffuseMaps = material->GetTextureCount(aiTextureType_DIFFUSE);
        uint32_t numNormalMaps = material->GetTextureCount(aiTextureType_NORMALS);
        uint32_t numMetalMaps = material->GetTextureCount(aiTextureType_METALNESS);
        uint32_t numRoughnessMaps = material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
        uint32_t numAoMaps = material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
        if (numDiffuseMaps > 0) {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &mat.albedoMapName);
            mat.hasAlbedo = true;
        }
        if (numNormalMaps > 0) {
            material->GetTexture(aiTextureType_NORMALS, 0, &mat.normalMapName);
            mat.hasNormal = true;
        }
        if (numMetalMaps > 0) {
            material->GetTexture(aiTextureType_METALNESS, 0, &mat.metallicMapName);
            mat.hasMetallic = true;
        }
        if (numRoughnessMaps > 0) {
            material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &mat.roughnessMapName);
            mat.hasRoughness = true;
        }
        if (numAoMaps > 0) {
            material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &mat.aoMapName);
            mat.hasAo = true;
        }

        materials.push_back(mat);
    }
}

void processLights(const aiScene* scene) {
    for (uint32_t i = 0; i < scene->mNumLights; i++) {
        Light lig = {};
        aiLight* light = scene->mLights[i];

        lig.type = light->mType;
        lig.position = {light->mPosition.x, light->mPosition.y, light->mPosition.z};
        lig.direction = {light->mDirection.x, light->mDirection.y, light->mDirection.z};
        lig.color = {light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b};
        lig.innerCone = light->mAngleInnerCone;
        lig.outerCone = light->mAngleOuterCone;
        lig.linear = light->mAttenuationLinear;
        lig.quadratic = light->mAttenuationQuadratic;

        lights.push_back(lig);
    }
}

int main(int argc, char** argv) {
    if (argc <= 0) {
        return 1;
    } else if (argc < 2) {
        cout << "Usage: " << argv[0] << " [FILE] " << endl;
        return 1;
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        argv[argc - 1], aiProcess_PreTransformVertices | aiProcess_Triangulate | aiProcess_GenNormals |
                            aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices |
                            aiProcess_ImproveCacheLocality | aiProcess_CalcTangentSpace | aiProcess_FindInstances);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE, !scene->mRootNode) {
        cout << "Error while loading model with assimp: " << importer.GetErrorString() << endl;
        return 1;
    }
    if (scene->HasLights()) {
        processLights(scene);
    }
    if (scene->HasMaterials()) {
        processMaterials(scene);
    }
    if (scene->HasMeshes()) {
        processNode(scene->mRootNode, scene);
    }

    string filename = string(getFilename(argv[argc - 1]));
    string filenameNoExt = filename.substr(0, filename.find_last_of('.'));
    string outputFilename = "../ressources/" + filenameNoExt + ".mod";

    ofstream output(outputFilename, ios::out | ios::binary);
    cout << "Writing mod file..." << endl;

    // Lights
    uint64_t numLights = lights.size();
    output.write((char*)&numLights, sizeof(uint64_t));
    for (Light light : lights) {
        output.write((char*)&light, sizeof(Light));
        /*
        output.write((char*)&light.type,sizeof(uint8_t));

        output.write((char*)&light.position.x,sizeof(float));
        output.write((char*)&light.position.y,sizeof(float));
        output.write((char*)&light.position.z,sizeof(float));

        output.write((char*)&light.direction.x,sizeof(float));
        output.write((char*)&light.direction.y,sizeof(float));
        output.write((char*)&light.direction.z,sizeof(float));

        output.write((char*)&light.diffuse.x,sizeof(float));
        output.write((char*)&light.diffuse.y,sizeof(float));
        output.write((char*)&light.diffuse.z,sizeof(float));

        output.write((char*)&light.specular.x,sizeof(float));
        output.write((char*)&light.specular.y,sizeof(float));
        output.write((char*)&light.specular.z,sizeof(float));

        output.write((char*)&light.ambient.x,sizeof(float));
        output.write((char*)&light.ambient.y,sizeof(float));
        output.write((char*)&light.ambient.z,sizeof(float));

        output.write((char*)&light.linear,sizeof(float));
        output.write((char*)&light.quadratic,sizeof(float));

        output.write((char*)&light.innerCone,sizeof(float));
        output.write((char*)&light.outerCone,sizeof(float));
        */
    }

    // Materials
    uint64_t numMaterials = materials.size();
    output.write((char*)&numMaterials, sizeof(uint64_t));
    for (Material material : materials) {
        output.write((char*)&material, sizeof(ModMaterial));
        const char* pathPrefix = "ressources/";
        // albedo map
        uint64_t albedoMapNameLength = material.albedoMapName.length + 11;
        output.write((char*)&albedoMapNameLength, sizeof(uint64_t));
        output.write(pathPrefix, 11);
        output.write((char*)&material.albedoMapName.data, material.albedoMapName.length);
        // Normal map
        uint64_t normalMapNameLength = material.normalMapName.length + 11;
        output.write((char*)&normalMapNameLength, sizeof(uint64_t));
        output.write(pathPrefix, 11);
        output.write((char*)&material.normalMapName.data, material.normalMapName.length);
        // metallic map
        uint64_t metallicMapNameLength = material.metallicMapName.length + 11;
        output.write((char*)&metallicMapNameLength, sizeof(uint64_t));
        output.write(pathPrefix, 11);
        output.write((char*)&material.metallicMapName.data, material.metallicMapName.length);
        // roughness map
        uint64_t roughnessMapNameLength = material.roughnessMapName.length + 11;
        output.write((char*)&roughnessMapNameLength, sizeof(uint64_t));
        output.write(pathPrefix, 11);
        output.write((char*)&material.roughnessMapName.data, material.roughnessMapName.length);
        // ao map
        uint64_t aoMapNameLength = material.aoMapName.length + 11;
        output.write((char*)&aoMapNameLength, sizeof(uint64_t));
        output.write(pathPrefix, 11);
        output.write((char*)&material.aoMapName.data, material.aoMapName.length);
    }

    // Meshes
    uint64_t numMeshes = meshes.size();
    output.write((char*)&numMeshes, sizeof(uint64_t));
    for (Mesh& mesh : meshes) {
        uint64_t numVertices = mesh.positions.size();
        uint64_t numIndices = mesh.indices.size();
        uint64_t materialIndex = mesh.materialIndex;
        // output.write((char*)&mesh.material,sizeof(Material));

        output.write((char*)&materialIndex, sizeof(uint64_t));
        output.write((char*)&numVertices, sizeof(uint64_t));
        output.write((char*)&numIndices, sizeof(uint64_t));
        for (int i = 0; i < numVertices; i++) {
            output.write((char*)&mesh.positions[i].x, sizeof(float));
            output.write((char*)&mesh.positions[i].y, sizeof(float));
            output.write((char*)&mesh.positions[i].z, sizeof(float));

            output.write((char*)&mesh.normals[i].x, sizeof(float));
            output.write((char*)&mesh.normals[i].y, sizeof(float));
            output.write((char*)&mesh.normals[i].z, sizeof(float));

            output.write((char*)&mesh.tangents[i].x, sizeof(float));
            output.write((char*)&mesh.tangents[i].y, sizeof(float));
            output.write((char*)&mesh.tangents[i].z, sizeof(float));

            output.write((char*)&mesh.texCoord[i].x, sizeof(float));
            output.write((char*)&mesh.texCoord[i].y, sizeof(float));
        }
        for (int i = 0; i < numIndices; i++) {
            output.write((char*)&mesh.indices[i], sizeof(uint32_t));
        }
    }
    output.close();
}
