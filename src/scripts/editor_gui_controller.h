#pragma once
#include "../buffers/overlay_buffer.h"
#include "../components/script.h"
#include "../editor_gui.h"

typedef struct EditorGUIControllerArgs {
    OverlayBuffer* overlay_buffer;

    ButtonW* hot_button;
    ButtonW* active_button;
    InputW* hot_input;
    InputW* active_input;
} EditorGUIControllerArgs;

EditorGUIControllerArgs editor_gui_controller_create_default_args(
    OverlayBuffer* overlay_buffer
);

Script* editor_gui_controller_create_script(EditorGUIControllerArgs* args);
