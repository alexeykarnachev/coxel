bool buffer_create_cube_shadowmap(GLuint* fbo, GLuint* tex, size_t size, size_t n_layers);


#define _CHECK_FRAMEBUFFER \
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {\
                fprintf(stderr, "ERROR: framebuffer not complete\n");\
                return false;\
            }\

bool buffer_create_cube_shadowmap(GLuint* fbo, GLuint* tex, size_t size, size_t n_layers) {
    glGenFramebuffers(1, fbo);
    glGenTextures(1, tex);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, *tex);
    glTexImage3D(
            GL_TEXTURE_CUBE_MAP_ARRAY,
            0,
            GL_DEPTH_COMPONENT,
            size,
            size,
            n_layers * 6,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            NULL
    );

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_GREATER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *tex, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    _CHECK_FRAMEBUFFER;

    return true;
}
