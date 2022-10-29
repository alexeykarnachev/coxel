typedef struct Sprite {
    size_t width;
    size_t height;
    size_t n_channels;
    unsigned char* data;
} Sprite;


bool sprite_create_from_file(Sprite* sprite, char* file_path) {
    clear_struct(sprite);

    int width, height, n_channels;
    unsigned char* data = stbi_load(file_path, &width, &height, &n_channels, 0);
    if (data) {
        if (n_channels != 3 && n_channels != 4) {
            fprintf(
                stderr, "ERROR: failed to create sprite with %d channels: %s\n",
                n_channels, file_path
            );
            free(data);
            return false;
        }
    } else {
        fprintf(stderr, "ERROR: failed to create sprite: %s\n", file_path);
        return false;
    }

    sprite->width = width;
    sprite->height = height;
    sprite->n_channels = n_channels;
    sprite->data = data;
    return true;
}
