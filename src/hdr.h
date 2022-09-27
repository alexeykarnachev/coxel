typedef struct HDR {
    GLuint fbo;
    GLuint rgba16f_buffer;
    GLuint program;
} HDR;

bool hdr_create_fbo(GLuint* fbo, GLuint* rgba16f_buffer, size_t scr_width, size_t scr_height);
bool hdr_create_program(GLuint* program);
bool hdr_create(HDR* hdr, size_t scr_width, size_t scr_height);
void hdr_bind(HDR* hdr);
void hdr_draw(HDR* hdr, GLuint fbo);

bool hdr_create(HDR* hdr, size_t scr_width, size_t scr_height) {
    memset(hdr, 0, sizeof(*hdr));

    GLuint fbo;
    GLuint rgba16f_buffer;
    GLuint program = glCreateProgram();

    if (
        !hdr_create_fbo(&fbo, &rgba16f_buffer, scr_width, scr_height)
        || !hdr_create_program(&program)
    ) {
        return false;
    }

    hdr->fbo = fbo;
    hdr->rgba16f_buffer = rgba16f_buffer;
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
    glBindTexture(GL_TEXTURE_2D, hdr->rgba16f_buffer);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

bool hdr_create_fbo(GLuint* fbo, GLuint* rgba16f_buffer, size_t scr_width, size_t scr_height) {
    GLuint depth_buffer;

    glGenFramebuffers(1, fbo);
    gl_create_rgba16f_buffer(rgba16f_buffer, scr_width, scr_height);
    gl_create_depth_buffer(&depth_buffer, scr_width, scr_height);

    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *rgba16f_buffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "ERROR: hdr framebuffer not complete\n");
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

bool hdr_create_program(GLuint* program) {
    int n_deps = 1;
    const char* hdr_file_path = "./shaders/common/hdr.glsl"; 
    const char* deps_file_paths[1] = {hdr_file_path};
    bool is_linked = shader_link_program(
        "./shaders/common/plane.vert",
        NULL,
        NULL,
        "./shaders/postproc/hdr.frag",
        *program,
        1,
        deps_file_paths
    );
    if (!is_linked) {
        fprintf(stderr, "ERROR: failed to link hdr program\n");
        return false;
    }

    return true;
}

