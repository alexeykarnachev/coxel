#pragma once
#include "../buffers/overlay_buffer.h"
#include "../components/script.h"
#include "../window.h"

typedef struct EditorGUIControllerArgs {
    OverlayBuffer* overlay_buffer;
} EditorGUIControllerArgs;

void _editor_gui_controller_update(size_t _, void* args_p) {
    EditorGUIControllerArgs* args = (EditorGUIControllerArgs*)(args_p
    );

    unsigned char id = 0;
    int x = (int)(INPUT.cursor_x * args->overlay_buffer->width);
    int y = (int)(INPUT.cursor_y * args->overlay_buffer->height);
    glBindFramebuffer(GL_FRAMEBUFFER, args->overlay_buffer->fbo);
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &id);

    int entity = (int)id - 1;
}

EditorGUIControllerArgs editor_gui_controller_create_default_args(
    OverlayBuffer* overlay_buffer
) {
    EditorGUIControllerArgs args;
    args.overlay_buffer = overlay_buffer;
    return args;
}

Script* editor_gui_controller_create_script(
    EditorGUIControllerArgs* args
) {
    return script_create(_editor_gui_controller_update, args);
};
