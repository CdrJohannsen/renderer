

struct FrameBuffer {

    void create(uint32_t width,uint32_t heigth){
        glGenFramebuffers(1,&fbo);

        glGenTextures(2, textures);

        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,heigth,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,width,heigth,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[0], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textures[1], 0);
        unbind();
    }

    GLuint getTextureID(int i){
        return textures[i];
    }

    void destroy(){
        glDeleteFramebuffers(1,&fbo);
    }

    void bind(){
        glBindFramebuffer(GL_FRAMEBUFFER,fbo);
    }

    void unbind(){
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }

    private:
    GLuint fbo;
    GLuint textures[2];
};
