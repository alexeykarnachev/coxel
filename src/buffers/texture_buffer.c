#include "texture_buffer.h"
#include "../includes.h"


int texture_buffer_create(
    TextureBuffer* buffer,
    void* data,
    size_t width,
    size_t height,
    size_t internal_format,
    size_t format,
    size_t type
) {
    glGenFramebuffers(1, &buffer->fbo);
    glGenRenderbuffers(1, &buffer->rbo);
    texture_create_2d(
        &buffer->texture, data, 0,
        width, height,
        internal_format, format, type, GL_NEAREST);

    glBindRenderbuffer(GL_RENDERBUFFER, buffer->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    
    glBindFramebuffer(GL_FRAMEBUFFER, buffer->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->texture.tex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer->rbo);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    check_framebuffer(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    buffer->width = width;
    buffer->height = height;

    return 1;
}

