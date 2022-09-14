typedef struct Sphere {
    GLuint program;
    GLuint vbo;
    GLuint vao;
    GLuint ebo;

    GLuint vs_in_pos_loc;
    GLuint u_mv_loc;
    GLuint u_proj_loc;
    GLuint u_tess_lvl_inner_loc;
    GLuint u_tess_lvl_outer_loc;
    GLuint u_center_pos_loc;
    GLuint u_light_pos_loc;
} Sphere;

bool create_sphere(Sphere* sphere) {
    GLuint program = glCreateProgram();
    bool is_linked = link_program_files(
        "./shaders/sphere/sphere.vert",
        "./shaders/sphere/sphere.tesc",
        "./shaders/sphere/sphere.tese",
        "./shaders/sphere/sphere.frag",
        program
    );
    if (!is_linked) {
        fprintf(stderr, "ERROR: failed to link sphere program files\n");
        return false;
    }

    sphere->vs_in_pos_loc = glGetAttribLocation(program, "vs_in_pos");
    sphere->u_mv_loc = glGetUniformLocation(program, "u_mv");
    sphere->u_proj_loc = glGetUniformLocation(program, "u_proj");
    sphere->u_tess_lvl_inner_loc = glGetUniformLocation(program, "u_tess_lvl_inner");
    sphere->u_tess_lvl_outer_loc = glGetUniformLocation(program, "u_tess_lvl_outer");
    sphere->u_center_pos_loc = glGetUniformLocation(program, "u_center_pos");
    sphere->u_light_pos_loc = glGetUniformLocation(program, "u_light_pos");

    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ICOSAHEDRON_VERTS), ICOSAHEDRON_VERTS, GL_STATIC_DRAW);

    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(sphere->vs_in_pos_loc);
    glVertexAttribPointer(sphere->vs_in_pos_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    GLuint ebo;
    glCreateBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ICOSAHEDRON_FACES), ICOSAHEDRON_FACES, GL_STATIC_DRAW);

    sphere->program = program;
    sphere->vbo = vbo;
    sphere->vao = vao;
    sphere->ebo = ebo;

    return true;
}


// int main(void) {
//     glPatchParameteri(GL_PATCH_VERTICES, 3);
//     glUseProgram(program);
// 
//     while (!glfwWindowShouldClose(window)) {
//         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT);
// 
//         glUseProgram(program);
//         glUniformMatrix4fv(u_mv, 1, GL_TRUE, (float*)&VIEW);
//         glUniformMatrix4fv(u_proj, 1, GL_TRUE, (float*)&PROJ);
//         glUniform1f(u_tess_lvl_inner, 6.0f);
//         glUniform1f(u_tess_lvl_outer, 6.0f);
//         glUniform3f(u_center_pos, 0.0, 0.0, 0.0);
//         glUniform3f(u_light_pos, 0.0, 5.0, 0.0);
// 
//         glBindVertexArray(icos_vao);
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, icos_ebo);
//         glDrawElements(GL_PATCHES, sizeof(ICOSAHEDRON_FACES) / sizeof(ICOSAHEDRON_FACES[0]),  GL_UNSIGNED_BYTE, 0);
// 
//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }
//     glfwTerminate();
// }

