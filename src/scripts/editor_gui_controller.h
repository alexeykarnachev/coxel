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
    if (!ecs_is_component_enabled(entity, GUI_WIDGET_COMPONENT)) {
        return;
    }
    GUIWidget* widget = COMPONENTS[GUI_WIDGET_COMPONENT][entity];
    int type = widget->type;

    // if (tag == GUI_BUTTON_WIDGET_T) {
    //     // size_t rect;
    //     // size_t text;

    //     // int is_pushed;
    //     // int is_hot;
    //     GUIRect* rect = (GUIRect*)COMPONENTS[GUI_RECT_T][button.rect];
    //     GUIText* text = (GUIText*)COMPONENTS[GUI_TEXT_T][button.text];
    //     Vec3 position = ecs_get_world_position(button.rect);
    //     int was_active = (int)(ACTIVE_BUTTON == i);
    //     int is_hot = is_point_inside_rect(
    //         position.data[0],
    //         position.data[1],
    //         rect->width,
    //         rect->height,
    //         cursor_x,
    //         cursor_y
    //     );
    //     was_gui_interacted |= is_hot;

    //     if (is_hot) {
    //         new_entity_hot = button.rect;
    //     }

    //     int push_direction = 0;
    //     if (is_hot && INPUT.is_mouse_left_released) {
    //         push_direction = was_active ? -1 : 1;
    //     }

    //     if (is_hot && !was_active) {
    //         rect->color = BUTTON_HOT_COLOR;
    //         text->color = BUTTON_LABEL_HOT_COLOR;
    //     } else if (!was_active) {
    //         rect->color = BUTTON_COLD_COLOR;
    //         text->color = BUTTON_LABEL_COLD_COLOR;
    //     }

    //     if (push_direction == 1) {
    //         rect->color = BUTTON_ACTIVE_COLOR;
    //         text->color = BUTTON_LABEL_ACTIVE_COLOR;
    //         ACTIVE_BUTTON = i;
    //     } else if (push_direction == -1) {
    //         rect->color = BUTTON_HOT_COLOR;
    //         text->color = BUTTON_LABEL_HOT_COLOR;
    //         ACTIVE_BUTTON = -1;
    //     }
    // }
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
