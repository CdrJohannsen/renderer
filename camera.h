#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    public:

        Camera(float fov, float width, float heigth) {
            projection = glm::perspective(fov/2.0f,width/heigth,0.2f,1000.0f);
            view = glm::mat4(1.0f);
            position = glm::vec3(0.0f,0.0f,5.0f);
            update();
        }
        
        glm::mat4 getViewProj(){
            return viewProj;
        }

        glm::mat4 getView(){
            return view;
        }

        virtual void update(){
            viewProj = projection*view;
        }

        virtual void translate(glm::vec3 v){
            position += v;
            view = glm::translate(view,v*-1.0f);
        }
        string getPositionString(){
            string a = "x: ";
            a.append(to_string(position.x));
            a.append(" y: ");
            a.append(to_string(position.y));
            a.append(" z: ");
            a.append(to_string(position.z));
            return a;
        }

    protected:
        glm::vec3 position;
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 viewProj;
};
