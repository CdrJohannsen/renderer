
class Light {
    public:
        Light(glm::vec3 d, glm::vec3 s, glm::vec3 a){
            diffuse=d;
            specular=s;
            ambient=a;
        }

        Light(glm::vec3 d, float a){
            diffuse=d;
            specular=d;
            ambient=d*a;
        }

    protected:
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 ambient;
};

class DirLight : public Light {
    public:
        DirLight(Shader* shader,glm::vec3 dir, glm::vec3 d, float a) : DirLight(shader,dir,d,d,d*a){}
        DirLight(Shader* shader,glm::vec3 dir, glm::vec3 d, glm::vec3 s, glm::vec3 a) : Light(d,s,a) {
            direction = glm::vec4(dir,0.0f);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), "u_dir_light.diffuse"), 1, (float*)&d);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), "u_dir_light.specular"), 1, (float*)&s);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), "u_dir_light.ambient"), 1, (float*)&a);
            directionLocation = glGetUniformLocation(shader->getShaderID(),"u_dir_light.direction");
        }

        void update(glm::mat4 view){
            glm::vec4 transformedDirection = glm::transpose(glm::inverse(view)) * direction;
            glUniform3fv(directionLocation, 1, (float*)&transformedDirection);
        }

    protected:
        glm::vec4 direction;
        int directionLocation;
};

class PointLight : public Light {
    public:
        PointLight(Shader* shader,glm::vec4 pos, glm::vec3 d, float a, float l, float q) : PointLight(shader,pos,d,d,d*a,l,q){}
        PointLight(Shader* shader,glm::vec4 pos, glm::vec3 d, glm::vec3 s, glm::vec3 a, float l, float q) : Light(d,s,a) {
            position = pos;
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), "u_point_light.diffuse"), 1, (float*)&d);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), "u_point_light.specular"), 1, (float*)&s);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), "u_point_light.ambient"), 1, (float*)&a);
            glUniform1f(glGetUniformLocation(shader->getShaderID(), "u_point_light.linear"), l);
            glUniform1f(glGetUniformLocation(shader->getShaderID(), "u_point_light.quadratic"), q);
            positionLocation = glGetUniformLocation(shader->getShaderID(),"u_point_light.position");
        }

        void update(glm::mat4 view,glm::mat4 posMat){
            position = posMat * position;
            glm::vec3 transformedPosition = (glm::vec3)(view * position);
            glUniform3fv(positionLocation, 1, (float*)&transformedPosition);
        }

    protected:
        glm::vec4 position;
        int positionLocation;
};

class SpotLight : public Light {
    public:
        SpotLight(Shader* shader,glm::vec4 pos, glm::vec3 dir, glm::vec3 d, float a, float i, float o) : SpotLight(shader,pos,dir,d,d,d*a,i,o){}
        SpotLight(Shader* shader,glm::vec4 pos, glm::vec3 dir, glm::vec3 d, glm::vec3 s, glm::vec3 a, float i, float o) : Light(d,s,a) {
            position = pos;
            direction = glm::vec4(dir,0.0f);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), "u_spot_light.diffuse"), 1, (float*)&d);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), "u_spot_light.specular"), 1, (float*)&s);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), "u_spot_light.ambient"), 1, (float*)&a);
            glUniform1f(glGetUniformLocation(shader->getShaderID(), "u_spot_light.innerCone"), i);
            glUniform1f(glGetUniformLocation(shader->getShaderID(), "u_spot_light.outerCone"), o);
            positionLocation = glGetUniformLocation(shader->getShaderID(),"u_spot_light.position");
            directionLocation = glGetUniformLocation(shader->getShaderID(),"u_spot_light.direction");
        }

        void update(glm::mat4 view,glm::mat4 posMat){
            position = posMat * position;
            glm::vec3 transformedPosition = (glm::vec3)(view * position);
            glUniform3fv(positionLocation, 1, (float*)&transformedPosition);

            glm::vec4 transformedDirection = glm::transpose(glm::inverse(view)) * direction;
            glUniform3fv(directionLocation, 1, (float*)&transformedDirection);
        }

    protected:
        glm::vec4 position;
        int positionLocation;
        glm::vec4 direction;
        int directionLocation;
};
