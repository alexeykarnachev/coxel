#include "overlay_buffer.h"

#include "../includes.h"

int overlay_buffer_create(
    OverlayBuffer* buffer, size_t width, size_t height
) {
    glGenFramebuffers(1, &buffer->fbo);
    glGenRenderbuffers(1, &buffer->rbo);

    texture_create_2d(
        &buffer->outline_texture,
        NULL,
        0,
        width,
        height,
        GL_R8UI,
        GL_RED_INTEGER,
        GL_UNSIGNED_BYTE,
        GL_NEAREST
    );

    texture_create_2d(
        &buffer->gui_rect_texture,
        NULL,
        0,
        width,
        height,
        GL_RGBA,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        GL_NEAREST
    );

    texture_create_2d(
        &buffer->gui_text_texture,
        NULL,
        0,
        width,
        height,
        GL_RGBA,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        GL_NEAREST
    );

    // TODO: 1 byte for element is not enough for sure!
    // There could be more than 256 entities
    texture_create_2d(
        &buffer->entity_id_texture,
        NULL,
        0,
        width,
        height,
        GL_R8UI,
        GL_RED_INTEGER,
        GL_UNSIGNED_BYTE,
        GL_NEAREST
    );

    glBindRenderbuffer(GL_RENDERBUFFER, buffer->rbo);
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height
    );

    glBindFramebuffer(GL_FRAMEBUFFER, buffer->fbo);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + 0,
        GL_TEXTURE_2D,
        buffer->outline_texture.tex,
        0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + 1,
        GL_TEXTURE_2D,
        buffer->gui_rect_texture.tex,
        0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + 2,
        GL_TEXTURE_2D,
        buffer->gui_text_texture.tex,
        0
    );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + 3,
        GL_TEXTURE_2D,
        buffer->entity_id_texture.tex,
        0
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer->rbo
    );

    GLuint buffers[4] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, buffers);
    glReadBuffer(GL_COLOR_ATTACHMENT3);

    check_framebuffer(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    buffer->width = width;
    buffer->height = height;

    return 1;
}
