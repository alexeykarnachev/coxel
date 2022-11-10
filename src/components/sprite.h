typedef struct Sprite {
    float tex_x;
    float tex_y;
    float tex_width;
    float tex_height;

    Texture* texture;
} Sprite;

Sprite _SPRITES_ARENA[1 << 14];
size_t _N_SPRITES = 0;


Sprite* sprite_create(
    float tex_x,
    float tex_y,
    float tex_width,
    float tex_height,
    Texture* texture
) {
    Sprite* sprite = &_SPRITES_ARENA[_N_SPRITES++];

    sprite->tex_x = tex_x;
    sprite->tex_y = tex_y;
    sprite->tex_width = tex_width;
    sprite->tex_height = tex_height;
    sprite->texture = texture;

    return sprite;
}
