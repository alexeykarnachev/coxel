#pragma once
#include "../buffers/overlay_buffer.h"
#include "../components/script.h"
#include "../editor_gui.h"

typedef struct EditorGUIControllerArgs {
    OverlayBuffer* overlay_buffer;

    int last_hot_tag;

    ButtonW* hot_button;
    ButtonW* active_button;
    InputW* hot_input;
    InputW* active_input;
    PaneW* hot_pane;
    PaneW* active_pane;
    ListW* hot_list;
    ButtonListW* hot_button_list;
} EditorGUIControllerArgs;

EditorGUIControllerArgs editor_gui_controller_create_default_args(
    OverlayBuffer* overlay_buffer
);

Script* editor_gui_controller_create_script(EditorGUIControllerArgs* args);
