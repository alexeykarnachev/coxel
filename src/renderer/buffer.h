bool buffer_create_depth_cube(GLuint* fbo, GLuint* tex, size_t width, size_t height);


#define _CHECK_FRAMEBUFFER \
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {\
                fprintf(stderr, "ERROR: framebuffer not complete\n");\
                return false;\
            }\

bool buffer_create_depth_cube(GLuint* fbo, GLuint* tex, size_t width, size_t height) {
    texture_create_depth_cube(tex, width, height);
    glGenFramebuffers(1, fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *tex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    _CHECK_FRAMEBUFFER

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

