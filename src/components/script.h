#pragma once
#include <stddef.h>


typedef struct Script {
    void (*update)(size_t, void*);
    void* args;
} Script;

Script* script_create(void (*update)(size_t, void*), void* args);

