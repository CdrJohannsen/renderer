#pragma once
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
using namespace std;

struct Shader {
    Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename);
    virtual ~Shader();

    void bind();
    void unbind();

    GLuint getShaderID(){
        return shaderID;
    };

    void setBool(const string &name, bool value) const;
    void setInt(const string &name, int value) const;
    void setFloat(const string &name, float value) const;
    void setVec2(const string &name, const glm::vec2 &value) const;
    void setVec2(const string &name, float x, float y) const;
    void setVec3(const string &name, const glm::vec3 &value) const;
    void setVec3(const string &name, float x, float y, float z) const;
    void setVec4(const string &name, const glm::vec4 &value) const;
    void setVec4(const string &name, float x, float y, float z, float w) const;
    void setMat2(const string &name, const glm::mat2 &mat) const;
    void setMat3(const string &name, const glm::mat3 &mat) const;
    void setMat4(const string &name, const glm::mat4 &mat) const;

    private:

    GLuint compile(std::string shaderSource, GLenum type);
    std::string parse(const char* filename);
    GLuint createShader(const char* vertexShaderFilename, const char* fragmentShaderFilename);

    GLuint shaderID;
};
