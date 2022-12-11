#include "sprite.h"

Sprite SPRITES_ARENA[1 << 14];
size_t N_SPRITES = 0;

Sprite* sprite_create(
    float tex_x,
    float tex_y,
    float tex_width,
    float tex_height,
    Texture* texture
) {
    Sprite* sprite = &SPRITES_ARENA[N_SPRITES++];

    sprite->tex_x = tex_x;
    sprite->tex_y = tex_y;
    sprite->tex_width = tex_width;
    sprite->tex_height = tex_height;
    sprite->texture = texture;

    return sprite;
}
