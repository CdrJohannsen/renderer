#pragma once
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>
#include "floating_camera.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "imgui/imgui.h"

class Object{
    public:
        Object(char* filename, Shader* shader, Shader* light_shader, glm::vec3 p, glm::vec3 r = {0,0,0}, glm::vec3 s = {1,1,1}){
            model.init(filename,shader,light_shader);
            position = p;
            rotation = r;
            size = s;
            modelMat = glm::mat4(1.0f);
            // Scale here
            modelMat = glm::scale(modelMat,glm::vec3(1.0f));
            // Set uniforms
            modelViewLocation = glGetUniformLocation(shader->getShaderID(),"u_modelView");
            invModelViewLocation = glGetUniformLocation(shader->getShaderID(),"u_invModelView");
            modelViewProjLocation = glGetUniformLocation(shader->getShaderID(),"u_modelViewProj");
        }

        void render(FloatingCamera camera){
            modelViewProj = camera.getViewProj() * modelMat;
            modelView = camera.getView() * modelMat;
            invModelView = glm::transpose(glm::inverse(modelView));
            glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE,&modelView[0][0]);
            glUniformMatrix4fv(invModelViewLocation, 1, GL_FALSE,&invModelView[0][0]);
            glUniformMatrix4fv(modelViewProjLocation, 1, GL_FALSE,&modelViewProj[0][0]);
            model.render();
        }

        void updateLights(FloatingCamera camera){
            model.updateLights(camera.getView(), modelMat);
        }

        void move(float x, float y, float z){
            position += glm::vec3(x,y,z);
            modelMat = glm::translate(glm::mat4(1.0f), position);
        }

        void scale(float x, float y, float z){
            size += glm::vec3(x,y,z);
            modelMat = glm::scale(glm::mat4(1.0f), size);
        }

        void rotate(float angle, float x, float y, float z){
            rotation += glm::vec3(x,y,z);
            modelMat = glm::rotate(modelMat,angle, glm::vec3(x,y,z));
        }

        void renderDebugUI(){
            if(ImGui::TreeNode("Name")){
                ImGui::DragFloat("Pos X", &position.x ,0.5,-FLT_MAX,+FLT_MAX);
            }
        }

    private:
        Model model;
        glm::mat4 modelMat;
        glm::mat4 invModelView;
        glm::mat4 modelView;
        glm::mat4 modelViewProj;
        GLuint modelViewLocation ;
        GLuint invModelViewLocation;
        GLuint modelViewProjLocation;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 size;
};
