#pragma once
#include "../buffers/gbuffer.h"
#include "../components/script.h"
#include "../la.h"

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

EditorEntityControllerArgs
editor_entity_controller_create_default_args(GBuffer* gbuffer);

Script* editor_entity_controller_create_script(EditorEntityControllerArgs* args
);
