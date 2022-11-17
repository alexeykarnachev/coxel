typedef struct EntityMouseDragArgs {
    int* entity;
    int active;
    Transformation entity_start_transformation;
} EntityMouseDragArgs;

float DONE = false;

void _entity_mouse_drag_update(size_t _, void* args_p) {
    EntityMouseDragArgs* args = (EntityMouseDragArgs*)(args_p);

    if ((!args->active && !INPUT.g_pressed) || INPUT.mouse_left_pressed) {
        args->active = 0;
        return;
    }

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

    if (!args->active) {
        args->entity_start_transformation = *entity_transformation;
        args->active = 1;
    }


    Camera* camera = (Camera*)COMPONENTS[CAMERA_T][camera_entity];
    Transformation* camera_transformation =
        (Transformation*)COMPONENTS[TRANSFORMATION_T][camera_entity];
    Mat4 proj_mat = camera_get_perspective_projection_mat(camera);
    Mat4 view_mat = camera_get_view_mat(camera, camera_transformation);
    Mat4 vp_mat = mat4_mat4_mul(&proj_mat, &view_mat);

    Mat4 proj_inv = mat4_inverse(&proj_mat);
    Mat4 view_inv = mat4_inverse(&view_mat);
    Mat4 vp_inv = mat4_mat4_mul(&view_inv, &proj_inv);

    Vec3 entity_world_position = ecs_get_world_position(entity);
    Vec4 entity_proj_position_4 = mat4_vec3_mul(
        &vp_mat, &entity_world_position);
    Vec3 entity_proj_position = vec4_to_vec3(&entity_proj_position_4);
    entity_proj_position = vec3_scale(
        &entity_proj_position, 1.0 / entity_proj_position_4.data[3]);

    float proj_z = entity_proj_position.data[2];

    // ----------------------------------------
    float proj_x = INPUT.cursor_x * 2.0 - 1.0;
    float proj_y = INPUT.cursor_y * 2.0 - 1.0;

    Vec4 proj_pos_ent = {{proj_x, proj_y, proj_z, 1.0}};
    Vec4 world_pos_ent_4 = mat4_vec4_mul(&vp_inv, &proj_pos_ent);
    Vec3 world_pos_ent = vec4_to_vec3(&world_pos_ent_4);

    world_pos_ent = vec3_scale(
        &world_pos_ent, 1.0 / world_pos_ent_4.data[3]);

    world_pos_ent = vec3(
        args->entity_start_transformation.position.data[0] + world_pos_ent.data[0],
        args->entity_start_transformation.position.data[1],
        args->entity_start_transformation.position.data[2]
    );

    // ----------------------------------------

    // Mat4 origin_world_mat = ecs_get_origin_world_mat(entity);
    // origin_world_mat = mat4_inverse(&origin_world_mat);
    // Vec4 entity_new_position_4 = mat4_vec3_mul(
    //     &origin_world_mat, &entity_new_world_position);
    // entity_transformation->position = vec4_to_vec3(
    //     &entity_new_position_4);

    // if (!DONE) {
    if (1) {
        entity_transformation->position = world_pos_ent;
        DONE = true;
        printf("pos:");
        vec3_print(&entity_transformation->position);
        printf("cam:");
        vec3_print(&camera_transformation->position);
    }
}

Script* entity_mouse_drag_create_script(
    EntityMouseDragArgs* args
) {
    return script_create(_entity_mouse_drag_update, args);
}
