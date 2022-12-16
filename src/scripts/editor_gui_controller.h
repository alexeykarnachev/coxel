#pragma once
#include "../buffers/overlay_buffer.h"
#include "../components/script.h"
#include "../components/gui_rect.h"
#include "../components/gui_text.h"
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
    if (type == GUI_WIDGET_PANE) {
        printf("PANE\n");
    } else if (type == GUI_WIDGET_BUTTON) {
        ButtonW* button = (ButtonW*)widget->pointer;
        GUIRect* rect = (GUIRect*)COMPONENTS[GUI_RECT_COMPONENT][button->rect];
        GUIText* text = (GUIText*)COMPONENTS[GUI_TEXT_COMPONENT][button->text];
        Vec3 position = ecs_get_world_position(button->rect);
        if (INPUT.is_mouse_left_released) {
            // gui_button_toggle(button)
            printf("toggle\n");
        }

    } else if (type == GUI_WIDGET_INPUT) {
        printf("INPUT\n");
    }

    // if (tag == GUI_BUTTON_WIDGET_T) {
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
