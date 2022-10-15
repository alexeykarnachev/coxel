#include "../includes.h"


int main(void) {
    Camera camera;
    float camera_fov = 45.0;
    float camera_near = 0.1;
    float camera_far = 1000.0;
    float camera_aspect = 1.7777;
    float camera_side_sens = 10.0;
    float camera_straight_sens = 1.0;
    float camera_rotation_sens = 1.0;
    Vec3 camera_start_pos = {{0.0, 0.0, 0.0}};
    camera_create(
        &camera,
        camera_fov,
        camera_near,
        camera_far,
        camera_aspect,
        camera_side_sens,
        camera_straight_sens,
        camera_rotation_sens, 
        camera_start_pos
    );

    Window window;
    window_create(&window, 1920, 1080, &camera);

    GLuint material_program = glCreateProgram();
    program_create_material(material_program);

    GLuint cubemap_depth_program = glCreateProgram();
    program_create_cubemap_depth(cubemap_depth_program);

    GLuint cubemap_depth_tex;
    texture_create_cubemap_depth(&cubemap_depth_tex, 1024, 1024);

    GLuint cubemap_depth_fbo;
    buffer_create_cubemap_depth(&cubemap_depth_fbo, &cubemap_depth_tex);

    PointLight point_light;
    Vec3 point_light_world_pos = {{0.0, 1.0, 3.0}};
    Vec3 point_light_color = {{1.0, 1.0, 1.0}};
    float point_light_energy = 200.0;
    point_light_create(
        &point_light,
        point_light_world_pos,
        point_light_color,
        point_light_energy
    );

    Material sphere_material;
    Vec3 sphere_diffuse_color = {{0.8, 0.2, 0.3}};
    Vec3 sphere_ambient_color = sphere_diffuse_color;
    Vec3 sphere_specular_color = {{1.0, 1.0, 1.0}};
    float sphere_shininess = 256.0;
    material_create(
        &sphere_material,
        sphere_diffuse_color,
        sphere_ambient_color,
        sphere_specular_color,
        sphere_shininess
    );

    Material plane_material;
    Vec3 plane_diffuse_color = {{0.8, 0.8, 0.8}};
    Vec3 plane_ambient_color = plane_diffuse_color;
    Vec3 plane_specular_color = {{1.0, 1.0, 1.0}};
    float plane_shininess = 256.0;
    material_create(
        &plane_material,
        plane_diffuse_color,
        plane_ambient_color,
        plane_specular_color,
        plane_shininess
    );

    Transformation sphere_transformation;
    Vec3 sphere_scale = {{1.0, 1.0, 1.0}};
    Vec3 sphere_rotation = {{0.0, 0.0, 0.0}};
    Vec3 sphere_translation = {{0.0, 0.0, -5.0}};
    transformation_create(&sphere_transformation, sphere_scale, sphere_rotation, sphere_translation);

    Transformation plane_transformation;
    Vec3 plane_scale = {{10.0, 10.0, 10.0}};
    Vec3 plane_rotation = {{0.0, 0.0, 0.0}};
    Vec3 plane_translation = {{0.0, 0.0, -10.0}};
    transformation_create(&plane_transformation, plane_scale, plane_rotation, plane_translation);

    Mesh sphere_mesh;
    mesh_create_icosahedron(&sphere_mesh, &sphere_material, &sphere_transformation);

    Mesh plane_mesh;
    mesh_create_plane(&plane_mesh, &plane_material, &plane_transformation);

    Renderer renderer;
    renderer_create(&renderer);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    while (!glfwWindowShouldClose(window.glfw_window)) {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render to the depth cubemap buffer:
        renderer_set_program(&renderer, cubemap_depth_program);
        renderer_set_fbo(&renderer, cubemap_depth_fbo);

        renderer_set_mesh(&renderer, &sphere_mesh);
        renderer_draw_triangles(&renderer);

        renderer_set_mesh(&renderer, &plane_mesh);
        renderer_draw_triangles(&renderer);

        // Render to the main buffer:
        renderer_set_program(&renderer, material_program);
        renderer_set_fbo(&renderer, 0);

        glViewport(0, 0, 1920, 1080);
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_depth_tex);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer_set_scene(&renderer, &camera, &point_light);

        renderer_set_mesh(&renderer, &sphere_mesh);
        renderer_draw_triangles(&renderer);

        renderer_set_mesh(&renderer, &plane_mesh);
        renderer_draw_triangles(&renderer);

        glfwSwapBuffers(window.glfw_window);
        glfwPollEvents();

        mesh_rotate(&sphere_mesh, 0.01, 0.01, 0.01);
    }
    glfwTerminate();
}

