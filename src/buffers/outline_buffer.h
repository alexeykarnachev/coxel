typedef struct OutlineBuffer {
    size_t width;
    size_t height;
    GLuint fbo;
    Texture outline_texture;
} OutlineBuffer;


int outline_buffer_create(
    OutlineBuffer* buffer,
    size_t width,
    size_t height
) {
    glGenFramebuffers(1, &buffer->fbo);

    texture_create_2d(
        &buffer->outline_texture, NULL, 0,
        width, height,
        GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, buffer->fbo);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 0, GL_TEXTURE_2D,
        buffer->outline_texture.tex, 0
    );

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    check_framebuffer(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    buffer->width = width;
    buffer->height = height;

    return 1;
}

