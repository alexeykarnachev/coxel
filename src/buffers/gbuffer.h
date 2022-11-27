typedef struct GBuffer {
    size_t width;
    size_t height;
    GLuint fbo;
    GLuint rbo;
    Texture world_pos_texture;
    Texture world_norm_texture;
    Texture diffuse_texture;
    Texture specular_texture;
    Texture entity_id_texture;
} GBuffer;


int gbuffer_create(GBuffer* buffer, size_t width, size_t height) {
    glGenFramebuffers(1, &buffer->fbo);
    glGenRenderbuffers(1, &buffer->rbo);

    // TODO: Factor these textures out in a loop or so
    texture_create_2d(
        &buffer->world_pos_texture, NULL, 0,
        width, height,
        GL_RGB32F, GL_RGB, GL_FLOAT, GL_NEAREST);

    texture_create_2d(
        &buffer->world_norm_texture, NULL, 0,
        width, height,
        GL_RGB32F, GL_RGB, GL_FLOAT, GL_NEAREST);

    texture_create_2d(
        &buffer->diffuse_texture, NULL, 0,
        width, height,
        GL_RGB32F, GL_RGB, GL_FLOAT, GL_NEAREST);

    texture_create_2d(
        &buffer->specular_texture, NULL, 0,
        width, height,
        GL_RGB32F, GL_RGB, GL_FLOAT, GL_NEAREST);

    // TODO: 1 byte for element is not enough for sure!
    // There could be more than 256 entities.
    texture_create_2d(
        &buffer->entity_id_texture, NULL, 0,
        width, height,
        GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_NEAREST);

    glBindRenderbuffer(GL_RENDERBUFFER, buffer->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    
    glBindFramebuffer(GL_FRAMEBUFFER, buffer->fbo);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 0, GL_TEXTURE_2D,
        buffer->world_pos_texture.tex, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_2D,
        buffer->world_norm_texture.tex, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 2, GL_TEXTURE_2D,
        buffer->diffuse_texture.tex, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 3, GL_TEXTURE_2D,
        buffer->specular_texture.tex, 0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 4, GL_TEXTURE_2D,
        buffer->entity_id_texture.tex, 0
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
        buffer->rbo
    );

    GLuint buffers[5] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4
    };
    glDrawBuffers(5, buffers);

    check_framebuffer(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    buffer->width = width;
    buffer->height = height;

    return 1;
}

