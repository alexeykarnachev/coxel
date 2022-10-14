typedef struct Material {
    Vec4 diffuse_color = {1.0, 1.0, 1.0, 1.0};
    Vec4 ambient_color = {1.0, 1.0, 1.0, 1.0};
    Vec4 specular_color = {1.0, 1.0, 1.0, 1.0};
    float shininess = 256.0;
} Material;

