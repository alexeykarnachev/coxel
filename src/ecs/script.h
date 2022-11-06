 typedef struct Script {
    void (*update)(void*);
    void* args;
 } Script;
