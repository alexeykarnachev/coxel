void texture_create_cubemap_depth(GLuint* tex, size_t width, size_t height);


void texture_create_cubemap_depth(GLuint* tex, size_t width, size_t height) {
    glGenTextures(1, tex);
    GLuint internal_format = GL_DEPTH_COMPONENT;
    GLenum type = GL_DEPTH_COMPONENT;
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, *tex);
    for (size_t i = 0; i < 6; ++i) {
        GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        glTexImage2D(target, 0, internal_format, width, height, 0, type, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}



