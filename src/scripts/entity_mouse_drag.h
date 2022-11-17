typedef struct EntityMouseDragArgs {
    int* entity;
    int active;
} EntityMouseDragArgs;

void _entity_mouse_drag_update(size_t _, void* args_p) {
    EntityMouseDragArgs* args = (EntityMouseDragArgs*)(args_p);

    if (
        (!args->active && !INPUT.g_pressed)
        || INPUT.mouse_left_pressed
        || INPUT.mouse_middle_pressed
    ) {
        args->active = 0;
        return;
    }
    args->active = 1;

    if ((fabs(INPUT.cursor_dx) + fabs(INPUT.cursor_dy)) < EPS) {
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

    float cursor_proj_x = INPUT.cursor_x * 2.0 - 1.0;
    float cursor_proj_y = INPUT.cursor_y * 2.0 - 1.0;

    Vec4 cursor_near_proj = {{cursor_proj_x, cursor_proj_y, -1.0, 1.0}};
    Vec4 cursor_near_world_4 = mat4_vec4_mul(&vp_inv, &cursor_near_proj);
    Vec3 cursor_near_world = vec4_to_vec3(&cursor_near_world_4);
    cursor_near_world = vec3_scale(&cursor_near_world, 1.0 / cursor_near_world_4.data[3]);

    Vec4 cursor_far_proj = {{cursor_proj_x, cursor_proj_y, 1.0, 1.0}};
    Vec4 cursor_far_world_4 = mat4_vec4_mul(&vp_inv, &cursor_far_proj);
    Vec3 cursor_far_world = vec4_to_vec3(&cursor_far_world_4);
    cursor_far_world = vec3_scale(&cursor_far_world, 1.0 / cursor_far_world_4.data[3]);
    
    Vec3 cursor_dir = vec3_diff(&cursor_far_world, &cursor_near_world);
    cursor_dir = vec3_norm(&cursor_dir);
    Vec3 cursor_plane_vec = vec3_cross(&cursor_dir, &vec3_pos_x);
    cursor_plane_vec = vec3_norm(&cursor_plane_vec);
    Vec3 cursor_plane_perp = vec3_cross(&cursor_dir, &cursor_plane_vec);
    cursor_plane_perp = vec3_norm(&cursor_plane_perp);

    // https://stackoverflow.com/questions/5666222/3d-line-plane-intersection
    // p0, p1: Define the line.
    // p_co, p_no: define the plane:
    //     p_co Is a point on the plane (plane coordinate).
    //     p_no Is a normal vector defining the plane direction;
    //          (does not need to be normalized).

    // Return a Vector or None (when the intersection can't be found).

    Vec3 u = vec3_pos_x;
    float dot = vec3_dot(&cursor_plane_perp, &u);
    Vec3 w = vec3_negate(&cursor_near_world);
    float fac = -vec3_dot(&cursor_plane_perp, &w) / dot;
    u = vec3_scale(&u, fac);

    vec3_print(&u);
    printf("\n");

    entity_transformation->position.data[0] = u.data[0];
    // ----------------------------------------

    // Mat4 origin_world_mat = ecs_get_origin_world_mat(entity);
    // origin_world_mat = mat4_inverse(&origin_world_mat);
    // Vec4 entity_new_position_4 = mat4_vec3_mul(
    //     &origin_world_mat, &entity_new_world_position);
    // entity_transformation->position = vec4_to_vec3(
    //     &entity_new_position_4);
}

Script* entity_mouse_drag_create_script(
    EntityMouseDragArgs* args
) {
    return script_create(_entity_mouse_drag_update, args);
}
