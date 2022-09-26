typedef struct HDR {
    GLuint fbo;
    GLuint tex;
    GLuint program;
} HDR;

bool hdr_create(HDR* hdr, size_t scr_width, size_t scr_height) {
    memset(hdr, 0, sizeof(*hdr));

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scr_width, scr_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    GLuint depth_rbo;
    glGenRenderbuffers(1, &depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scr_width, scr_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "ERROR: Framebuffer not complete\n");
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLuint program = glCreateProgram();
    int n_deps = 1;
    const char* hdr_file_path = "./shaders/common/hdr.glsl"; 
    const char* deps_file_paths[1] = {hdr_file_path};
    bool is_linked = shader_link_program(
        "./shaders/common/plane.vert",
        NULL,
        NULL,
        "./shaders/postproc/hdr.frag",
        program,
        1,
        deps_file_paths
    );
    if (!is_linked) {
        fprintf(stderr, "ERROR: failed to link sphere program\n");
        return false;
    }

    hdr->fbo = fbo;
    hdr->tex = tex;
    hdr->program = program;
    return true;
}

void hdr_bind(HDR* hdr) {
    glBindFramebuffer(GL_FRAMEBUFFER, hdr->fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void hdr_draw(HDR* hdr, GLuint fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(hdr->program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdr->tex);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

