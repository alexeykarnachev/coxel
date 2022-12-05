typedef struct OverlayBuffer {
    size_t width;
    size_t height;
    GLuint fbo;
    Texture outline_texture;
    Texture gui_texture;
} OverlayBuffer;


int overlay_buffer_create(
    OverlayBuffer* buffer,
    size_t width,
    size_t height
) {
    glGenFramebuffers(1, &buffer->fbo);

    texture_create_2d(
        &buffer->outline_texture, NULL, 0,
        width, height,
        GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, GL_NEAREST);

    texture_create_2d(
        &buffer->gui_texture, NULL, 0,
        width, height,
        GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, buffer->fbo);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 0, GL_TEXTURE_2D,
        buffer->outline_texture.tex, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_2D,
        buffer->gui_texture.tex, 0
    );

    GLuint buffers[2] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1
    };
    glDrawBuffers(2, buffers);

    check_framebuffer(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    buffer->width = width;
    buffer->height = height;

    return 1;
}

