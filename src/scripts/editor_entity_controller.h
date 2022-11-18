typedef enum MODE {SELECT, DRAG} MODE;
typedef enum DRAG_AXIS {X, Y, Z, VIEWPORT} DRAG_AXIS;

typedef struct EditorEntityControllerArgs {
    int entity;

    Vec3 select_color;
    TextureBuffer* gbuffer;

    MODE mode;
    DRAG_AXIS drag_axis;

    float cursor_start_x;
    float cursor_start_y;
    float cursor_x;
    float cursor_y;
    Vec3 entity_start_world_position;
    Vec3 entity_start_local_position;
} EditorEntityControllerArgs;

Material* _get_material(int entity) {
    if (entity == -1) {
        return NULL;
    }
    Material* material = (Material*)COMPONENTS[MATERIAL_T][entity];
    return material;
}

void _editor_entity_controller_update(size_t _, void* args_p) {
    EditorEntityControllerArgs* args = (EditorEntityControllerArgs*)(args_p);

    unsigned char id = 0;
    int x = (int)(INPUT.cursor_x * args->gbuffer->width);
    int y = (int)(INPUT.cursor_y * args->gbuffer->height);
    glBindFramebuffer(GL_FRAMEBUFFER, args->gbuffer->fbo);
    glReadPixels(x, y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &id);

    int entity = (int)id - 1;

    if (INPUT.mouse_left_released && args->mode != SELECT) {
        args->mode = SELECT;
        return;
    }
    
    entity = INPUT.mouse_left_released ? entity : args->entity;
    Material* material = _get_material(entity);

    int prev_entity = args->entity;
    Material* prev_material = _get_material(prev_entity);

    if (prev_material) {
        prev_material->constant_color = vec3_zeros;
    }
    if (material) {
        material->constant_color = args->select_color;
    }

    args->entity = entity;

    // ----------------------------------------------

    if (!INPUT.g_pressed && args->mode != DRAG) {
        return;
    }

    int camera_entity = ecs_get_active_camera_entity();
    if (camera_entity == -1) {
        return;
    }
    
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

    if (args->mode != DRAG) {
        args->mode = DRAG;
        args->drag_axis = VIEWPORT;
        args->cursor_x = 0.5 * (entity_proj_position.data[0] + 1.0);
        args->cursor_y = 0.5 * (entity_proj_position.data[1] + 1.0);
        args->cursor_start_x = args->cursor_x;
        args->cursor_start_y = args->cursor_y;
        args->entity_start_world_position = entity_world_position;
        args->entity_start_local_position = entity_transformation->position;
    } else if (INPUT.x_released) {
        // args->drag_axis = args->drag_axis == X ? VIEWPORT : X;
        args->drag_axis = X;
        entity_transformation->position = args->entity_start_local_position;
        args->cursor_x = args->cursor_start_x;
        args->cursor_y = args->cursor_start_y;
        return;
    } else if (INPUT.y_released) {
        // args->drag_axis = args->drag_axis == Y ? VIEWPORT : Y;
        args->drag_axis = Y;
        entity_transformation->position = args->entity_start_local_position;
        args->cursor_x = args->cursor_start_x;
        args->cursor_y = args->cursor_start_y;
        return;
    } else if (INPUT.z_released) {
        // args->drag_axis = args->drag_axis == Z ? VIEWPORT : Z;
        args->drag_axis = Z;
        entity_transformation->position = args->entity_start_local_position;
        args->cursor_x = args->cursor_start_x;
        args->cursor_y = args->cursor_start_y;
        return;
    }

    if ((fabs(INPUT.cursor_dx) + fabs(INPUT.cursor_dy)) < EPS) {
        return;
    }

    args->cursor_x += INPUT.cursor_dx;
    args->cursor_y += INPUT.cursor_dy;

    float cursor_proj_x = args->cursor_x * 2.0 - 1.0;
    float cursor_proj_y = args->cursor_y * 2.0 - 1.0;

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

    if (args->drag_axis != VIEWPORT) {
        Vec3 line = vec3_zeros;
        line.data[args->drag_axis] = 1.0;
        Vec3 cursor_plane_vec = vec3_cross(&cursor_dir, &line);
        cursor_plane_vec = vec3_norm(&cursor_plane_vec);
        Vec3 cursor_plane_normal = vec3_cross(&cursor_dir, &cursor_plane_vec);
        cursor_plane_normal = vec3_norm(&cursor_plane_normal);

        Vec3 isect_p;
        Vec3 line_p = args->entity_start_world_position;
        line_p.data[args->drag_axis] += 1;
        int is_isect = isect_line_plane(
            &isect_p,
            &args->entity_start_world_position,
            &line_p,
            &cursor_near_world,
            &cursor_plane_normal
        );

        if (!is_isect) {
            return;
        }

        entity_world_position.data[args->drag_axis] = isect_p.data[args->drag_axis];

        Mat4 origin_world_mat = ecs_get_origin_world_mat(entity);
        Mat4 origin_world_inv = mat4_inverse(&origin_world_mat);
        Vec4 entity_local_position_4 = mat4_vec3_mul(
            &origin_world_inv, &entity_world_position);
        Vec3 entity_local_position = vec4_to_vec3(&entity_local_position_4);
        entity_transformation->position = entity_local_position;
    }
}

EditorEntityControllerArgs editor_entity_controller_create_default_args(
    TextureBuffer* gbuffer
) {
    EditorEntityControllerArgs args;

    args.entity = -1;
    args.select_color = DEFAULT_SELECT_COLOR;
    args.gbuffer = gbuffer;
    args.mode = SELECT;
    args.drag_axis = VIEWPORT;

    return args;
}

Script* editor_entity_controller_create_script(
    EditorEntityControllerArgs* args
) {
    return script_create(_editor_entity_controller_update, args);
}
