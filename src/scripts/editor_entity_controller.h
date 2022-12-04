static int AXIS_X = 0;
static int AXIS_Y = 1;
static int AXIS_Z = 2;
static int AXIS_W = 3;

static int MODE_SELECT = 0;
static int MODE_DRAG = 1;
static int MODE_SCALE = 2;
static int MODE_ROTATE = 3;


typedef struct EditorEntityControllerArgs {
    int entity;

    GBuffer* gbuffer;

    int mode;
    int axis;

    float cursor_x;
    float cursor_y;
    float cursor_start_x;
    float cursor_start_y;
    Mat4 entity_start_world_mat;
    Mat4 entity_start_local_mat;
} EditorEntityControllerArgs;

static int key_to_axis(int key) {
    if (key == 88) {
        return AXIS_X;
    } else if (key == 89) {
        return AXIS_Y;
    } else if (key == 90) {
        return AXIS_Z;
    }
    return AXIS_W;
}

// TODO: Refactor this. Alot of repeated stuff could be factored out.
void _editor_entity_controller_update(size_t _, void* args_p) {
    EditorEntityControllerArgs* args = (EditorEntityControllerArgs*)(args_p);

    // ----------------------------------------------
    // Entity selection:
    unsigned char id = 0;
    int x = (int)(INPUT.cursor_x * args->gbuffer->width);
    int y = (int)(INPUT.cursor_y * args->gbuffer->height);
    glBindFramebuffer(GL_FRAMEBUFFER, args->gbuffer->fbo);
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &id);

    int entity = (int)id - 1;

    if (INPUT.mouse_left_released && args->mode != MODE_SELECT) {
        window_set_default_cursor();
        args->mode = MODE_SELECT;
        return;
    }
    
    entity = INPUT.mouse_left_released ? entity : args->entity;
    ecs_disable_component(args->entity, HAS_OUTLINE_T);
    ecs_enable_component(entity, HAS_OUTLINE_T);
    args->entity = entity;
    
    if (entity == -1) {
        return;
    }

    Transformation* entity_transformation = 
        (Transformation*)COMPONENTS[TRANSFORMATION_T][entity];
    if (!entity_transformation) {
        return;
    }

    int camera_entity = ecs_get_active_camera_entity();
    if (camera_entity == -1) {
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

    if (
        args->mode != MODE_SELECT
        && (INPUT.x_released || INPUT.y_released || INPUT.z_released)
    ) {
        int axis = key_to_axis(INPUT.last_released_key);
        args->axis = args->axis == axis ? AXIS_W : axis;
        transformation_create_from_model_mat(
            entity_transformation, &args->entity_start_local_mat);

        entity_world_position = ecs_get_world_position(entity);
        entity_proj_position = vec3_project(
            &entity_world_position, &vp_mat);
    }

    Mat4 entity_world_mat = ecs_get_world_mat(entity);
    Mat4 entity_local_mat = ecs_get_local_mat(entity);
    Mat4 wv_mat = mat4_mat4_mul(&view_mat, &entity_world_mat);

    if (INPUT.g_pressed || args->mode == MODE_DRAG) {
        // ----------------------------------------------
        // Entity drag:
        if (args->mode != MODE_DRAG) {
            window_set_drag_cursor();
            args->mode = MODE_DRAG;
            args->axis = AXIS_W;
            args->cursor_x = 0.5 * (entity_proj_position.data[0] + 1.0);
            args->cursor_y = 0.5 * (entity_proj_position.data[1] + 1.0);
            args->cursor_start_x = args->cursor_x;
            args->cursor_start_y = args->cursor_y;
            args->entity_start_world_mat = entity_world_mat;
            args->entity_start_local_mat = entity_local_mat;
            return;
        } 

        args->cursor_x += INPUT.cursor_dx;
        args->cursor_y += INPUT.cursor_dy;

        float cursor_proj_x = args->cursor_x * 2.0 - 1.0;
        float cursor_proj_y = args->cursor_y * 2.0 - 1.0;

        Vec3 entity_new_world_position;

        if (args->axis == AXIS_W) {
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

            Vec3 p0 = model_mat_extract_translation_vec(
                &args->entity_start_world_mat);
            Vec3 p1 = p0;
            p1.data[args->axis] += 1.0;

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
        entity_transformation->translation = vec3_project(
            &entity_new_world_position, &origin_world_inv);
    } else if (INPUT.s_pressed || args->mode == MODE_SCALE) {
        // ----------------------------------------------
        // Entity scale:
        if (args->mode != MODE_SCALE) {
            window_set_drag_cursor();
            args->mode = MODE_SCALE;
            args->axis = AXIS_W;
            args->cursor_x = INPUT.cursor_x;
            args->cursor_y = INPUT.cursor_y;
            args->cursor_start_x = args->cursor_x;
            args->cursor_start_y = args->cursor_y;
            args->entity_start_world_mat = entity_world_mat;
            args->entity_start_local_mat = entity_local_mat;
        }

        args->cursor_x += INPUT.cursor_dx;
        args->cursor_y += INPUT.cursor_dy;
        
        Vec2 cursor_start_xy = vec2(
            args->cursor_start_x * 2.0 - 1.0,
            args->cursor_start_y * 2.0 - 1.0
        );
        Vec2 cursor_xy = vec2(
            args->cursor_x * 2.0 - 1.0,
            args->cursor_y * 2.0 - 1.0
        );
        Vec2 entity_xy = vec3_to_vec2(&entity_proj_position);

        Vec2 entity_to_cursor_start = vec2_diff(
            &cursor_start_xy, &entity_xy);
        Vec2 entity_to_cursor = vec2_diff(
            &cursor_xy, &entity_xy);
        float len_start = vec2_length(&entity_to_cursor_start);
        float len_curr = vec2_length(&entity_to_cursor);
        float scale_factor = len_curr / len_start;

        Vec3 scale = vec3_ones;
        if (args->axis == AXIS_W) {
            scale = vec3_scale(&scale, scale_factor);
        } else {
            scale.data[args->axis] = scale_factor;
        }

        Mat4 entity_new_world_mat = model_mat_scale(
            args->entity_start_world_mat, &scale);

        Mat4 origin_world_mat = ecs_get_origin_world_mat(entity);
        Mat4 origin_world_inv = mat4_inverse(&origin_world_mat);
        Mat4 entity_new_local_mat = mat4_mat4_mul(
            &origin_world_inv, &entity_new_world_mat);

        transformation_create_from_model_mat(
            entity_transformation, &entity_new_local_mat);
    } else if (INPUT.r_pressed || args->mode == MODE_ROTATE) {
        // ----------------------------------------------
        // Entity rotate:
        if (args->mode != MODE_ROTATE) {
            window_set_drag_cursor();
            args->mode = MODE_ROTATE;
            args->axis = AXIS_W;
            args->cursor_x = INPUT.cursor_x;
            args->cursor_y = INPUT.cursor_y;
            args->cursor_start_x = args->cursor_x;
            args->cursor_start_y = args->cursor_y;
            args->entity_start_world_mat = entity_world_mat;
            args->entity_start_local_mat = entity_local_mat;
        }

        Vec2 cursor_prev_xy = vec2(
            args->cursor_x * 2.0 - 1.0,
            args->cursor_y * 2.0 - 1.0
        );
        args->cursor_x += INPUT.cursor_dx;
        args->cursor_y += INPUT.cursor_dy;
        Vec2 cursor_xy = vec2(
            args->cursor_x * 2.0 - 1.0,
            args->cursor_y * 2.0 - 1.0
        );
        Vec2 entity_xy = vec3_to_vec2(&entity_proj_position);

        Vec2 entity_to_cursor_prev = vec2_diff(
            &cursor_prev_xy, &entity_xy);
        Vec2 entity_to_cursor = vec2_diff(
            &cursor_xy, &entity_xy);

        float angle = vec2_angle(&entity_to_cursor, &entity_to_cursor_prev);
        
        Mat3 rotation_mat = mat3_identity;
        if (args->axis == AXIS_W) {
            rotation_mat = mat3_get_z_rotation(-angle);
            Transformation wv_t;
            transformation_create_from_model_mat(&wv_t, &wv_mat);
            wv_t.rotation_mat = mat3_mat3_mul(&rotation_mat, &wv_t.rotation_mat);
            Mat4 wv_mat_rot = transformation_get_model_mat(&wv_t);
            Mat4 entity_new_world_mat = mat4_mat4_mul(&view_inv, &wv_mat_rot);

            Mat4 origin_world_mat = ecs_get_origin_world_mat(entity);
            Mat4 origin_world_inv = mat4_inverse(&origin_world_mat);
            Mat4 entity_new_local_mat = mat4_mat4_mul(
                &origin_world_inv, &entity_new_world_mat);
            transformation_create_from_model_mat(
                entity_transformation, &entity_new_local_mat);
            return;
        } else if (args->axis == AXIS_X) {
            float sign = camera_is_codirected(
                camera, camera_transformation, &vec3_pos_x);
            rotation_mat = mat3_get_x_rotation(angle * sign);
        } else if (args->axis == AXIS_Y) {
            float sign = camera_is_codirected(
                camera, camera_transformation, &vec3_pos_y);
            rotation_mat = mat3_get_y_rotation(angle * sign);
        } else if (args->axis == AXIS_Z) {
            float sign = camera_is_codirected(
                camera, camera_transformation, &vec3_pos_z);
            rotation_mat = mat3_get_z_rotation(angle * sign);
        }

        Transformation w_t;
        transformation_create_from_model_mat(&w_t, &entity_world_mat);
        w_t.rotation_mat = mat3_mat3_mul(&rotation_mat, &w_t.rotation_mat);
        Mat4 entity_new_world_mat = transformation_get_model_mat(&w_t);

        Mat4 origin_world_mat = ecs_get_origin_world_mat(entity);
        Mat4 origin_world_inv = mat4_inverse(&origin_world_mat);
        Mat4 entity_new_local_mat = mat4_mat4_mul(
            &origin_world_inv, &entity_new_world_mat);
        transformation_create_from_model_mat(
            entity_transformation, &entity_new_local_mat);
    }
}

EditorEntityControllerArgs editor_entity_controller_create_default_args(
    GBuffer* gbuffer
) {
    EditorEntityControllerArgs args;

    args.entity = -1;
    args.gbuffer = gbuffer;
    args.mode = MODE_SELECT;
    args.axis = AXIS_W;

    return args;
}

Script* editor_entity_controller_create_script(
    EditorEntityControllerArgs* args
) {
    return script_create(_editor_entity_controller_update, args);
}
