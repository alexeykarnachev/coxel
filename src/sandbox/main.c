#include "../includes.h"

int main(void) {
    window_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    Renderer renderer;
    renderer_create(
        &renderer, GBUFFER_WIDTH, GBUFFER_HEIGHT);

    size_t suzanne_0 = ecs_create_entity(-1);
    ecs_add_component(
        suzanne_0, TRANSFORMATION_T, 
        transformation_create(
            vec3(2.0, 2.0, 2.0),
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, 0.0, -10.0)
        )
    );
    ecs_add_component(
        suzanne_0, MESH_T, mesh_create_suzanne());
    ecs_add_component(
        suzanne_0, MATERIAL_T, material_create_default());

    size_t ironman_0 = ecs_create_entity(-1);
    ecs_add_component(
        ironman_0, TRANSFORMATION_T, 
        transformation_create(
            vec3(0.02, 0.02, 0.02),
            vec3(0.0, 0.0, 0.0),
            vec3(8.0, -2.0, -10.0)
        )
    );
    ecs_add_component(
        ironman_0, MESH_T, mesh_create_ironman());
    ecs_add_component(
        ironman_0, MATERIAL_T, material_create_default());

    size_t plane_0 = ecs_create_entity(-1);
    ecs_add_component(
        plane_0, TRANSFORMATION_T, 
        transformation_create(
            vec3(20.0, 1.0, 20.0),
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, -5.0, -20.0)
        )
    );
    ecs_add_component(
        plane_0, MESH_T, mesh_create_plane());
    ecs_add_component(
        plane_0, MATERIAL_T, material_create_default());

    size_t surf_sphere_0 = ecs_create_entity(-1);
    ecs_add_component(
        surf_sphere_0, TRANSFORMATION_T, 
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 0.0),
            vec3(5.0, 2.0, -2.0)
        )
    );
    ecs_add_component(
        surf_sphere_0, MESH_T, mesh_create_surf_sphere());
    ecs_add_component(
        surf_sphere_0, MATERIAL_T, 
        material_create(vec3(0.9, 0.2, 0.2), vec3(1.0, 1.0, 1.0))
    );

    size_t sphere_0 = ecs_create_entity(-1);
    ecs_add_component(
        sphere_0, TRANSFORMATION_T, 
        transformation_create(
            vec3(5.0, 1.0, 1.0),
            vec3(0.0, 0.0, deg2rad(45.0)),
            vec3(-2.0, 2.0, -2.0)
        )
    );
    ecs_add_component(
        sphere_0, MESH_T, mesh_create_sphere());
    ecs_add_component(
        sphere_0, MATERIAL_T, 
        material_create(vec3(0.9, 0.2, 0.2), vec3(1.0, 1.0, 1.0))
    );

    size_t sphere_1 = ecs_create_entity(-1);
    ecs_add_component(
        sphere_1, TRANSFORMATION_T, 
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, 2.0, -2.0)
        )
    );
    ecs_add_component(
        sphere_1, MESH_T, mesh_create_sphere());
    ecs_add_component(
        sphere_1, MATERIAL_T, 
        material_create(vec3(0.2, 0.9, 0.2), vec3(1.0, 1.0, 1.0))
    );

    size_t sphere_2 = ecs_create_entity(-1);
    ecs_add_component(
        sphere_2, TRANSFORMATION_T, 
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, deg2rad(45.0)),
            vec3(2.0, 2.0, -2.0)
        )
    );
    ecs_add_component(
        sphere_2, MESH_T, mesh_create_sphere());
    ecs_add_component(
        sphere_2, MATERIAL_T, 
        material_create(vec3(0.2, 0.2, 0.9), vec3(1.0, 1.0, 1.0))
    );

    size_t sphere_3 = ecs_create_entity(sphere_2);
    ecs_add_component(
        sphere_3, TRANSFORMATION_T, 
        transformation_create(
            vec3(0.3, 0.3, 0.3),
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, -1.5, 0.0)
        )
    );
    ecs_add_component(
        sphere_3, MESH_T, mesh_create_sphere());
    ecs_add_component(
        sphere_3, MATERIAL_T, 
        material_create(vec3(0.1, 0.1, 0.95), vec3(1.0, 1.0, 1.0))
    );

    size_t sphere_4 = ecs_create_entity(sphere_2);
    ecs_add_component(
        sphere_4, TRANSFORMATION_T, 
        transformation_create(
            vec3(0.3, 0.3, 0.3),
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, 1.5, 0.0)
        )
    );
    ecs_add_component(
        sphere_4, MESH_T, mesh_create_sphere());
    ecs_add_component(
        sphere_4, MATERIAL_T, 
        material_create(vec3(0.1, 0.1, 0.95), vec3(1.0, 1.0, 1.0))
    );
     
    Texture sun_icon_texture;
    texture_create_2d(
        &sun_icon_texture, SUN_ICON_32.pixel_data, 0,
        SUN_ICON_32.width, SUN_ICON_32.height,
        GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR);

    size_t point_light_0 = ecs_create_entity(-1);
    ecs_add_component(
        point_light_0, TRANSFORMATION_T,
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, 12.0, 0.0)
        )
    );
    ecs_add_component(
        point_light_0, POINT_LIGHT_T, point_light_create_default());
    ecs_add_component(
        point_light_0, SPRITE_T, sprite_create(0.0, 0.0, 1.0, 1.0, &sun_icon_texture));
    ecs_add_component(
        point_light_0, MATERIAL_T, material_create_default());

    size_t point_light_1 = ecs_create_entity(-1);
    ecs_add_component(
        point_light_1, TRANSFORMATION_T,
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, 7.0, 7.0)
        )
    );
    ecs_add_component(
        point_light_1, POINT_LIGHT_T, point_light_create_default());
    ecs_add_component(
        point_light_1, SPRITE_T, sprite_create(0.0, 0.0, 1.0, 1.0, &sun_icon_texture));
    ecs_add_component(
        point_light_1, MATERIAL_T, material_create_default());

    size_t point_light_2 = ecs_create_entity(-1);
    ecs_add_component(
        point_light_2, TRANSFORMATION_T,
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 0.0),
            vec3(-7.0, 7.0, 7.0)
        )
    );
    ecs_add_component(
        point_light_2, POINT_LIGHT_T, point_light_create_default());
    ecs_add_component(
        point_light_2, SPRITE_T, sprite_create(0.0, 0.0, 1.0, 1.0, &sun_icon_texture));
    ecs_add_component(
        point_light_2, MATERIAL_T, material_create_default());

    size_t point_light_3 = ecs_create_entity(-1);
    ecs_add_component(
        point_light_3, TRANSFORMATION_T,
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 0.0),
            vec3(7.0, 7.0, 7.0)
        )
    );
    ecs_add_component(
        point_light_3, POINT_LIGHT_T, point_light_create_default());
    ecs_add_component(
        point_light_3, SPRITE_T, sprite_create(0.0, 0.0, 1.0, 1.0, &sun_icon_texture));
    ecs_add_component(
        point_light_3, MATERIAL_T, material_create_default());

    size_t camera_0 = ecs_create_entity(-1);
    ecs_add_component(
        camera_0, TRANSFORMATION_T,
        transformation_create(vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 5.0))
    );
    ecs_add_component(
        camera_0, CAMERA_T, camera_create_default());
    // TODO: use script args as a pointer? Like all other components.
    CameraMouseControllerArgs camera_mouse_controller_args =
        camera_mouse_controller_create_default_args();
    ecs_add_component(
        camera_0, SCRIPT_T,
        camera_mouse_controller_create_script(
            &camera_mouse_controller_args));

    entity_create_gui_pane(
        20, 20, 800, 72, 24, "Hello, ZALOOPA 8==o");

    size_t editor_entity_controller_0 = ecs_create_entity(-1);
    EditorEntityControllerArgs editor_entity_controller_args =
        editor_entity_controller_create_default_args(&renderer.gbuffer);
    ecs_add_component(
        editor_entity_controller_0, SCRIPT_T,
        editor_entity_controller_create_script(
            &editor_entity_controller_args));

    while (!INPUT.window_should_close) {
        window_update();
        ecs_update();
        renderer_update(&renderer);
    }

    glfwTerminate();
}

