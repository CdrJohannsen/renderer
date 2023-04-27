#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <cassert>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

struct Position{
    float x,y,z;
};

struct Position2D{
    float x, y;
};

struct Material {
    Position diffuse;
    Position specular;
    Position emissive;
    float shininess;
    bool hasDiffuse = false;
    bool hasNormal = false;
    bool hasSpecular = false;
    aiString diffuseMapName;
    aiString normalMapName;
    aiString specularMapName;
};

struct ModMaterial {
    Position diffuse;
    Position specular;
    Position emissive;
    float shininess;
    bool hasDiffuse = false;
    bool hasNormal = false;
    bool hasSpecular = false;
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
    int8_t type; // 1: Direction  2: Point  3: Spot
    Position position;
    Position direction;
    Position diffuse;
    Position specular;
    Position ambient;
    float innerCone;
    float outerCone;
    float linear;
    float quadratic;
};

vector<Mesh> meshes;
vector<Material> materials;
vector<Light> lights;

void processMesh(aiMesh* mesh,const aiScene* scene){
    Mesh m;
    for (unsigned int i = 0; i < mesh->mNumVertices;i++){
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
        if (*mesh->mNumUVComponents > 0){
            texCoord.x = mesh->mTextureCoords[0][i].x;
            texCoord.y = mesh->mTextureCoords[0][i].y;

            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;

        }
        m.texCoord.push_back(texCoord);
        m.tangents.push_back(tangent);

    }
    for (unsigned int i = 0; i < mesh->mNumFaces;i++){
        aiFace face = mesh->mFaces[i];
        assert(face.mNumIndices == 3);
        for(unsigned int j=0; j < face.mNumIndices;j++){
            m.indices.push_back(face.mIndices[j]);
        }
    }
    m.materialIndex = mesh->mMaterialIndex;
    meshes.push_back(m);
}

void processNode(aiNode* node, const aiScene* scene){

    for(unsigned int i = 0; i < node->mNumMeshes;i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh,scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren;i++){
        processNode(node->mChildren[i],scene);
    }
}

char* getFilename(char* filename){
    int length = strlen(filename);
    char* lastSlash = filename;
    for (int i = 0;i < length;i++){
        if(filename[i] == '/' || filename[i] == '\\'){
            lastSlash = filename+i+1;
        }
    }
    return lastSlash;
}

void processMaterials(const aiScene* scene){
    for(uint32_t i = 0; i < scene->mNumMaterials; i++){
        Material mat = {};
        aiMaterial* material = scene->mMaterials[i];

        aiColor3D diffuse(0.0f,0.0f,0.0f);
        if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE,diffuse)){
            cout << "Diffuse color could not be loaded!" << endl;
        }
        mat.diffuse =  {diffuse.r,diffuse.g,diffuse.b};
        aiColor3D specular(0.0f,0.0f,0.0f);
        if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_SPECULAR, specular)){
            cout << "Specular color could not be loaded!" << endl;
        }
        mat.specular =  {specular.r,specular.g,specular.b};
        aiColor3D emissive(0.0f,0.0f,0.0f);
        if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive)){
            cout << "Emissive color could not be loaded!" << endl;
        }
        mat.emissive =  {emissive.r,emissive.g,emissive.b};

        float shininess = 0.0f;
        if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shininess)){
            cout << "Shininess color could not be loaded!" << endl;
        }
        mat.shininess =  shininess;
        float shininessStrength = 0.0f;
        if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS_STRENGTH,shininessStrength)){
            cout << "Shininess Strength color could not be loaded!" << endl;
        }
        mat.specular.x *= shininessStrength;
        mat.specular.y *= shininessStrength;
        mat.specular.z *= shininessStrength;

        uint32_t numDiffuseMaps = material->GetTextureCount(aiTextureType_DIFFUSE);
        uint32_t numNormalMaps = material->GetTextureCount(aiTextureType_NORMALS);
        uint32_t numSpecularMaps = material->GetTextureCount(aiTextureType_SPECULAR);
        if (numDiffuseMaps > 0) {
            material->GetTexture(aiTextureType_DIFFUSE, 0, &mat.diffuseMapName);
            mat.hasDiffuse = true;
        }
        if (numNormalMaps > 0) {
            material->GetTexture(aiTextureType_NORMALS, 0, &mat.normalMapName);
            mat.hasNormal = true;
        }
        if (numSpecularMaps > 0) {
            material->GetTexture(aiTextureType_SPECULAR, 0, &mat.specularMapName);
            mat.hasSpecular = true;
        }

        materials.push_back(mat);
    }
}

