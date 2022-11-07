typedef struct Script {
    void (*update)(size_t, void*);
    void* args;
} Script;

Script _SCRIPTS_ARENA[1 << 14];
size_t _N_SCRIPTS = 0;


Script* script_create(void (*update)(size_t, void*), void* args) {
    Script* script = &_SCRIPTS_ARENA[_N_SCRIPTS++];

    script->update = update;
    script->args = args;

    return script;
}


