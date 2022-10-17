typedef struct PointLight {
    Vec3 world_pos;
    Vec3 color;
    float energy;
} PointLight;


void point_light_create(
    PointLight* point_light,
    Vec3 world_pos,
    Vec3 color,
    float energy
) {
    clear_struct(point_light);

    point_light->world_pos = world_pos;
    point_light->color = color;
    point_light->energy = energy;
}

