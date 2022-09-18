typedef struct Bloom {
    size_t n_steps;

    GLuint program;
    GLuint inp_fbo;
    GLuint inp_textures[2];
    GLuint pingpong_fbo[2];
    GLuint pingpong_textures[2];
} Bloom;

void create_fbo(GLuint* fbo, GLuint* textures, size_t n_textures, size_t width, size_t height) {
    glGenFramebuffers(1, fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

    glGenTextures(n_textures, textures);
    for (size_t i = 0; i < n_textures; ++i) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
    }
}

bool bloom_create(Bloom* bloom, size_t n_steps, size_t scr_width, size_t scr_height) {
    memset(bloom, 0, sizeof(*bloom));
    GLuint program = glCreateProgram();

    bool is_linked = shader_link_program(
        "./shaders/common/plane.vert",
        NULL,
        NULL,
        "./shaders/postproc/blur.frag",
        program
    );
    if (!is_linked) {
        fprintf(stderr, "ERROR: failed to link blur program\n");
        return false;
    }

    GLuint inp_fbo;
    GLuint ping_fbo;
    GLuint pong_fbo;
    GLuint inp_textures[2];
    GLuint pingpong_textures[2];

    create_fbo(&inp_fbo, inp_textures, 2, scr_width, scr_height);
    GLenum color_attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, color_attachments);

    create_fbo(&ping_fbo, &pingpong_textures[0], 1, scr_width, scr_height);
    create_fbo(&pong_fbo, &pingpong_textures[1], 1, scr_width, scr_height);

    bloom->n_steps = n_steps;
    bloom->program = program;
    bloom->inp_fbo = inp_fbo;
    bloom->inp_textures[0] = inp_textures[0];
    bloom->inp_textures[1] = inp_textures[1];
    bloom->pingpong_textures[0] = pingpong_textures[0];
    bloom->pingpong_textures[1] = pingpong_textures[1];
    bloom->pingpong_fbo[0] = ping_fbo;
    bloom->pingpong_fbo[1] = pong_fbo;
}
