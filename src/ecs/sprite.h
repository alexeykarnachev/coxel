#define SPRITE_PACK_SIZE 32

typedef struct Sprite {
    float world_width;
    float world_height;

    float tex_x;
    float tex_y;
    float tex_width;
    float tex_height;

    GLuint tex;
} Sprite;


void sprite_pack(Sprite* sprite, float dst[]) {
    dst[0] = sprite->world_width;
    dst[1] = sprite->world_height;
    dst[2] = sprite->tex_x;
    dst[3] = sprite->tex_y;
    dst[4] = sprite->tex_width;
    dst[5] = sprite->tex_height;
}

