#pragma once
#include "../texture.h"


typedef struct Sprite {
    float tex_x;
    float tex_y;
    float tex_width;
    float tex_height;

    Texture* texture;
} Sprite;

Sprite* sprite_create(
    float tex_x,
    float tex_y,
    float tex_width,
    float tex_height,
    Texture* texture
);
