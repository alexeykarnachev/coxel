void _mesh_selection_update() {
    if (INPUT.mouse_left_pressed) {
        SCENE.selected_mesh_id = SCENE.cursor_on_mesh_id;
    }
}

Script mesh_selection_create_script() {
    Script script;
    script.update = _mesh_selection_update;
    return script;
}
