typedef struct GBuffer {
    GLuint fbo;
    GLuint rbo;
    GLuint tex;
} GBuffer;


bool gbuffer_create(GBuffer* buffer, size_t width, size_t height) {
    glGenFramebuffers(1, &buffer->fbo);
    texture_create_2d(&buffer->tex, NULL, 0, width, height, GL_R8, GL_RED, GL_UNSIGNED_BYTE);

    glGenRenderbuffers(1, &buffer->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, buffer->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    
    glBindFramebuffer(GL_FRAMEBUFFER, buffer->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->tex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer->rbo);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    check_framebuffer(false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

