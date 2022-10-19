typedef struct Renderer {
    GLuint material_program;
    GLuint shadow_program;
    GLuint current_fbo;

    float shadow_near_plane;
    float shadow_far_plane;
    float shadow_disk_radius;
    float shadow_bias;
    size_t shadow_n_samples;
    DepthCubemap cube_shadowmap;

    Camera* camera;

    Mesh* meshes;
    size_t n_meshes;

    PointLight* point_lights;
    size_t n_point_lights;
} Renderer;

bool renderer_create(
    Renderer* renderer,
    size_t shadow_size,
    float shadow_near_plane,
    float shadow_far_plane,
    float shadow_disk_radius,
    float shadow_bias,
    size_t shadow_n_samples
);
void renderer_set_program(Renderer* renderer, GLuint program);
bool renderer_draw_mesh(Renderer* renderer, Mesh* mesh);
void renderer_set_scene(
    Renderer* renderer,
    Camera* camera,
    Mesh* meshes,
    size_t n_meshes,
    PointLight* point_lights,
    size_t n_point_lights
);
bool renderer_set_depth_cubemap_fbo(Renderer* renderer, DepthCubemap* depth_cubemap);
void renderer_draw_triangles(Renderer* renderer);
void renderer_draw_patches(Renderer* renderer, size_t n_vertices_in_patch);

void _renderer_set_fbo(Renderer* renderer, GLuint fbo, size_t viewport_width, size_t viewport_height);


bool renderer_create(
    Renderer* renderer,
    size_t shadow_size,
    float shadow_near_plane,
    float shadow_far_plane,
    float shadow_disk_radius,
    float shadow_bias,
    size_t shadow_n_samples
) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    renderer->material_program = glCreateProgram();
    renderer->shadow_program = glCreateProgram();
    program_create_material(renderer->material_program);
    program_create_depth_cubemap(renderer->shadow_program);
    
    renderer->shadow_near_plane = shadow_near_plane;
    renderer->shadow_far_plane = shadow_far_plane;
    renderer->shadow_disk_radius = shadow_disk_radius;
    renderer->shadow_bias = shadow_bias;
    renderer->shadow_n_samples = shadow_n_samples;
    depth_cubemap_create(&renderer->cube_shadowmap, shadow_size);
}

bool renderer_draw_shadows(Renderer* renderer) {
    _renderer_set_fbo(
        renderer,
        renderer->cube_shadowmap.fbo,
        renderer->cube_shadowmap.size, 
        renderer->cube_shadowmap.size
    );

    GLuint p = renderer->shadow_program;
    glUseProgram(p);
    bool ok = true;
    ok &= program_set_uniform_1f(p, "max_dist", renderer->shadow_far_plane);

    for (size_t i = 0; i < renderer->n_point_lights; ++i) {
        depth_cubemap_set_view(
            &renderer->cube_shadowmap,
            renderer->point_lights[i].world_pos,
            renderer->shadow_near_plane,
            renderer->shadow_far_plane
        );

        ok &= program_set_uniform_matrix_4fv(p, "view_proj_mats", renderer->cube_shadowmap.view_proj_mats, 6, GL_TRUE);
        ok &= program_set_uniform_3fv(p, "world_pos", renderer->cube_shadowmap.world_pos.data, 1);

        for (size_t j = 0; j < renderer->n_meshes; ++j) {
            Mesh* mesh = &(renderer->meshes[j]);
            ok &= program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation.world_mat.data, 1, GL_TRUE);

            if (!ok) {
                return false;
            }

            mesh_bind(mesh);
            ok &= program_set_attribute(p, "model_pos", 3, GL_FLOAT); 
            glDrawElements(GL_TRIANGLES, mesh->n_elements, GL_UNSIGNED_BYTE, 0);
        }
    }

    return true;
}

bool renderer_draw_materials(
    Renderer* renderer,
    size_t viewport_width,
    size_t viewport_height
) {
    _renderer_set_fbo(renderer, 0, viewport_width, viewport_height);
    
    PointLight point_light = renderer->point_lights[1];

    GLuint p = renderer->material_program;
    glUseProgram(p);
    bool ok = true;
    ok &= program_set_uniform_matrix_4fv(p, "view_mat", renderer->camera->view_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "proj_mat", renderer->camera->proj_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_3fv(p, "eye_world_pos", renderer->camera->pos.data, 1);
    ok &= program_set_uniform_3fv(p, "point_light_world_pos", point_light.world_pos.data, 1);
    ok &= program_set_uniform_3fv(p, "point_light_color", point_light.color.data, 1);
    ok &= program_set_uniform_1f(p, "point_light_energy", point_light.energy);
    ok &= program_set_uniform_1f(p, "shadow_far_plane", renderer->shadow_far_plane);
    ok &= program_set_uniform_1i(p, "shadow_n_samples", renderer->shadow_n_samples);
    ok &= program_set_uniform_1f(p, "shadow_disk_radius", renderer->shadow_disk_radius);
    ok &= program_set_uniform_1f(p, "shadow_bias", renderer->shadow_bias);

    for (size_t i = 0; i < renderer->n_meshes; ++i) {
        Mesh* mesh = &(renderer->meshes[i]);
        ok &= program_set_uniform_3fv(p, "diffuse_color", mesh->material.diffuse_color.data, 1);
        ok &= program_set_uniform_3fv(p, "ambient_color", mesh->material.ambient_color.data, 1);
        ok &= program_set_uniform_3fv(p, "specular_color", mesh->material.specular_color.data, 1);
        ok &= program_set_uniform_1f(p, "shininess", mesh->material.shininess);
        ok &= program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation.world_mat.data, 1, GL_TRUE);

        if (!ok) {
            return false;
        }

        mesh_bind(mesh);
        ok &= program_set_attribute(p, "model_pos", 3, GL_FLOAT); 
        glDrawElements(GL_TRIANGLES, mesh->n_elements, GL_UNSIGNED_BYTE, 0);
    }

    return ok;
}

void renderer_set_scene(
    Renderer* renderer,
    Camera* camera,
    Mesh* meshes,
    size_t n_meshes,
    PointLight* point_lights,
    size_t n_point_lights
) {
    renderer->camera = camera;
    renderer->meshes = meshes;
    renderer->n_meshes = n_meshes;
    renderer->point_lights = point_lights;
    renderer->n_point_lights = n_point_lights;
}

void _renderer_set_fbo(Renderer* renderer, GLuint fbo, size_t viewport_width, size_t viewport_height) {
    if (renderer->current_fbo == fbo) {
        return;
    }
    renderer->current_fbo = fbo;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, viewport_width, viewport_height);
}

