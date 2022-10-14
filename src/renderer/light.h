typedef struct PointLight {
    Vec3 world_pos = vec3_zeros();
    Vec3 color = vec3_zeros();
    float energy = 100.0f;
} PointLight;


void point_light_create(PointLight* point_light, Vec3 world_pos, Vec3 color, float energy) {
    clear_struct(point_light);

    point_light->world_pos = world_pos;
    point_light->color = color;
    point_light->energy = energy;
}
