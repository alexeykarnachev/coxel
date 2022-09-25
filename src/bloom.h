typedef struct Bloom {
    GLuint blur_program;
    GLuint bloom_program;
    GLuint inp_fbo;
    GLuint inp_textures[2];
    GLuint pingpong_fbo[2];
    GLuint pingpong_textures[2];

    GLuint u_is_horizontal;
} Bloom;

void bloom_create_fbo(GLuint* fbo, GLuint* textures, size_t n_textures, size_t width, size_t height) {
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

    GLuint rbo_depth;
    glGenRenderbuffers(1, &rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool bloom_create(Bloom* bloom, size_t scr_width, size_t scr_height) {
    memset(bloom, 0, sizeof(*bloom));

    GLuint blur_program = glCreateProgram();
    bool is_linked = shader_link_program(
        "./shaders/common/plane.vert",
        NULL,
        NULL,
        "./shaders/postproc/blur.frag",
        blur_program,
        0,
        NULL
    );
    if (!is_linked) {
        fprintf(stderr, "ERROR: failed to link blur program\n");
        return false;
    }

    GLuint bloom_program = glCreateProgram();
    
    const char* deps_file_paths[1] = {"./shaders/common/hdr.glsl"};
    is_linked = shader_link_program(
        "./shaders/common/plane.vert",
        NULL,
        NULL,
        "./shaders/postproc/bloom.frag",
        bloom_program,
        1,
        deps_file_paths 
    );
    if (!is_linked) {
        fprintf(stderr, "ERROR: failed to link bloom program\n");
        return false;
    }

    GLuint inp_fbo;
    GLuint ping_fbo;
    GLuint pong_fbo;
    GLuint inp_textures[2];
    GLuint pingpong_textures[2];

    bloom_create_fbo(&inp_fbo, inp_textures, 2, scr_width, scr_height);
    bloom_create_fbo(&ping_fbo, &pingpong_textures[0], 1, scr_width, scr_height);
    bloom_create_fbo(&pong_fbo, &pingpong_textures[1], 1, scr_width, scr_height);

    bloom->blur_program = blur_program;
    bloom->bloom_program = bloom_program;
    bloom->inp_fbo = inp_fbo;
    bloom->inp_textures[0] = inp_textures[0];
    bloom->inp_textures[1] = inp_textures[1];
    bloom->pingpong_textures[0] = pingpong_textures[0];
    bloom->pingpong_textures[1] = pingpong_textures[1];
    bloom->pingpong_fbo[0] = ping_fbo;
    bloom->pingpong_fbo[1] = pong_fbo;

    bool ok = true;
    ok &= shader_get_uniform_location(&(bloom->u_is_horizontal), bloom->blur_program, "u_is_horizontal");
    if (!ok) {
        fprintf(stderr, "ERROR: failed to find some attribute or uniform locations in the blur program\n");
        return false;
    }

    return true;
}

void bloom_bind(Bloom* bloom) {
    glBindFramebuffer(GL_FRAMEBUFFER, bloom->inp_fbo);
    GLenum color_attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, color_attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, bloom->inp_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void bloom_draw(Bloom* bloom, size_t n_iters) {
    glUseProgram(bloom->blur_program);

    for (unsigned int i = 0; i < n_iters; i++)
    {
        bool is_horizontal = i % 2 == 0;
        int tex = i == 0 ? bloom->inp_textures[1] : bloom->pingpong_textures[!is_horizontal];

        glBindFramebuffer(GL_FRAMEBUFFER, bloom->pingpong_fbo[is_horizontal]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1f(bloom->u_is_horizontal, is_horizontal);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(bloom->bloom_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bloom->inp_textures[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloom->pingpong_textures[(n_iters - 1) % 2 == 0]);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
