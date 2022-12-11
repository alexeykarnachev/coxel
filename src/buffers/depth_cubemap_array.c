#include "depth_cubemap_array.h"

#include "../includes.h"

int depth_cubemap_array_create(
    DepthCubemapArray* d, size_t size, size_t n_layers
) {
    glGenFramebuffers(1, &d->fbo);
    glGenTextures(1, &d->tex);

    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, d->tex);
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

    glTexParameteri(
        GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR
    );
    glTexParameteri(
        GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR
    );
    glTexParameteri(
        GL_TEXTURE_CUBE_MAP_ARRAY,
        GL_TEXTURE_COMPARE_MODE,
        GL_COMPARE_REF_TO_TEXTURE
    );
    glTexParameteri(
        GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_GREATER
    );
    glTexParameteri(
        GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE
    );
    glTexParameteri(
        GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
    );
    glTexParameteri(
        GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE
    );

    glBindFramebuffer(GL_FRAMEBUFFER, d->fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, d->tex, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    check_framebuffer(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    d->size = size;
    d->n_layers = n_layers;

    return 1;
}
