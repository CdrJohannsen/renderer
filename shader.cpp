#include "shader.h"
#include <fstream>
#include <iostream>
using namespace std;

Shader::Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename){
    shaderID = createShader(vertexShaderFilename,fragmentShaderFilename);
}
Shader::~Shader(){
    glDeleteProgram(shaderID);
}

void Shader::bind(){
    glUseProgram(shaderID);
}

void Shader::unbind(){
    glUseProgram(0);
}


GLuint Shader::compile(string shaderSource, GLenum type){
    GLuint id = glCreateShader(type);
    const char* src = shaderSource.c_str();
    glShaderSource(id, 1, &src, 0);
    glCompileShader(id);

    int result;
    glGetShaderiv(id,GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE){
        int length = 0;
        glGetShaderiv(id,GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetShaderInfoLog(id,length,&length,message);
        cout << "Shader compilation error: " << message << endl;
        delete[] message;
        return 0;
    }
    return id;
}

string Shader::parse(const char* filename){
    FILE* file;
    file = fopen(filename,"rb");
    if(file==nullptr) {
        cout << "File " << filename << " not found!" << endl;
    }
    string contents;
    fseek(file,0,SEEK_END);
    size_t filesize =ftell(file);
    rewind(file);
    contents.resize(filesize);

    fread(&contents[0], 1, filesize, file);
    fclose(file);
    return contents;
}

GLuint Shader::createShader(const char* vertexShaderFilename, const char* fragmentShaderFilename){
    string vertexShaderSource = parse(vertexShaderFilename);
    string fragmentShaderSource = parse(fragmentShaderFilename);

    GLuint program = glCreateProgram();
    GLuint vs = compile(vertexShaderSource,GL_VERTEX_SHADER);
    GLuint fs = compile(fragmentShaderSource,GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    #ifdef _RELEASE
    glDetachShader(program, vs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);
    #endif
    return program;
}
