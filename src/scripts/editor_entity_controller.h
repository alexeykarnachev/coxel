static int AXIS_X = 0;
static int AXIS_Y = 1;
static int AXIS_Z = 2;
static int AXIS_W = 3;

static int MODE_SELECT = 0;
static int MODE_DRAG = 1;
static int MODE_SCALE = 2;


typedef struct EditorEntityControllerArgs {
    int entity;

    Vec3 select_color;
    TextureBuffer* gbuffer;

    int mode;
    int axis;

    float cursor_x;
    float cursor_y;
    float cursor_start_x;
    float cursor_start_y;
    Vec3 entity_start_world_position;
    Vec3 entity_start_local_position;
    Mat4 entity_start_world_mat;
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
        return AXIS_X;
    } else if (key == 89) {
        return AXIS_Y;
    } else if (key == 90) {
        return AXIS_Z;
    }
    return AXIS_W;
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

    if (INPUT.mouse_left_released && args->mode != MODE_SELECT) {
        window_set_default_cursor();
        args->mode = MODE_SELECT;
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

    if (INPUT.x_released || INPUT.y_released || INPUT.z_released) {
        int axis = key_to_axis(INPUT.last_released_key);
        args->axis = args->axis == axis ? AXIS_W : axis;
        entity_transformation->translation = args->entity_start_local_position;

        entity_world_position = ecs_get_world_position(entity);
        entity_proj_position = vec3_project(
            &entity_world_position, &vp_mat);
    }

    Mat4 entity_world_mat = ecs_get_world_mat(entity);

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
            args->entity_start_world_position = entity_world_position;
            args->entity_start_world_mat = entity_world_mat;
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

            Vec3 p0 = args->entity_start_world_position;
            Vec3 p1 = args->entity_start_world_position;
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
            args->entity_start_world_position = entity_world_position;
            args->entity_start_local_position = entity_transformation->translation;
            args->entity_start_world_mat = entity_world_mat;
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
            &entity_xy, &cursor_start_xy);
        Vec2 entity_to_cursor = vec2_diff(
            &entity_xy, &cursor_xy);
        float len_start = vec2_length(&entity_to_cursor_start);
        float len_curr = vec2_length(&entity_to_cursor);
        float scale_factor = len_curr / len_start;

        Mat4 entity_new_world_mat = args->entity_start_world_mat;
        entity_new_world_mat.data[4] *= scale_factor;
        entity_new_world_mat.data[5] *= scale_factor;
        entity_new_world_mat.data[6] *= scale_factor;

        Mat4 origin_world_mat = ecs_get_origin_world_mat(entity);
        Mat4 origin_world_inv = mat4_inverse(&origin_world_mat);
        Mat4 entity_new_local_mat = mat4_mat4_mul(
            &origin_world_inv, &entity_new_world_mat);

        transformation_create_from_model_mat(
            entity_transformation, &entity_new_local_mat);
    }

}

EditorEntityControllerArgs editor_entity_controller_create_default_args(
    TextureBuffer* gbuffer
) {
    EditorEntityControllerArgs args;

    args.entity = -1;
    args.select_color = DEFAULT_SELECT_COLOR;
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