void processLights(const aiScene* scene){
    for(uint32_t i = 0; i < scene->mNumLights; i++){
        Light lig = {};
        aiLight* light = scene->mLights[i];

        lig.type = light->mType;
        lig.position =  {light->mPosition.x,        light->mPosition.y,         light->mPosition.z};
        lig.direction = {light->mDirection.x,       light->mDirection.y,        light->mDirection.z};
        lig.diffuse =   {light->mColorDiffuse.r,    light->mColorDiffuse.g,     light->mColorDiffuse.b};
        lig.specular =  {light->mColorSpecular.r,   light->mColorSpecular.g,    light->mColorSpecular.b};
        lig.ambient =   {light->mColorAmbient.r,    light->mColorAmbient.g,     light->mColorAmbient.b};
        lig.innerCone = light->mAngleInnerCone;
        lig.outerCone = light->mAngleOuterCone;
        lig.linear = light->mAttenuationLinear;
        lig.quadratic = light->mAttenuationQuadratic;

        lights.push_back(lig);
    }
}

int main (int argc, char** argv) {
    if(argc <= 0){
        return 1;
    }
    else if(argc < 2){
        cout << "Usage: " << argv[0] << " [FILE] " << endl;
        return 1;
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(argv[argc-1], aiProcess_PreTransformVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_CalcTangentSpace | aiProcess_FindInstances);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE, !scene->mRootNode){
        cout << "Error while loading model with assimp: " << importer.GetErrorString() << endl;
        return 1;
    }
    if (scene->HasLights()){
        processLights(scene);
    }
    if (scene->HasMaterials()){
        processMaterials(scene);
    }
    if (scene->HasMeshes()){
        processNode(scene->mRootNode, scene);
    }

    string filename = string(getFilename(argv[argc-1]));
    string filenameNoExt = filename.substr(0,filename.find_last_of('.'));
    string outputFilename = "../ressources/" + filenameNoExt + ".mod";

    ofstream output(outputFilename,ios::out | ios::binary);
    cout << "Writing mod file..." << endl;

    // Lights
    uint64_t numLights = lights.size();
    output.write((char*)&numLights,sizeof(uint64_t));
    for (Light light: lights) {
        output.write((char*)&light,sizeof(Light));
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
    output.write((char*)&numMaterials,sizeof(uint64_t));
    for (Material material: materials) {
        output.write((char*)&material, sizeof(ModMaterial));
        const char* pathPrefix = "ressources/";
        // Diffuse map
        uint64_t diffuseMapNameLength = material.diffuseMapName.length + 11;
        output.write((char*)&diffuseMapNameLength, sizeof(uint64_t));
        output.write(pathPrefix, 11);
        output.write((char*)&material.diffuseMapName.data, material.diffuseMapName.length);
        // Normal map
        uint64_t normalMapNameLength = material.normalMapName.length + 11;
        output.write((char*)&normalMapNameLength, sizeof(uint64_t));
        output.write(pathPrefix, 11);
        output.write((char*)&material.normalMapName.data, material.normalMapName.length);
        // Specular map
        uint64_t specularMapNameLength = material.specularMapName.length + 11;
        output.write((char*)&specularMapNameLength, sizeof(uint64_t));
        output.write(pathPrefix, 11);
        output.write((char*)&material.specularMapName.data, material.specularMapName.length);
    }

    // Meshes
    uint64_t numMeshes = meshes.size();
    output.write((char*)&numMeshes,sizeof(uint64_t));
    for (Mesh& mesh : meshes) {
        uint64_t numVertices = mesh.positions.size();
        uint64_t numIndices = mesh.indices.size();
        uint64_t materialIndex = mesh.materialIndex;
        //output.write((char*)&mesh.material,sizeof(Material));

        output.write((char*)&materialIndex,sizeof(uint64_t));
        output.write((char*)&numVertices,sizeof(uint64_t));
        output.write((char*)&numIndices,sizeof(uint64_t));
        for (int i = 0; i < numVertices; i++){
            output.write((char*)&mesh.positions[i].x,sizeof(float));
            output.write((char*)&mesh.positions[i].y,sizeof(float));
            output.write((char*)&mesh.positions[i].z,sizeof(float));

            output.write((char*)&mesh.normals[i].x,sizeof(float));
            output.write((char*)&mesh.normals[i].y,sizeof(float));
            output.write((char*)&mesh.normals[i].z,sizeof(float));

            output.write((char*)&mesh.tangents[i].x,sizeof(float));
            output.write((char*)&mesh.tangents[i].y,sizeof(float));
            output.write((char*)&mesh.tangents[i].z,sizeof(float));

            output.write((char*)&mesh.texCoord[i].x,sizeof(float));
            output.write((char*)&mesh.texCoord[i].y,sizeof(float));
        }
        for (int i = 0; i < numIndices; i++){
            output.write((char*)&mesh.indices[i],sizeof(uint32_t));
        }
    }
    output.close();

}
