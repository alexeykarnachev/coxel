typedef struct EntityMouseDragArgs {
    int* entity;
    int active;
} EntityMouseDragArgs;


Vec3 _screen_to_world(
    float x,
    float y,
    Mat4* view_inv,
    Mat4* proj_inv
) {
    float ndc_x = 2.0f * x - 1.0f;
    float ndc_y = 2.0f * y - 1.0f;
    Vec4 clip = vec4(ndc_x, ndc_y, -1.0, 1.0);
    Vec4 view = mat4_vec4_mul(proj_inv, &clip);
    view = vec4(
        view.data[0], view.data[1], -1.0, 0.0);
    Vec4 world_v4 = mat4_vec4_mul(view_inv, &view);
    Vec3 world = {{
        world_v4.data[0], world_v4.data[1], world_v4.data[2]}};
    return world;
}

void _entity_mouse_drag_update(size_t _, void* args_p) {
    EntityMouseDragArgs* args = (EntityMouseDragArgs*)(args_p);

    if ((!args->active && !INPUT.g_pressed) || INPUT.mouse_left_pressed) {
        args->active = 0;
        return;
    }
    args->active = 1;

    int camera_entity = ecs_get_active_camera_entity();
    if (camera_entity == -1) {
        return;
    }

    
    int entity = *(args->entity);
    if (entity == -1) {
        return;
    }

    Transformation* entity_transformation = 
        (Transformation*)COMPONENTS[TRANSFORMATION_T][entity];

    if (!entity_transformation) {
        return;
    }

    // TODO: Refactor this mess.
    Camera* camera = (Camera*)COMPONENTS[CAMERA_T][camera_entity];
    Transformation* camera_transformation =
        (Transformation*)COMPONENTS[TRANSFORMATION_T][camera_entity];
    Mat4 proj = camera_get_perspective_projection_mat(camera);
    Mat4 view = camera_get_view_mat(camera, camera_transformation);
    Mat4 world = ecs_get_world_mat(entity);
    Mat4 proj_inv = mat4_inverse(&proj);
    Mat4 view_inv = mat4_inverse(&view);
    // Vec3 parent_pos = ecs_get_origin_position(entity);

    Vec3 entity_position = vec3(
        world.data[3], world.data[7], world.data[11]);
    Vec3 entity_to_camera = vec3_diff(
        &camera_transformation->position,
        &entity_position
    );
    float dist_to_camera = vec3_length(&entity_to_camera);

    Vec3 cursor_world = _screen_to_world(
        INPUT.cursor_x, INPUT.cursor_y, &view_inv, &proj_inv);
    Vec3 prev_cursor_world = _screen_to_world(
        INPUT.cursor_x - INPUT.cursor_dx,
        INPUT.cursor_y - INPUT.cursor_dy,
        &view_inv,
        &proj_inv
    );
    
    Vec3 cursor_world_translation = vec3_diff(
        &cursor_world, &prev_cursor_world);
    Vec3 new_entity_world_position = vec3_sum(
        &entity_position, &cursor_world_translation);
    Vec3 new_entity_to_camera = vec3_diff(
        &new_entity_world_position, &camera_transformation->position);
    new_entity_to_camera = vec3_norm(&new_entity_to_camera);
    new_entity_to_camera = vec3_scale(
        &new_entity_to_camera, dist_to_camera);
    new_entity_world_position = vec3_sum(
        &camera_transformation->position, &new_entity_to_camera);

    // Vec3 new_entity_world_position = vec3_scale(
    //     &cursor_world, dist_to_camera);
    // new_entity_world_position = vec3_sum(
    //     &camera_transformation->position, &new_entity_world_position);

    // Vec3 translation = vec3_diff(
    //     &new_entity_world_position, &entity_position);
    // Vec3 new_entity_position = vec3_sum(
    //     &entity_transformation->position, &translation
    // );

    entity_transformation->position = new_entity_world_position;
}

Script* entity_mouse_drag_create_script(
    EntityMouseDragArgs* args
) {
    return script_create(_entity_mouse_drag_update, args);
}
