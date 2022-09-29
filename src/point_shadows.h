typedef struct PointShadows {
    GLuint fbo;
    GLuint depth_cubemap;
} PointShadows;

bool point_shadows_create(PointShadows* point_shadow, size_t scr_width, size_t scr_height);
bool point_shadows_create_fbo(GLuint* fbo, GLuint* depth_cubemap, size_t scr_width, size_t scr_height);
void point_shadows_bind(PointShadows* point_shadows);


bool point_shadows_create(PointShadows* point_shadows, size_t scr_width, size_t scr_height) {
    memset(point_shadows, 0, sizeof(*point_shadows));

    GLuint fbo;
    GLuint depth_cubemap;

    if (!point_shadows_create_fbo(&fbo, &depth_cubemap, scr_width, scr_height)) {
        return false;
    }

    point_shadows->fbo = fbo;
    point_shadows->depth_cubemap = depth_cubemap;
    return true;
}

void point_shadows_bind(PointShadows* point_shadows) {
    // TODO: Do we need to set glViewport here? Like so: glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, point_shadows->fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
}

bool point_shadows_create_fbo(GLuint* fbo, GLuint* depth_cubemap, size_t scr_width, size_t scr_height) {
    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *depth_cubemap, 0);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "ERROR: point shadows framebuffer not complete\n");
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

