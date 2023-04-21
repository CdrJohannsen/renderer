
class Light {
    public:
        Light(int i,glm::vec3 d, glm::vec3 s, glm::vec3 a){
            diffuse=d;
            specular=s;
            ambient=a;
            index = to_string(i);
        }

        Light(int i, glm::vec3 d, float a) : Light(i,d,d,d*a){}

    protected:
        string index;
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 ambient;
};

class DirLight : public Light {
    public:
        DirLight(int i, Shader* shader,glm::vec3 dir, glm::vec3 d, float a) : DirLight(i,shader,dir,d,d,d*a){}
        DirLight(int i, Shader* shader,glm::vec3 dir, glm::vec3 d, glm::vec3 s, glm::vec3 a) : Light(i,d,s,a) {
            direction = glm::vec4(dir,0.0f);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), ((string)"u_dir_lights["+index+"].diffuse").c_str()), 1, (float*)&d);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), ((string)"u_dir_lights["+index+"].specular").c_str()), 1, (float*)&s);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), ((string)"u_dir_lights["+index+"].ambient").c_str()), 1, (float*)&a);
            directionLocation = glGetUniformLocation(shader->getShaderID(),((string)"u_dir_lights["+index+"].direction").c_str());
        }

        void update(glm::mat4 view){
            glm::vec4 transformedDirection = glm::transpose(glm::inverse(view)) * direction;
            glUniform3fv(directionLocation, 1, (float*)&transformedDirection);
        }

    private:
        glm::vec4 direction;
        int directionLocation;
};

class PointLight : public Light {
    public:
        PointLight(int i, Shader* shader,glm::vec4 pos, glm::vec3 d, float a, float l, float q) : PointLight(i,shader,pos,d,d,d*a,l,q){}
        PointLight(int i, Shader* shader,glm::vec3 pos, glm::vec3 d, glm::vec3 s, glm::vec3 a, float l, float q) : PointLight(i,shader,glm::vec4(pos,1.0f),d,s,a,l,q) {}
        PointLight(int i, Shader* shader,glm::vec4 pos, glm::vec3 d, glm::vec3 s, glm::vec3 a, float l, float q) : Light(i, d,s,a) {
            position = pos;
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), ((string)"u_point_lights["+index+"].diffuse").c_str()), 1, (float*)&d);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), ((string)"u_point_lights["+index+"].specular").c_str()), 1, (float*)&s);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), ((string)"u_point_lights["+index+"].ambient").c_str()), 1, (float*)&a);
            glUniform1f(glGetUniformLocation(shader->getShaderID(), ((string)"u_point_lights["+index+"].linear").c_str()), l);
            glUniform1f(glGetUniformLocation(shader->getShaderID(), ((string)"u_point_lights["+index+"].quadratic").c_str()), q);
            positionLocation = glGetUniformLocation(shader->getShaderID(),((string)"u_point_lights["+index+"].position").c_str());
        }

        void update(glm::mat4 view,glm::mat4 posMat){
            position = posMat * position;
            glm::vec3 transformedPosition = (glm::vec3)(view * position);
            glUniform3fv(positionLocation, 1, (float*)&transformedPosition);
        }

    private:
        glm::vec4 position;
        int positionLocation;
};

class SpotLight : public Light {
    public:
        SpotLight(int i, Shader* shader,glm::vec4 pos, glm::vec3 dir, glm::vec3 d, float a, float in, float o) : SpotLight(i, shader,pos,dir,d,d,d*a,in,o){}
        SpotLight(int i, Shader* shader,glm::vec3 pos, glm::vec3 dir, glm::vec3 d, glm::vec3 s, glm::vec3 a, float in, float o) : SpotLight(i, shader,glm::vec4(pos,1.0f),dir,d,s,a,in,o){}
        SpotLight(int i, Shader* shader,glm::vec4 pos, glm::vec3 dir, glm::vec3 d, glm::vec3 s, glm::vec3 a, float in, float o) : Light(i, d,s,a) {
            position = pos;
            direction = glm::vec4(dir,0.0f);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), ((string)"u_spot_lights["+index+"].diffuse").c_str()), 1, (float*)&d);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), ((string)"u_spot_lights["+index+"].specular").c_str()), 1, (float*)&s);
            glUniform3fv(glGetUniformLocation(shader->getShaderID(), ((string)"u_spot_lights["+index+"].ambient").c_str()), 1, (float*)&a);
            glUniform1f(glGetUniformLocation(shader->getShaderID(), ((string)"u_spot_lights["+index+"].innerCone").c_str()), in);
            glUniform1f(glGetUniformLocation(shader->getShaderID(), ((string)"u_spot_lights["+index+"].outerCone").c_str()), o);
            positionLocation = glGetUniformLocation(shader->getShaderID(),((string)"u_spot_lights["+index+"].position").c_str());
            directionLocation = glGetUniformLocation(shader->getShaderID(),((string)"u_spot_lights["+index+"].direction").c_str());
        }

        void update(glm::mat4 view,glm::mat4 posMat){
            position = posMat * position;
            glm::vec3 transformedPosition = (glm::vec3)(view * position);
            glUniform3fv(positionLocation, 1, (float*)&transformedPosition);

            glm::vec4 transformedDirection = glm::transpose(glm::inverse(view)) * direction;
            glUniform3fv(directionLocation, 1, (float*)&transformedDirection);
        }

    private:
        glm::vec4 position;
        int positionLocation;
        glm::vec4 direction;
        int directionLocation;
};
