typedef struct PointShadows {
    GLuint fbo;
    GLuint depth_cubemap;
} PointShadows;

bool point_shadows_create(PointShadows* point_shadow, size_t width, size_t height);
bool point_shadows_create_fbo(GLuint* fbo, GLuint* depth_cubemap, size_t width, size_t height);
void point_shadows_bind(PointShadows* point_shadows);


bool point_shadows_create(PointShadows* point_shadows, size_t width, size_t height) {
    memset(point_shadows, 0, sizeof(*point_shadows));

    GLuint fbo;
    GLuint depth_cubemap;

    if (!point_shadows_create_fbo(&fbo, &depth_cubemap, width, height)) {
        return false;
    }

    point_shadows->fbo = fbo;
    point_shadows->depth_cubemap = depth_cubemap;
    return true;
}

void point_shadows_bind(PointShadows* point_shadows) {
    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, point_shadows->fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
}

bool point_shadows_create_fbo(GLuint* fbo, GLuint* depth_cubemap, size_t width, size_t height) {
    glGenFramebuffers(1, fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

    glGenTextures(1, depth_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *depth_cubemap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (size_t i = 0; i < 6; ++i) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_DEPTH_COMPONENT,
            width,
            height,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            NULL
        );
    }

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *depth_cubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "ERROR: point shadows framebuffer not complete\n");
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

