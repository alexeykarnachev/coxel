typedef struct DeferredBuffer {
    GLuint fbo;
    GLuint tex;
} DeferredBuffer;


bool deferred_buffer_create(DeferredBuffer* buffer, size_t width, size_t height) {
    glGenFramebuffers(1, &buffer->fbo);
    texture_create_2d(&buffer->tex, NULL, 0, width, height, GL_R8, GL_RED, GL_UNSIGNED_BYTE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, buffer->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->tex, 0);

    GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    glReadBuffer(GL_NONE);

    check_framebuffer(false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

