#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    public:

        Camera(float fov, float width, float heigth) {
            projection = glm::perspective(fov/2.0f,width/heigth,0.01f,1000.0f);
            view = glm::mat4(1.0f);
            update();
        }
        
        glm::mat4 getViewProj(){
            return viewProj;
        }

        void update(){
            viewProj = projection*view;
        }

        void translate(glm::vec3 v){
            view = glm::translate(view,v*-1.0f);
        }


    private:
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 viewProj;
};
