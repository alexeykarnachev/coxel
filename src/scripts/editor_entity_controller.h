typedef enum MODE {SELECT, DRAG} MODE;
typedef enum DRAG_AXIS {X, Y, Z, VIEWPORT} DRAG_AXIS;

typedef struct EditorEntityControllerArgs {
    int entity;

    Vec3 select_color;
    TextureBuffer* gbuffer;

    MODE mode;
    DRAG_AXIS drag_axis;

    float cursor_x;
    float cursor_y;
    Vec3 entity_start_world_position;
    Vec3 entity_start_local_position;
} EditorEntityControllerArgs;


static Material* get_material(int entity) {
    if (entity == -1) {
        return NULL;
    }
    Material* material = (Material*)COMPONENTS[MATERIAL_T][entity];
    return material;
}

static int key_to_axis(int key) {
    if (key == 88) {
        return X;
    } else if (key == 89) {
        return Y;
    } else if (key == 90) {
        return Z;
    }
    return VIEWPORT;
}

void _editor_entity_controller_update(size_t _, void* args_p) {
    EditorEntityControllerArgs* args = (EditorEntityControllerArgs*)(args_p);

    // ----------------------------------------------
    // Entity selection:
    unsigned char id = 0;
    int x = (int)(INPUT.cursor_x * args->gbuffer->width);
    int y = (int)(INPUT.cursor_y * args->gbuffer->height);
    glBindFramebuffer(GL_FRAMEBUFFER, args->gbuffer->fbo);
    glReadPixels(x, y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &id);

    int entity = (int)id - 1;

    if (INPUT.mouse_left_released && args->mode != SELECT) {
        window_set_default_cursor();
        args->mode = SELECT;
        return;
    }
    
    entity = INPUT.mouse_left_released ? entity : args->entity;
    Material* material = get_material(entity);

    int prev_entity = args->entity;
    Material* prev_material = get_material(prev_entity);

    if (prev_material) {
        prev_material->constant_color = vec3_zeros;
    }
    if (material) {
        material->constant_color = args->select_color;
    }

    args->entity = entity;

    // ----------------------------------------------
    // Entity drag:
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
    Vec3 entity_proj_position = vec3_project(
        &entity_world_position, &vp_mat);

    if (args->mode != DRAG) {
        window_set_drag_cursor();
        args->mode = DRAG;
        args->drag_axis = VIEWPORT;
        args->cursor_x = 0.5 * (entity_proj_position.data[0] + 1.0);
        args->cursor_y = 0.5 * (entity_proj_position.data[1] + 1.0);
        args->entity_start_world_position = entity_world_position;
        args->entity_start_local_position = entity_transformation->position;
        return;
    } 

    if (INPUT.x_released || INPUT.y_released || INPUT.z_released) {
        int axis = key_to_axis(INPUT.last_released_key);
        args->drag_axis = args->drag_axis == axis ? VIEWPORT : axis;
        entity_transformation->position = args->entity_start_local_position;

        entity_world_position = ecs_get_world_position(entity);
        entity_proj_position = vec3_project(
            &entity_world_position, &vp_mat);
    }

    args->cursor_x += INPUT.cursor_dx;
    args->cursor_y += INPUT.cursor_dy;

    float cursor_proj_x = args->cursor_x * 2.0 - 1.0;
    float cursor_proj_y = args->cursor_y * 2.0 - 1.0;

    Vec3 entity_new_world_position;

    if (args->drag_axis == VIEWPORT) {
        Vec4 cursor_proj = {{
            cursor_proj_x, cursor_proj_y, entity_proj_position.data[2], 1.0
        }};
        Vec4 cursor_world_4 = mat4_vec4_mul(&vp_inv, &cursor_proj);
        entity_new_world_position  = vec4_to_vec3(&cursor_world_4);
        entity_new_world_position = vec3_scale(
            &entity_new_world_position, 1.0 / cursor_world_4.data[3]);
    } else {
        Vec3 cursor_near_proj_position = {{cursor_proj_x, cursor_proj_y, -1.0}};
        Vec3 cursor_near_world_position = vec3_project(&cursor_near_proj_position, &vp_inv);

        Vec3 cursor_far_proj = {{cursor_proj_x, cursor_proj_y, 1.0}};
        Vec3 cursor_far_world = vec3_project(&cursor_far_proj, &vp_inv);

        Vec3 p0 = args->entity_start_world_position;
        Vec3 p1 = args->entity_start_world_position;
        p1.data[args->drag_axis] += 1.0;

        Vec3 isect_p;
        int is_isect = get_two_vecs_nearest_point(
            &isect_p, &cursor_near_world_position, &cursor_far_world, &p0, &p1);

        if (!is_isect) {
            return;
        }

        entity_new_world_position = isect_p;
    }

    Mat4 origin_world_mat = ecs_get_origin_world_mat(entity);
    Mat4 origin_world_inv = mat4_inverse(&origin_world_mat);
    entity_transformation->position = vec3_project(
        &entity_new_world_position, &origin_world_inv);
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
