#include "script.h"


static Script SCRIPTS_ARENA[1 << 14];
static size_t N_SCRIPTS = 0;


Script* script_create(void (*update)(size_t, void*), void* args) {
    Script* script = &SCRIPTS_ARENA[N_SCRIPTS++];

    script->update = update;
    script->args = args;

    return script;
}

