int32_t texture_buffer_create(
    GLuint* fbo,
    void* data,
    size_t width,
    size_t height,
    size_t internal_format,
    size_t format,
    size_t type
) {
    GLuint rbo;
    GLuint tex;

    glGenFramebuffers(1, fbo);
    glGenRenderbuffers(1, &rbo);
    texture_create_2d(&tex, data, 0, width, height, internal_format, format, type);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    
    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    check_framebuffer(false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

