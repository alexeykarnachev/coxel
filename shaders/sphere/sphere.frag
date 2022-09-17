#version 460 core

in vec3 es_pos;
in vec3 es_normal;
in vec3 es_light_dir;
in float es_light_dist;

out vec4 f_color;

const float shininess = 40.0;
const float light_power = 40.0;
const vec3 light_color = vec3(1.0, 1.0, 1.0);
const vec3 ambient_color = vec3(0.0, 0.0, 0.0);
const vec3 diffuse_color = vec3(0.3, 0.3, 0.6);
const vec3 specular_color = vec3(1.0, 1.0, 1.0);

void main() {
    float lambertian = max(dot(-es_light_dir, es_normal), 0.0);
    float specular = 0.0;
    
    if (lambertian > 0) {
        vec3 view_dir = normalize(es_pos);        
        vec3 half_dir = -normalize(es_light_dir + view_dir);
        float specular_angle = max(dot(half_dir, es_normal), 0.0);
        specular = pow(specular_angle, shininess / 4.0);

        // vec3 reflect_dir = reflect(-es_light_dir, es_normal);
        // specular = pow(max(dot(view_dir, reflect_dir), 0.0), 8.0);
    }
    
    vec3 color = ambient_color + diffuse_color * lambertian * light_color * light_power / es_light_dist + specular_color * specular * light_color * light_power / es_light_dist;
    f_color = vec4(color, 1.0);
}
